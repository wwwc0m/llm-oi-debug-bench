"""题目加载：题面 / 样例 / 来源信息。判题一律走洛谷。"""
import json
import os


class Problem:
    def __init__(self, problem_dir: str):
        self.dir = os.path.abspath(problem_dir)
        with open(os.path.join(self.dir, "problem.json"), encoding="utf-8") as f:
            meta = json.load(f)
        self.id = meta["id"]
        self.category = meta.get("category", "classic")
        self.title = meta["title"]
        self.statement = meta.get("statement") or ""
        if not self.statement:  # 题面缓存在 statement.md（fetch 产物）
            md = os.path.join(self.dir, "statement.md")
            if os.path.exists(md):
                with open(md, encoding="utf-8") as f:
                    self.statement = f.read()
        self.time_limit_ms = meta.get("time_limit_ms")
        self.memory_limit_mb = meta.get("memory_limit_mb")
        self.samples = meta.get("samples") or []
        self.source = meta.get("source")
        sol = os.path.join(self.dir, "assets", "solution.cpp")
        if not os.path.exists(sol):
            sol = os.path.join(self.dir, "solution.cpp")
        if os.path.exists(sol):
            with open(sol, encoding="utf-8") as f:
                self.reference_solution = f.read()
        else:
            self.reference_solution = ""
