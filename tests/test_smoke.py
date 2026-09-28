"""冒烟测试：不需要 API key / Cookie，Mock 后端 + Mock LLM 跑通全链路。

运行：python tests/test_smoke.py
覆盖：题目加载 → 样例缓存 → 本地预检门（编译/样例退回不计次）→
浏览器提交/逐测试点反馈 → 每轮代码与结果落盘（只记真实提交）。
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

BAD_COMPILE = """#include <bits/stdc++.h>
int main() { this is not valid c++ ; }
"""

# 参考解与 boundary 变体（原 P1908 逆序对，改为内置，避免依赖题库内容）
GOOD_CODE = """#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
int n;
vector<int> a, tmp;
ll count_inv(int l, int r) {
    if (r - l <= 1) return 0;
    int m = (l + r) / 2;
    ll res = count_inv(l, m) + count_inv(m, r);
    int i = l, j = m, k = l;
    while (i < m || j < r) {
        if (j >= r || (i < m && a[i] <= a[j])) tmp[k++] = a[i++];
        else { res += m - i; tmp[k++] = a[j++]; }
    }
    for (int t = l; t < r; t++) a[t] = tmp[t];
    return res;
}
int main() {
    scanf("%d", &n);
    a.resize(n); tmp.resize(n);
    for (auto &x : a) scanf("%d", &x);
    printf("%lld\\n", count_inv(0, n));
    return 0;
}
"""

BOUNDARY_CODE = GOOD_CODE.replace("a[i] <= a[j]", "a[i] < a[j]")


class MockFetcher(PlatformBackend):
    name = "mock"

    def fetch_problem(self, ref):
        # 样例含相等元素：boundary 变体（相等计逆序对）在本地样例即翻车
        return {"statement_md": "# 求逆序对数量\n\n给定序列求逆序对。",
                "samples": [{"input": "3\n5 5 2\n", "output": "2\n"}],
                "time_limit_ms": 1000, "memory_limit_mb": 128}

    def submit(self, code, lang_name, ref):
        raise NotImplementedError

    def wait_result(self, rid, ref, timeout_s=600):
        raise NotImplementedError


class MockBrowser:
    """到达浏览器的提交都已过本地预检门，直接返回 AC。"""

    def submit(self, code, lang, ref):
        return "1"

    def wait_result(self, rid, ref, timeout_s=300):
        return {"verdict": "AC", "passed": 20, "total": 20,
                "detail": "AC（通过测试点 20/20）", "points": []}

    def close(self):
        pass


def _make_llm_client(codes):
    """按给定代码序列依次返回 submit 工具调用。"""
    calls = [0]

    def create(**kw):
        i = min(calls[0], len(codes) - 1)
        calls[0] += 1
        tc = SimpleNamespace(
            id=f"call_{calls[0]}",
            function=SimpleNamespace(name="submit",
                                     arguments=json.dumps({"code": codes[i]})),
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
    out_dir = os.path.join(tmp, "out")

    with open(os.path.join(tmp, "problem.json"), "w", encoding="utf-8") as f:
        json.dump({"id": "smoke", "category": "classic", "title": "冒烟",
                   "samples": [],
                   "source": {"platform": "luogu", "pid": "P0000"}}, f)
    p = Problem(tmp)

    # 手工组装 LuoguJudge（绕开真实网络）
    j = LuoguJudge.__new__(LuoguJudge)
    j.p = p
    j.lang = "28"
    j.fetcher = MockFetcher()
    j.browser = MockBrowser()
    import tempfile as _tf
    j._workdir = _tf.mkdtemp(prefix="smoke_judge_")
    j._exe_key = None
    j._exe = (None, None)
    j._ensure_samples()

    assert len(p.samples) == 1, "样例应已从 Mock 后端拉取"
    assert os.path.exists(os.path.join(tmp, "platform_cache.json"))
    assert os.path.exists(os.path.join(tmp, "statement.md"))

    good, boundary = GOOD_CODE, BOUNDARY_CODE

    # 场景：①语法错误→编译门退回（不计次）②boundary→样例门退回（计次！协议v2）③正解→过门提交 AC
    client, calls = _make_llm_client([BAD_COMPILE, boundary, good])
    sess = Session(out_dir, p.id, "mock:model", "A")
    runner = AgentRunner(j, client, "mock-model", phase="A", out_dir=out_dir)
    final, usage, submits = runner.run(sess)
    j.cleanup()

    print("final =", final, "| submits =", submits)
    assert final["status"] == "fixed", final
    assert submits == 2, f"样例退回应计次：1(退回)+1(AC)=2，实际 {submits}"
    assert calls[0] == 3

    # 落盘：样例退回占 round1（含样例数据），正解是 round2
    r1_code = open(os.path.join(out_dir, "round1.cpp"), encoding="utf-8").read()
    assert r1_code == boundary, "样例退回应作为 round1 记录"
    r1 = json.load(open(os.path.join(out_dir, "round1.result.json"), encoding="utf-8"))
    assert r1.get("local_gate") == "samples" and "样例" in r1.get("errors", ""), r1
    r2_code = open(os.path.join(out_dir, "round2.cpp"), encoding="utf-8").read()
    assert r2_code == good
    r2 = json.load(open(os.path.join(out_dir, "round2.result.json"), encoding="utf-8"))
    assert r2["all_ac"] and "20/20" in r2["detail"], r2

    # 轨迹：编译退回（不计次）+ 样例退回（计次）+ 真实提交
    with open(sess.file, encoding="utf-8") as f:
        rounds = [json.loads(ln) for ln in f if ln.strip() and '"round"' in ln]
    actions = [r["action"] for r in rounds]
    assert actions == ["local_gate:compile", "local_gate:samples", "submit"], actions

    shutil.rmtree(tmp, ignore_errors=True)
    print("冒烟测试通过 ✓（样例退回计次+样例数据落盘）")


if __name__ == "__main__":
    main()
