"""LLM 选手：function-calling 循环，模拟 OI 黑盒调试。"""
import json
import os

from .session import Session

MAX_SUBMITS = 5           # 最多提交轮数 K（实验协议）
KEEP_LAST_MSGS = 24       # 历史过长时截断，只保留最近若干条消息

SYSTEM_PROMPT = """你是一名参加 OI（信息学奥赛）式比赛的选手。规则：
1. 题目用 C++17 解决，判题机编译命令为 g++ -O2 -std=c++17。
2. 你可以用工具：compile（编译并看报错）、run_sample（在题面样例上自测）、submit（正式提交）。
3. submit 的反馈是黑盒的：只知道每个测试点通过与否，未通过的标注 WA/TLE/MLE/RE，看不到测试点内容。
4. 正式提交最多 {K} 次，用完即止。
5. 每次修改代码后，把完整的当前代码传给工具（工具会自动复用编译缓存）。
6. submit 前系统会在本地编译并运行题面样例：编译失败退回不计次；
   样例未过的提交退回且【消耗一次提交次数】。请先用 run_sample 自测通过再提交。""".strip()

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
    if "results" not in res:
        return res.get("detail") or str(res)
    lines = [f"提交结果：{res['passed']}/{res['total']} 个测试点通过。"]
    for r in res["results"]:
        lines.append(f"  测试点 {r['case']}: {'通过' if r['verdict'] == 'AC' else '未通过（' + r['verdict'] + '）'}")
    return "\n".join(lines)


def _msg_role(m) -> str | None:
    """消息可能是 dict，也可能是 SDK 返回的 ChatCompletionMessage 对象。"""
    if isinstance(m, dict):
        return m.get("role")
    return getattr(m, "role", None)


def _truncate(messages):
    """保留 system+题面+提交历史 前缀与最近消息；截断点必须落在 tool 回复序列之外，
    否则 tool 消息与对应的 tool_calls 断开会导致 API 报错。"""
    tail = messages[-KEEP_LAST_MSGS:]
    cut = 0
    while cut < len(tail) and _msg_role(tail[cut]) == "tool":
        cut += 1
    return messages[:3] + tail[cut:]


def _stream_create(client, model: str, messages: list, tools: list,
                   extra: dict | None = None):
    """流式请求：推理模型思考期间 token 持续流动，连接不会空闲超时。

    extra：透传给 create 的额外参数（如 reasoning_effort、thinking 开关）。
    返回与同步接口同形的对象（choices[0].message / usage）；
    message 为 SimpleNamespace（tool_calls 元素也是），供本模块消费。
    测试用的 Mock 客户端可能返回非流式对象，直接透传。"""
    params = dict(model=model, messages=messages, tools=tools,
                  temperature=0.7, stream=True, timeout=180,
                  max_tokens=262144)  # 默认 256K（官方上限 384K；原默认 64K 会导致思考烧穿）
    params.update(extra or {})     # extra 可覆盖（边界扫描用）
    stream = client.chat.completions.create(**params)
    if not hasattr(stream, "__iter__"):  # Mock 透传
        return stream

    content_parts, calls, usage, finish = [], {}, None, None
    n_chunks = 0
    for chunk in stream:
        n_chunks += 1
        if n_chunks % 50 == 0:
            print(f"[agent] …思考流式接收中（已 {n_chunks} 块）", flush=True)
        if getattr(chunk, "usage", None):
            usage = chunk.usage
        if not getattr(chunk, "choices", None):
            continue
        c = chunk.choices[0]
        if c.finish_reason:
            finish = c.finish_reason
        d = c.delta
        if d is None:
            continue
        if d.content:
            content_parts.append(d.content)
        for tc in (d.tool_calls or []):
            slot = calls.setdefault(tc.index or 0,
                                    {"id": "", "name": "", "args": ""})
            if tc.id:
                slot["id"] = tc.id
            fn = getattr(tc, "function", None)
            if fn is not None:
                if fn.name:
                    slot["name"] = fn.name
                if fn.arguments:
                    slot["args"] += fn.arguments

    from types import SimpleNamespace as NS
    tcs = [NS(id=v["id"], type="function",
              function=NS(name=v["name"], arguments=v["args"]))
           for _, v in sorted(calls.items())]
    msg = NS(tool_calls=tcs or None, content="".join(content_parts) or None)
    u = NS(prompt_tokens=getattr(usage, "prompt_tokens", 0) or 0,
           completion_tokens=getattr(usage, "completion_tokens", 0) or 0)
    return NS(choices=[NS(message=msg, finish_reason=finish)], usage=u)


def _msg_to_dict(msg) -> dict:
    """把流式组装的助手消息转为可直接回传 API 的 dict。"""
    if msg.tool_calls:
        return {"role": "assistant", "content": msg.content,
                "tool_calls": [{"id": t.id, "type": "function",
                                "function": {"name": t.function.name,
                                             "arguments": t.function.arguments}}
                               for t in msg.tool_calls]}
    return {"role": "assistant", "content": msg.content}


class AgentRunner:
    def __init__(self, judge, client, model: str,
                 phase: str = "A", starter_code: str | None = None,
                 out_dir: str | None = None, max_submits: int = MAX_SUBMITS):
        self.judge = judge
        self.client = client
        self.model = model
        self.phase = phase
        self.starter_code = starter_code
        self.out_dir = out_dir  # 每轮 round<N>.cpp / round<N>.result.json 落盘目录
        self.max_submits = max_submits  # 本运行剩余额度（题目总额度 5 跨运行累计）

    def _remember_round(self, n: int, code: str, detail: str, messages: list):
        """把每次提交的代码与判定写入固定位置的【提交历史】消息，长对话截断不丢失。"""
        self._rounds_log.append(f"### 第 {n} 次提交 → {detail}\n```cpp\n{code}\n```")
        messages[2]["content"] = (
            "【提交历史】此前每次提交的完整代码与判定（供对比分析，早期上下文可能已被"
            "截断，以此为准）：\n\n" + "\n\n".join(self._rounds_log))

    def _save_round(self, n: int, code: str, result: dict):
        if not self.out_dir:
            return
        os.makedirs(self.out_dir, exist_ok=True)
        with open(os.path.join(self.out_dir, f"round{n}.cpp"), "w",
                  encoding="utf-8", newline="\n") as f:
            f.write(code)
        slim = {k: v for k, v in result.items() if k != "results"}
        with open(os.path.join(self.out_dir, f"round{n}.result.json"), "w",
                  encoding="utf-8", newline="\n") as f:
            json.dump(slim, f, ensure_ascii=False, indent=2)

    def run(self, session: Session):
        p = self.judge.p
        limit_line = (f"【时间限制】{p.time_limit_ms}ms；【内存限制】{p.memory_limit_mb}MB"
                      if p.time_limit_ms else "【限制】以评测平台判题为准（本地自测仅作参考）")
        user_prompt = (
            f"【题目】{p.title}\n\n{p.statement}\n\n"
            f"{limit_line}\n\n"
            f"【示例输入/输出】\n" + "\n".join(
                f"样例{i+1}：\n输入：\n{s['input']}\n输出：\n{s['output']}"
                for i, s in enumerate(p.samples))
        )
        if self.phase == "B" and self.starter_code:
            user_prompt += "\n\n【场上已有代码（判定不完全正确，请修复）】\n```cpp\n" + self.starter_code + "\n```"

        sys_content = SYSTEM_PROMPT.format(K=self.max_submits)
        if self.phase == "B":
            sys_content += "\n\n" + PHASE_B_EXTRA

        messages = [
            {"role": "system", "content": sys_content},
            {"role": "user", "content": user_prompt},
            # 提交历史：位置固定、每轮更新，长对话截断时永不丢失（含全部历史代码）
            {"role": "user", "content": "【提交历史】（尚无提交）"},
        ]
        self._rounds_log = []

        submits, usage_total = 0, {"prompt_tokens": 0, "completion_tokens": 0}
        final = {"status": "no_submit", "rounds_used": 0}

        # 可选注入请求参数（如 {"reasoning_effort":"low"}）——对照组实验用
        import os as _os
        req_extra = None
        if _os.environ.get("BENCH_REQUEST_EXTRA"):
            try:
                req_extra = json.loads(_os.environ["BENCH_REQUEST_EXTRA"])
            except Exception:
                req_extra = None

        for step in range(60):  # 硬上限：防止死循环
            print(f"[agent] 第 {step + 1} 步：请求模型 {self.model} ...", flush=True)
            try:
                resp = _stream_create(self.client, self.model,
                                      messages, _tools_schema(), req_extra)
            except Exception as e:
                print(f"[agent] API 调用失败：{str(e)[:150]}", flush=True)
                final = {"status": "error", "detail": str(e)[:200],
                         "rounds_used": submits}
                break
            msg = resp.choices[0].message
            u = resp.usage
            usage_total["prompt_tokens"] += getattr(u, "prompt_tokens", 0) or 0
            usage_total["completion_tokens"] += getattr(u, "completion_tokens", 0) or 0

            if msg.tool_calls:
                messages.append(_msg_to_dict(msg))
                for tc in msg.tool_calls:
                    code = json.loads(tc.function.arguments).get("code", "")
                    name = tc.function.name
                    if name == "compile":
                        result = self.judge.compile(code)
                        session.log_round(step, "compile", None,
                                          {"ok": result.get("ok"),
                                           "errors": (result.get("errors") or "")[:800]}, None)
                    elif name == "run_sample":
                        result = self.judge.run_sample(code)
                        # 本地样例测试数据完整记录（协议 v2）
                        slim = {"ok": result.get("ok"),
                                "runs": [{k: v for k, v in r.items()
                                          if k in ("sample", "verdict", "expected", "your_output")}
                                         for r in result.get("runs", [])]}
                        session.log_round(step, "run_sample", code, slim, None)
                    elif name == "submit":
                        result = self.judge.submit(code)
                        if result.get("ok"):
                            # 真正到达评测平台的提交：计数/落盘
                            submits += 1
                            view = {k: result[k] for k in ("ok", "all_ac", "passed", "total")
                                    if k in result}
                            view["detail"] = result.get("detail") or _fmt_submit(result)
                            result = view
                            self._save_round(submits, code, result)
                            session.log_round(step, name, code, result, None)
                            self._remember_round(submits, code, view.get("detail", ""), messages)
                        elif result.get("local_gate") == "samples":
                            # 样例未过的提交：计入提交次数（协议 v2），并落盘记录样例数据
                            submits += 1
                            result["detail"] = (f"提交被退回（样例未过，已计入提交次数，"
                                                f"剩余 {self.max_submits - submits} 次）")
                            self._save_round(submits, code, result)
                            session.log_round(step, "local_gate:samples", code,
                                              {"errors": (result.get("errors") or "")[:2000]}, None)
                            self._remember_round(submits, code,
                                                 "样例未过被退回：" +
                                                 (result.get("errors") or "")[:300], messages)
                        elif result.get("local_gate") == "compile":
                            # 编译失败退回：不计次（协议 v2 沿用）
                            session.log_round(step, "local_gate:compile", code,
                                              {"errors": (result.get("errors") or "")[:2000]}, None)
                    else:
                        result = {"ok": False, "errors": f"未知工具 {name}"}
                    brief = result.get("detail") or (
                        "退回：" + result.get("local_gate", "?") if result.get("local_gate")
                        else ("失败：" + str(result.get("errors", ""))[:120] if not result.get("ok") else "ok"))
                    print(f"[agent] 工具 {name} -> {brief}", flush=True)
                    messages.append({"role": "tool", "tool_call_id": tc.id,
                                     "content": json.dumps(result, ensure_ascii=False)[:6000]})
                    if name == "submit" and result.get("all_ac"):
                        final = {"status": "fixed", "rounds_used": submits}
                        return final, usage_total, submits
                if submits >= self.max_submits:
                    final = {"status": "exhausted", "rounds_used": submits}
                    break
            else:
                # 空响应（思考烧穿时既无内容也无工具调用）：
                # 空 assistant 消息不可回传（API 400），改为催促重答
                if not (msg.content or "").strip():
                    print("[agent] 本步响应为空（疑似思考超限），催促重答 ...", flush=True)
                    self._empty_nudges = getattr(self, "_empty_nudges", 0) + 1
                    if self._empty_nudges >= 3:
                        final = {"status": "error",
                                 "detail": "连续空响应（思考超限）",
                                 "rounds_used": submits}
                        break
                    messages.append({"role": "user",
                                     "content": "你上一次响应为空（思考可能超出输出上限）。"
                                                "请精简思考，直接调用 submit 工具提交完整代码。"})
                    continue
                # 模型不再调用工具：追问一次；若仍不调用则结束
                if final["status"] == "no_submit" and submits == 0:
                    messages.append(_msg_to_dict(msg))
                    messages.append({"role": "user",
                                     "content": "请通过 submit 工具提交你的代码。"})
                    continue
                final = {"status": "gave_up" if submits else "no_submit",
                         "rounds_used": submits}
                break

            if len(messages) > KEEP_LAST_MSGS + 4:
                messages = _truncate(messages)

        return final, usage_total, submits
