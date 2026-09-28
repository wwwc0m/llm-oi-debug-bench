"""纯生成模式：给模型题面+样例，只取代码，不接判题、不提交。

用法：python tools/generate_only.py <provider:model> <题号> [输出路径]
输出：模型生成的完整 C++ 代码（来自工具调用参数），本地 g++ 编译验证。
"""
import json
import os
import sys
import time

from dotenv import load_dotenv

load_dotenv()
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from runner.agent import SYSTEM_PROMPT, MAX_SUBMITS, _stream_create, _tools_schema
from runner.problem import Problem
from runner.providers import make_client
from runner import sandbox


def main():
    model_spec, pid = sys.argv[1], sys.argv[2]
    out_path = sys.argv[3] if len(sys.argv) > 3 else f"results/gen_{pid}_{model_spec.replace(':', '_')}.cpp"
    effort = sys.argv[4] if len(sys.argv) > 4 else ""  # '' | low | off
    extra = {}
    prov = model_spec.partition(":")[0]
    if effort == "low":
        if prov == "deepseek":
            extra = {"reasoning_effort": "low"}
        else:
            extra = {}  # 智谱无 low 档，等效用 off
    elif effort == "off":
        extra = {"extra_body": {"thinking": {"type": "disabled"}}}
    p = Problem(os.path.join("experiments", pid))
    limit_line = (f"【时间限制】{p.time_limit_ms}ms；【内存限制】{p.memory_limit_mb}MB"
                  if p.time_limit_ms else "【限制】以评测平台判题为准")
    user_prompt = (
        f"【题目】{p.title}\n\n{p.statement}\n\n{limit_line}\n\n"
        "【示例输入/输出】\n" + "\n".join(
            f"样例{i+1}：\n输入：\n{s['input']}\n输出：\n{s['output']}"
            for i, s in enumerate(p.samples)) +
        "\n\n【提交要求】代码必须是完整可编译的 C++17 程序：从标准输入读入数据、"
        "按算法计算并输出答案；禁止硬编码样例输出；禁止在代码之外输出任何分析文字。"
    )
    client, model = make_client(model_spec)
    messages = [
        {"role": "system", "content": SYSTEM_PROMPT.format(K=MAX_SUBMITS)},
        {"role": "user", "content": user_prompt},
    ]
    print(f"[gen] {model_spec} 生成中（effort={effort or '默认'}，流式）...", flush=True)
    t0 = time.time()
    resp = _stream_create(client, model, messages, _tools_schema(), extra)
    msg = resp.choices[0].message
    dt = time.time() - t0
    code = None
    for tc in (msg.tool_calls or []):
        try:
            args = json.loads(tc.function.arguments)
        except Exception:
            continue
        if args.get("code"):
            code = args["code"]
            break
    if code is None:
        code = (msg.content or "").strip() or ""
    with open(out_path, "w", encoding="utf-8", newline="\n") as f:
        f.write(code)
    print(f"[gen] 完成：耗时 {dt:.0f}s，代码 {len(code)} 字符 -> {out_path}", flush=True)
    print(f"[gen] usage: {resp.usage.prompt_tokens}/{resp.usage.completion_tokens}", flush=True)
    # 本地编译验证（不运行、不提交）
    import tempfile
    workdir = tempfile.mkdtemp(prefix="gen_")
    exe, err = sandbox.compile_cpp(code, workdir)
    print(f"[gen] 本地编译: {'通过 ✓' if exe else '失败 ✗'}")
    if not exe:
        print(err[:500])


if __name__ == "__main__":
    main()
