"""命令行入口（判题只走洛谷）。

用法：
  python -m runner.cli fetch --pid P1908 --id inv-pairs          # 从洛谷拉题生成题目目录
  python -m runner.cli run --problem inv-pairs --model deepseek:deepseek-chat [--phase A|B] [--mutant overflow]
  python -m runner.cli models                                     # 列出可用模型
"""
import argparse
import json
import os
import sys

from .agent import AgentRunner
from .platform_judge import LuoguJudge
from .problem import Problem
from .session import Session


def _load_problem(path) -> Problem:
    if not os.path.isdir(path):
        path = os.path.join("problems", path)
    return Problem(path)


def cmd_fetch(args):
    """从洛谷拉取题面/样例/限制，生成 problems/<id>/problem.json。"""
    from .platforms.luogu import LuoguBackend
    backend = LuoguBackend()
    ref = {"platform": "luogu", "pid": args.pid, "lang": args.lang or "28"}
    fetched = backend.fetch_problem(ref)
    out_dir = os.path.join("problems", args.id)
    os.makedirs(out_dir, exist_ok=True)
    meta = {
        "id": args.id,
        "category": args.category,
        "title": args.title or f"洛谷 {args.pid}",
        "statement": "",
        "time_limit_ms": fetched.get("time_limit_ms"),
        "memory_limit_mb": fetched.get("memory_limit_mb"),
        "samples": fetched["samples"],
        "source": ref,
    }
    with open(os.path.join(out_dir, "problem.json"), "w", encoding="utf-8") as f:
        json.dump(meta, f, ensure_ascii=False, indent=2)
    md = fetched["statement_md"]
    with open(os.path.join(out_dir, "statement.md"), "w", encoding="utf-8") as f:
        f.write(md)
    print(f"已创建 problems/{args.id}/（题面 {len(md)} 字，样例 {len(fetched['samples'])} 组，"
          f"测试点 {fetched.get('test_point_count')} 个）")


def cmd_models(args):
    from . import providers
    models = providers.list_models()
    if not models:
        sys.exit("没有可用模型：请在 .env 里填至少一个 API key")
    for m in models:
        print(m)


def cmd_run(args):
    from . import providers
    p = _load_problem(args.problem)
    client, model = providers.make_client(args.model)

    starter = None
    if args.phase == "B":
        cand = [args.starter] if args.starter else [
            os.path.join(p.dir, "mutants", f"{args.mutant}.cpp")]
        for path in cand:
            if path and os.path.exists(path):
                with open(path, encoding="utf-8") as f:
                    starter = f.read()
                break
        if starter is None:
            sys.exit("阶段 B 需要起始代码：--starter 文件 或 --mutant 名称")

    j = LuoguJudge(p)
    sess = Session("results/raw", p.id, args.model, args.phase,
                   extra={"mutant": args.mutant, "repeat": args.repeat})
    runner = AgentRunner(j, client, model, phase=args.phase, starter_code=starter)
    final, usage, submits = runner.run(sess)
    j.cleanup()
    print(json.dumps({"final": final, "usage": usage, "submits": submits,
                      "trajectory": sess.file}, ensure_ascii=False, indent=2))


def main():
    ap = argparse.ArgumentParser(prog="runner")
    sub = ap.add_subparsers(dest="cmd", required=True)

    f = sub.add_parser("fetch", help="从洛谷拉题生成题目目录")
    f.add_argument("--pid", required=True, help="洛谷题号，如 P1908")
    f.add_argument("--id", required=True, help="本地题目目录名")
    f.add_argument("--lang", default=None, help="提交语言代号（默认 28=C++14，联调核对）")
    f.add_argument("--category", default="classic",
                   choices=["classic", "reskin", "novel"])
    f.add_argument("--title", default=None)
    f.set_defaults(func=cmd_fetch)

    m = sub.add_parser("models", help="列出可用模型")
    m.set_defaults(func=cmd_models)

    r = sub.add_parser("run", help="跑一轮 agent 实验（判题走洛谷）")
    r.add_argument("--problem", required=True)
    r.add_argument("--model", required=True)
    r.add_argument("--phase", choices=["A", "B"], default="A")
    r.add_argument("--mutant", default="overflow")
    r.add_argument("--starter", default=None,
                   help="阶段 B 起始代码路径（默认用 mutants/<mutant>.cpp）")
    r.add_argument("--repeat", type=int, default=0)
    r.set_defaults(func=cmd_run)

    args = ap.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
