"""洛谷判题：题面/样例用 requests 匿名拉取；提交与结果用真实浏览器。"""

import hashlib
import json
import os
import shutil
import tempfile
import time

from . import sandbox
from .browser_backend import LuoguBrowser
from .platforms.luogu import LuoguBackend

# 本地自测用宽松限制（正式时限以洛谷判题为准）
LOCAL_TL_MS = 10000
LOCAL_ML_MB = 1024


class LuoguJudge:
    def __init__(self, problem):
        self.p = problem
        if not problem.source or problem.source.get("platform") != "luogu":
            raise ValueError("题目缺少 source 字段：{platform: luogu, pid: ...}")
        self.fetcher = LuoguBackend()
        self.browser = LuoguBrowser()
        self.lang = problem.source.get("lang", "28")
        self._workdir = tempfile.mkdtemp(prefix="bench_lg_")
        self._exe_key = None
        self._exe = (None, None)
        self._ensure_samples()

    # ---------- 题面/样例缓存 ----------

    def _ensure_samples(self):
        if self.p.samples:
            return
        fetched = self.fetcher.fetch_problem(self.p.source)
        cache_path = os.path.join(self.p.dir, "platform_cache.json")
        with open(cache_path, "w", encoding="utf-8") as f:
            json.dump(fetched, f, ensure_ascii=False, indent=1)
        self.p.samples = fetched["samples"]
        md = fetched["statement_md"]
        with open(os.path.join(self.p.dir, "statement.md"), "w", encoding="utf-8") as f:
            f.write(md)
        self.p.statement = self.p.statement or md[:6000]

    # ---------- 三工具 ----------

    def _get_exe(self, code: str):
        h = hashlib.sha256(code.encode()).hexdigest()
        if h != self._exe_key:
            self._exe = sandbox.compile_cpp(code, self._workdir)
            self._exe_key = h
        return self._exe

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
                "note": "本地自测，正式时限以洛谷为准",
            })
        return {"ok": True, "runs": runs}

    def submit(self, code: str) -> dict:
        t0 = time.time()
        try:
            rid = self.browser.submit(code, self.lang, self.p.source)
        except Exception as e:  # 网络/风控问题不应崩实验
            return {"ok": False, "errors": f"洛谷提交失败：{e}"}
        result = self.browser.wait_result(rid, self.p.source)
        passed, total = result.get("passed"), result.get("total")
        return {
            "ok": True,
            "passed": passed,
            "total": total,
            "all_ac": result["verdict"] == "AC",
            "verdict": result["verdict"],
            "detail": f"{result['detail']}（往返 {time.time() - t0:.0f}s）",
        }

    def cleanup(self):
        shutil.rmtree(self._workdir, ignore_errors=True)
        try:
            self.browser.close()
        except Exception:
            pass
