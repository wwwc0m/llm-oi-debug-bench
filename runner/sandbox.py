"""沙箱：编译 C++、限时限内存地运行被测程序。"""
import os
import subprocess
import threading
import time

import psutil

COMPILE_TIMEOUT_S = 30
GXX = os.environ.get("BENCH_GXX", "g++")


def compile_cpp(code: str, workdir: str, name: str = "main"):
    """编译代码，返回 (exe路径, None) 或 (None, 错误信息)。"""
    src = os.path.join(workdir, f"{name}.cpp")
    exe = os.path.join(workdir, f"{name}.exe")
    with open(src, "w", encoding="utf-8", newline="\n") as f:
        f.write(code)
    try:
        r = subprocess.run(
            [GXX, "-O2", "-std=c++17", "-o", exe, src],
            capture_output=True, text=True, timeout=COMPILE_TIMEOUT_S,
        )
    except subprocess.TimeoutExpired:
        return None, f"编译超时（>{COMPILE_TIMEOUT_S}s）"
    except FileNotFoundError:
        return None, f"找不到编译器 {GXX}，请确认 MinGW 已安装并在 PATH"
    if r.returncode != 0:
        return None, r.stderr[-4000:]
    return exe, None


def normalize_output(s: str) -> str:
    """OI 惯例：忽略每行行尾空白与文末空行。"""
    lines = [ln.rstrip() for ln in s.replace("\r\n", "\n").split("\n")]
    while lines and lines[-1] == "":
        lines.pop()
    return "\n".join(lines)


def run_case(exe: str, input_text: str, tl_ms: int, ml_mb: int):
    """运行单个测试点。

    返回 dict(verdict, stdout, elapsed_ms, peak_mem_mb)。
    verdict 优先级：TLE / RE /（输出比对）AC|WA，最后内存超限覆盖为 MLE。
    """
    try:
        p = psutil.Popen(
            [exe],
            stdin=subprocess.PIPE, stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL, text=True,
            cwd=os.path.dirname(exe),
        )
    except OSError as e:
        return {"verdict": "RE", "stdout": "", "elapsed_ms": 0.0,
                "peak_mem_mb": 0.0, "note": str(e)}

    peak_mb = [0.0]

    def poll_memory():
        # Windows 下 psutil 提供 peak_wset，轮询兜底防止进程过快退出
        try:
            while p.is_running():
                info = p.memory_info()
                rss = getattr(info, "peak_wset", 0) or info.rss
                peak_mb[0] = max(peak_mb[0], rss / 1024 / 1024)
                time.sleep(0.002)
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            pass

    t = threading.Thread(target=poll_memory, daemon=True)
    t.start()

    verdict, stdout = "AC", ""
    start = time.perf_counter()
    try:
        stdout, _ = p.communicate(input_text, timeout=tl_ms / 1000)
        if p.returncode != 0:
            verdict = "RE"
    except subprocess.TimeoutExpired:
        for proc in [p] + p.children(recursive=True):
            try:
                proc.kill()
            except psutil.Error:
                pass
        p.communicate()
        verdict = "TLE"
    elapsed_ms = (time.perf_counter() - start) * 1000
    t.join(timeout=1.0)

    if verdict == "AC" and peak_mb[0] > ml_mb:
        verdict = "MLE"

    return {"verdict": verdict, "stdout": stdout or "",
            "elapsed_ms": round(elapsed_ms, 1),
            "peak_mem_mb": round(peak_mb[0], 1)}
