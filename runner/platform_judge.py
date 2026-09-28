"""平台判题：本地编译/样例自测 + 真实 OJ 提交/黑盒反馈回读。

与 runner.judge.Judge 鸭子类型兼容（compile / run_sample / submit / cleanup），
agent.py 无需感知后端差异。默认洛谷，可选 codeforces。
"""
import json
import os
import tempfile
import time

from bs4 import BeautifulSoup

from . import sandbox
from .platforms.codeforces import CodeforcesBackend
from .platforms.luogu import LuoguBackend

_BACKENDS = {
    "luogu": LuoguBackend,
    "codeforces": CodeforcesBackend,
}

# 本地自测用宽松限制（正式时限以平台判题为准）
LOCAL_TL_MS = 10000
LOCAL_ML_MB = 1024


class PlatformJudge:
    def __init__(self, problem):
        self.p = problem
        if not problem.source:
            raise ValueError("该题没有 source 字段，不是平台题目")
        cls = _BACKENDS[problem.source["platform"]]
        self.backend = cls()
        self.lang = problem.source.get("lang", "")
        self._workdir = tempfile.mkdtemp(prefix="bench_plat_")
        self._last_exe_key = None
        self._last_exe = None
        self._ensure_samples()

    # ---------- 题面/样例缓存 ----------

    def _ensure_samples(self):
        if self.p.samples:
            return
        fetched = self.backend.fetch_problem(self.p.source)
        cache_path = os.path.join(self.p.dir, "platform_cache.json")
        with open(cache_path, "w", encoding="utf-8") as f:
            json.dump(fetched, f, ensure_ascii=False, indent=1)
        self.p.samples = fetched["samples"]
        text = BeautifulSoup(fetched["statement_html"], "html.parser").get_text("\n", strip=True)
        with open(os.path.join(self.p.dir, "statement.txt"), "w", encoding="utf-8") as f:
            f.write(text)
        if not self.p.statement or self.p.statement.startswith("（平台题"):
            self.p.statement = text[:6000]

    # ---------- 三工具 ----------

    def _get_exe(self, code: str):
        import hashlib
        h = hashlib.sha256(code.encode()).hexdigest()
        if h != self._last_exe_key:
            exe, err = sandbox.compile_cpp(code, self._workdir)
            self._last_exe_key, self._last_exe = h, (exe, err)
        return self._last_exe

    def compile(self, code: str) -> dict:
        exe, err = self._get_exe(code)
        if exe is None:
            return {"ok": False, "errors": err}
        return {"ok": True, "hint": "编译通过（-O2 -std=c++17，本地）"}

    def run_sample(self, code: str) -> dict:
        exe, err = self._get_exe(code)
        if exe is None:
            return {"ok": False, "errors": err}
        runs = []
        for i, s in enumerate(self.p.samples):
            r = sandbox.run_case(exe, s["input"], LOCAL_TL_MS, LOCAL_ML_MB)
            verdict = r["verdict"]
            if verdict == "AC":
                if sandbox.normalize_output(r["stdout"]) != sandbox.normalize_output(s["output"]):
                    verdict = "WA"
            runs.append({
                "sample": i + 1,
                "input": s["input"],
                "expected": s["output"],
                "your_output": r["stdout"] if verdict in ("AC", "WA") else "",
                "verdict": verdict,
                "note": "本地自测，正式时限以平台为准",
            })
        return {"ok": True, "runs": runs}

    def submit(self, code: str) -> dict:
        t0 = time.time()
        try:
            rid = self.backend.submit(code, self.lang, self.p.source)
        except Exception as e:  # 平台网络/风控问题不应崩实验
            return {"ok": False, "errors": f"平台提交失败：{e}", "compile_failed": False}
        result = self.backend.wait_result(rid, self.p.source)
        passed, total = result.get("passed"), result.get("total")
        return {
            "ok": True,
            "passed": passed,
            "total": total,
            "all_ac": result["verdict"] == "AC",
            "detail": f"[{self.p.source['platform']}] {result['detail']}"
                      f"（往返 {time.time() - t0:.0f}s）",
            "verdict": result["verdict"],
        }

    def cleanup(self):
        import shutil
        shutil.rmtree(self._workdir, ignore_errors=True)


def open_judge(problem, backend_hint: str | None = None):
    """按题目来源选择判题器：平台题 → PlatformJudge；本地题 → 本地 Judge。"""
    if backend_hint == "local" and not problem.source:
        from .judge import Judge
        return Judge(problem)
    if problem.source:
        return PlatformJudge(problem)
    from .judge import Judge
    return Judge(problem)
