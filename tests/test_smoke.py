"""冒烟测试：不需要任何 API key / Cookie，用 Mock 后端 + Mock LLM 跑通全链路。

运行：python tests/test_smoke.py
覆盖：题目加载（洛谷 source）→ LuoguJudge 样例缓存 → agent 工具循环 →
      洛谷提交/黑盒反馈格式 → 轨迹记录。
"""
import json
import os
import shutil
import sys
import tempfile
from types import SimpleNamespace

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from runner.agent import AgentRunner
from runner.platform_judge import LuoguJudge
from runner.platforms.base import PlatformBackend
from runner.problem import Problem
from runner.session import Session


class MockLuoguBackend(PlatformBackend):
    """第一次提交返回 WA（部分点），第二次返回 AC。"""

    name = "mock"

    def __init__(self):
        self.rid = 0

    def fetch_problem(self, ref):
        return {"statement_html": "<p>求逆序对数量。</p>",
                "samples": [{"input": "3\n3 1 2\n", "output": "2\n"}],
                "time_limit_ms": 1000, "memory_limit_mb": 128}

    def submit(self, code, lang_name, ref):
        self.rid += 1
        return str(self.rid)

    def wait_result(self, rid, ref, timeout_s=600):
        if rid == "1":
            return {"verdict": "WA", "passed": 4, "total": 20,
                    "detail": "WA（通过测试点 4/20）"}
        return {"verdict": "AC", "passed": 20, "total": 20,
                "detail": "AC（通过测试点 20/20）"}


def _make_llm_client(bad_code, good_code):
    calls = [0]

    def create(**kw):
        calls[0] += 1
        code = bad_code if calls[0] == 1 else good_code
        tc = SimpleNamespace(
            id=f"call_{calls[0]}",
            function=SimpleNamespace(name="submit",
                                     arguments=json.dumps({"code": code})),
        )
        msg = SimpleNamespace(tool_calls=[tc], content=None)
        return SimpleNamespace(choices=[SimpleNamespace(message=msg)],
                               usage=SimpleNamespace(prompt_tokens=10,
                                                     completion_tokens=5))

    return SimpleNamespace(chat=SimpleNamespace(
        completions=SimpleNamespace(create=create))), calls


def main():
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    tmp = tempfile.mkdtemp(prefix="smoke_prob_")

    # 1) 构造平台题目（samples 为空 → 触发从 Mock 后端拉取并缓存）
    with open(os.path.join(tmp, "problem.json"), "w", encoding="utf-8") as f:
        json.dump({"id": "smoke", "category": "classic", "title": "冒烟",
                   "samples": [],
                   "source": {"platform": "luogu", "pid": "P0000"}}, f)
    p = Problem(tmp)

    # 手工组装 LuoguJudge，替换后端为 Mock（绕开需要 Cookie 的真实后端）
    j = LuoguJudge.__new__(LuoguJudge)
    j.p = p
    j.lang = ""
    j.backend = MockLuoguBackend()
    j._workdir = tempfile.mkdtemp(prefix="smoke_judge_")
    j._exe_key = None
    j._exe = (None, None)
    j._ensure_samples()

    assert len(p.samples) == 1, "样例应已从 Mock 后端拉取"
    assert os.path.exists(os.path.join(tmp, "platform_cache.json"))
    assert os.path.exists(os.path.join(tmp, "statement.txt"))

    prob_dir = os.path.join(root, "problems", "classic-inv-pairs")
    with open(os.path.join(prob_dir, "solution.cpp"), encoding="utf-8") as f:
        good = f.read()
    with open(os.path.join(prob_dir, "mutants", "boundary.cpp"), encoding="utf-8") as f:
        bad = f.read()

    client, calls = _make_llm_client(bad, good)
    sess = Session(os.path.join(root, "results", "raw"), p.id, "mock:model", "A")
    final, usage, submits = AgentRunner(j, client, "mock-model", phase="A").run(sess)
    j.cleanup()

    print("final =", final, "| submits =", submits)
    assert final["status"] == "fixed" and submits == 2 and calls[0] == 2

    with open(sess.file, encoding="utf-8") as f:
        rounds = [json.loads(ln) for ln in f if ln.strip() and '"round"' in ln]
    assert len(rounds) == 2
    first = rounds[0]["tool_result"]
    assert first["ok"] and not first["all_ac"] and "WA" in first["detail"], first
    second = rounds[1]["tool_result"]
    assert second["all_ac"] and "20/20" in second["detail"], second

    shutil.rmtree(tmp, ignore_errors=True)
    print("冒烟测试通过 ✓")


if __name__ == "__main__":
    main()
