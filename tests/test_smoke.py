"""冒烟测试：不需要任何 API key，用 Mock LLM 客户端跑通 agent 全链路。

运行：python tests/test_smoke.py
覆盖：agent 工具调用循环 → 本地判题 → 黑盒反馈格式 → 轨迹记录。
"""
import json
import os
import sys
from types import SimpleNamespace

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from runner.agent import AgentRunner
from runner.judge import Judge, Problem
from runner.session import Session


def _make_client(bad_code: str, good_code: str):
    """第一次 create 返回提交 bad_code 的工具调用，之后返回 good_code。"""
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
        usage = SimpleNamespace(prompt_tokens=10, completion_tokens=5)
        return SimpleNamespace(choices=[SimpleNamespace(message=msg)], usage=usage)

    client = SimpleNamespace(
        chat=SimpleNamespace(completions=SimpleNamespace(create=create)))
    return client, calls


def main():
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    prob_dir = os.path.join(root, "problems", "classic-inv-pairs")
    p = Problem(prob_dir)
    with open(os.path.join(prob_dir, "solution.cpp"), encoding="utf-8") as f:
        good = f.read()
    with open(os.path.join(prob_dir, "mutants", "boundary.cpp"), encoding="utf-8") as f:
        bad = f.read()

    client, calls = _make_client(bad, good)
    j = Judge(p)
    sess = Session(os.path.join(root, "results", "raw"), p.id, "mock:model", "A")
    final, usage, submits = AgentRunner(j, client, "mock-model", phase="A").run(sess)
    j.cleanup()

    print("final =", final, "| submits =", submits)
    print("trajectory =", sess.file)
    assert final["status"] == "fixed", f"期望第二次提交后 fixed，实际 {final}"
    assert submits == 2 and calls[0] == 2
    assert usage["prompt_tokens"] == 20  # 两次调用各 10，累计

    with open(sess.file, encoding="utf-8") as f:
        lines = [json.loads(ln) for ln in f if ln.strip()]
    rounds = [l for l in lines if "round" in l]
    assert len(rounds) == 2, f"轨迹应有 2 轮，实际 {len(rounds)}"
    # 第一轮应为 WA（boundary 变体），黑盒反馈里应含"未通过"
    assert "未通过" in json.dumps(rounds[0]["tool_result"], ensure_ascii=False) or \
           rounds[0]["tool_result"].get("all_ac") is False
    print("冒烟测试通过 ✓")


if __name__ == "__main__":
    main()
