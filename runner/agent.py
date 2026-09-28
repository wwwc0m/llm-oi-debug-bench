"""LLM 选手：function-calling 循环，模拟 OI 黑盒调试。"""
import json

from .judge import Judge
from .session import Session

MAX_SUBMITS = 10          # 最多提交轮数 K
KEEP_LAST_MSGS = 24       # 历史过长时截断，只保留最近若干条消息

SYSTEM_PROMPT = """你是一名参加 OI（信息学奥赛）式比赛的选手。规则：
1. 题目用 C++17 解决，判题机编译命令为 g++ -O2 -std=c++17。
2. 你可以用工具：compile（编译并看报错）、run_sample（在题面样例上自测）、submit（正式提交）。
3. submit 的反馈是黑盒的：只知道每个测试点通过与否，未通过的标注 WA/TLE/MLE/RE，看不到测试点内容。
4. 正式提交最多 {K} 次，用完即止。请先本地自测有把握再提交。
5. 每次修改代码后，把完整的当前代码传给工具（工具会自动复用编译缓存）。""".strip()

PHASE_B_EXTRA = """场上已有一份能编译但判定不完全正确的代码（见用户消息末尾）。
你的任务：调试并修复它。允许小修，也允许重写。"""


def _tools_schema():
    return [
        {"type": "function", "function": {
            "name": "compile",
            "description": "编译当前代码，返回编译结果或报错全文",
            "parameters": {"type": "object",
                           "properties": {"code": {"type": "string", "description": "完整 C++ 源代码"}},
                           "required": ["code"]}}},
        {"type": "function", "function": {
            "name": "run_sample",
            "description": "在题面给出的可见样例上运行当前代码，返回输入/期望输出/实际输出",
            "parameters": {"type": "object",
                           "properties": {"code": {"type": "string", "description": "完整 C++ 源代码"}},
                           "required": ["code"]}}},
        {"type": "function", "function": {
            "name": "submit",
            "description": "正式提交。返回每个测试点的 通过/未通过 及判定（黑盒，无测试点内容）",
            "parameters": {"type": "object",
                           "properties": {"code": {"type": "string", "description": "完整 C++ 源代码"}},
                           "required": ["code"]}}},
    ]


def _fmt_submit(res: dict) -> str:
    lines = [f"提交结果：{res['passed']}/{res['total']} 个测试点通过。"]
    for r in res["results"]:
        lines.append(f"  测试点 {r['case']}: {'通过' if r['verdict'] == 'AC' else '未通过（' + r['verdict'] + '）'}")
    return "\n".join(lines)


def _truncate(messages):
    """保留 system+user 前缀与最近消息；截断点必须落在 tool 回复序列之外，
    否则 tool 消息与对应的 tool_calls 断开会导致 API 报错。"""
    tail = messages[-KEEP_LAST_MSGS:]
    cut = 0
    while cut < len(tail) and tail[cut].get("role") == "tool":
        cut += 1
    return messages[:2] + tail[cut:]


class AgentRunner:
    def __init__(self, judge: Judge, client, model: str,
                 phase: str = "A", starter_code: str | None = None):
        self.judge = judge
        self.client = client
        self.model = model
        self.phase = phase
        self.starter_code = starter_code

    def run(self, session: Session):
        p = self.judge.p
        user_prompt = (
            f"【题目】{p.title}\n\n{p.statement}\n\n"
            f"【时间限制】{p.time_limit_ms}ms；【内存限制】{p.memory_limit_mb}MB\n\n"
            f"【示例输入/输出】\n" + "\n".join(
                f"样例{i+1}：\n输入：\n{s['input']}\n输出：\n{s['output']}"
                for i, s in enumerate(p.samples))
        )
        if self.phase == "B" and self.starter_code:
            user_prompt += "\n\n【场上已有代码（判定不完全正确，请修复）】\n```cpp\n" + self.starter_code + "\n```"

        sys_content = SYSTEM_PROMPT.format(K=MAX_SUBMITS)
        if self.phase == "B":
            sys_content += "\n\n" + PHASE_B_EXTRA

        messages = [
            {"role": "system", "content": sys_content},
            {"role": "user", "content": user_prompt},
        ]

        submits, usage_total = 0, {"prompt_tokens": 0, "completion_tokens": 0}
        final = {"status": "no_submit", "rounds_used": 0}

        for step in range(60):  # 硬上限：防止死循环
            resp = self.client.chat.completions.create(
                model=self.model, messages=messages,
                tools=_tools_schema(), temperature=0.7,
            )
            msg = resp.choices[0].message
            u = resp.usage
            usage_total["prompt_tokens"] += getattr(u, "prompt_tokens", 0) or 0
            usage_total["completion_tokens"] += getattr(u, "completion_tokens", 0) or 0

            if msg.tool_calls:
                messages.append(msg)
                for tc in msg.tool_calls:
                    code = json.loads(tc.function.arguments).get("code", "")
                    name = tc.function.name
                    if name == "compile":
                        result = self.judge.compile(code)
                    elif name == "run_sample":
                        result = self.judge.run_sample(code)
                    elif name == "submit":
                        submits += 1
                        result = self.judge.submit(code)
                        result = {k: v for k, v in result.items() if k != "results"} | \
                                 {"detail": _fmt_submit(result)} if result.get("ok") else result
                    else:
                        result = {"ok": False, "errors": f"未知工具 {name}"}
                    session.log_round(step, name, code, result, None)
                    messages.append({"role": "tool", "tool_call_id": tc.id,
                                     "content": json.dumps(result, ensure_ascii=False)[:6000]})
                    if name == "submit" and result.get("all_ac"):
                        final = {"status": "fixed", "rounds_used": submits}
                        return final, usage_total, submits
                if submits >= MAX_SUBMITS:
                    final = {"status": "exhausted", "rounds_used": submits}
                    break
            else:
                # 模型不再调用工具：追问一次；若仍不调用则结束
                if final["status"] == "no_submit" and submits == 0:
                    messages.append(msg)
                    messages.append({"role": "user",
                                     "content": "请通过 submit 工具提交你的代码。"})
                    continue
                final = {"status": "gave_up" if submits else "no_submit",
                         "rounds_used": submits}
                break

            if len(messages) > KEEP_LAST_MSGS + 4:
                messages = _truncate(messages)

        return final, usage_total, submits
