"""判题器：加载题目、组织三工具（compile / run_sample / submit）。"""
import glob
import hashlib
import json
import os
import tempfile
import shutil

from . import sandbox


class Problem:
    """一个 OI 题目：题面 + 样例 + 隐藏测试。"""

    def __init__(self, problem_dir: str, require_hidden: bool = True):
        self.dir = os.path.abspath(problem_dir)
        with open(os.path.join(self.dir, "problem.json"), encoding="utf-8") as f:
            meta = json.load(f)
        self.id = meta["id"]
        self.category = meta["category"]
        self.title = meta["title"]
        self.statement = meta["statement"]
        self.time_limit_ms = meta["time_limit_ms"]
        self.memory_limit_mb = meta["memory_limit_mb"]
        self.samples = meta["samples"]  # [{"input": ..., "output": ...}]
        with open(os.path.join(self.dir, "solution.cpp"), encoding="utf-8") as f:
            self.reference_solution = f.read()
        # build 命令生成测试之前，允许以空测试集加载
        self.hidden = self._load_hidden() if require_hidden else []

    def _load_hidden(self):
        cases = []
        for in_path in sorted(glob.glob(os.path.join(self.dir, "tests", "*.in"))):
            ans_path = os.path.splitext(in_path)[0] + ".ans"
            if not os.path.exists(ans_path):
                raise FileNotFoundError(f"缺少答案文件：{ans_path}（先跑 cli build）")
            with open(in_path, encoding="utf-8") as f:
                inp = f.read()
            with open(ans_path, encoding="utf-8") as f:
                ans = f.read()
            cases.append({
                "name": os.path.splitext(os.path.basename(in_path))[0],
                "input": inp,
                "answer": ans,
            })
        if not cases:
            raise FileNotFoundError(f"{self.dir}/tests/ 下没有任何 .in 文件")
        return cases


class Judge:
    """对一份代码执行判题；对相同代码缓存编译结果。"""

    def __init__(self, problem: Problem):
        self.p = problem
        self._cache = {}   # code_hash -> (exe, compile_error)
        self._workdir = tempfile.mkdtemp(prefix="bench_")

    # ---------- 内部 ----------

    def _get_exe(self, code: str):
        h = hashlib.sha256(code.encode()).hexdigest()
        if h not in self._cache:
            exe, err = sandbox.compile_cpp(code, self._workdir)
            self._cache[h] = (exe, err)
        return self._cache[h]

    def _run_cases(self, exe, cases, show_answers: bool):
        results = []
        for c in cases:
            r = sandbox.run_case(exe, c["input"],
                                 self.p.time_limit_ms, self.p.memory_limit_mb)
            verdict = r["verdict"]
            if verdict == "AC":
                got = sandbox.normalize_output(r["stdout"])
                want = sandbox.normalize_output(c["answer"])
                if got != want:
                    verdict = "WA"
            item = {"case": c["name"], "verdict": verdict,
                    "elapsed_ms": r["elapsed_ms"], "peak_mem_mb": r["peak_mem_mb"]}
            if verdict == "WA" and show_answers:
                item["expected_tail"] = want[-500:]
                item["your_output_tail"] = got[-500:]
            results.append(item)
        return results

    # ---------- agent 可调用的三个工具 ----------

    def compile(self, code: str) -> dict:
        exe, err = self._get_exe(code)
        if exe is None:
            return {"ok": False, "errors": err}
        return {"ok": True, "hint": "编译通过（-O2 -std=c++17）"}

    def run_sample(self, code: str) -> dict:
        """在题面给出的可见样例上运行（样例输入与期望输出对选手可见）。"""
        exe, err = self._get_exe(code)
        if exe is None:
            return {"ok": False, "errors": err}
        runs = []
        for i, s in enumerate(self.p.samples):
            r = sandbox.run_case(exe, s["input"],
                                 self.p.time_limit_ms, self.p.memory_limit_mb)
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
            })
        return {"ok": True, "runs": runs}

    def submit(self, code: str) -> dict:
        """正式提交：只返回每个测试点的 通过/未通过 与判定（WA/TLE/MLE/RE），
        不返回测试点内容。这是本实验的“黑盒反馈”。"""
        exe, err = self._get_exe(code)
        if exe is None:
            return {"ok": False, "errors": err, "compile_failed": True}
        results = self._run_cases(exe, self.p.hidden, show_answers=False)
        passed = sum(1 for r in results if r["verdict"] == "AC")
        return {"ok": True, "passed": passed, "total": len(results),
                "all_ac": passed == len(results), "results": results}

    def cleanup(self):
        shutil.rmtree(self._workdir, ignore_errors=True)
