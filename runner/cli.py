"""命令行入口（判题走洛谷：题面 requests 拉取，提交/结果走浏览器）。

用法：
  python -m runner.cli fetch --pid P1908                    # 拉题生成 experiments/P1908/
  python -m runner.cli run --problem P1908 --model deepseek:deepseek-chat
  python -m runner.cli batch --problem P1908 --repeat 3     # 全部可用模型 × 重复
  python -m runner.cli models                               # 列出可用模型
"""
import argparse
import json
import os
import re
import sys

from .agent import AgentRunner
from .platform_judge import LuoguJudge
from .problem import Problem
from .session import Session

EXP_ROOT = "experiments"


def _load_problem(path) -> Problem:
    if not os.path.isdir(path):
        path = os.path.join(EXP_ROOT, path)
    return Problem(path)


def cmd_fetch(args):
    """从洛谷拉取题面/样例/限制，生成 experiments/<pid>/ 目录。"""
    from .platforms.luogu import LuoguBackend
    backend = LuoguBackend()
    ref = {"platform": "luogu", "pid": args.pid, "lang": args.lang or "28"}
    fetched = backend.fetch_problem(ref)
    out_dir = os.path.join(EXP_ROOT, args.pid)
    os.makedirs(os.path.join(out_dir, "assets"), exist_ok=True)
    os.makedirs(os.path.join(out_dir, "runs"), exist_ok=True)
    meta = {
        "id": args.pid,
        "category": args.category,
        "title": args.title or f"{fetched.get('name') or ''}（洛谷 {args.pid}）",
        "difficulty": fetched.get("difficulty"),
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
    print(f"已创建 {out_dir}/（题面 {len(md)} 字，样例 {len(fetched['samples'])} 组，"
          f"测试点 {fetched.get('test_point_count')} 个）")


def cmd_models(args):
    from . import providers
    models = providers.list_models()
    if not models:
        sys.exit("没有可用模型：请在 .env 里填至少一个 API key")
    for m in models:
        print(m)


def _run_one(problem_id: str, model_spec: str, phase: str, mutant: str,
             starter: str | None, repeat: int) -> dict:
    """跑一次完整实验（一题 × 一模型 × 一重复），产物写入 runs 目录。"""
    from . import providers
    p = _load_problem(problem_id)
    client, model = providers.make_client(model_spec)

    starter_code = None
    if phase == "B":
        cand = [starter] if starter else [
            os.path.join(p.dir, "assets", "mutants", f"{mutant}.cpp"),
            os.path.join(p.dir, "mutants", f"{mutant}.cpp"),  # 兼容旧布局
        ]
        for path in cand:
            if path and os.path.exists(path):
                with open(path, encoding="utf-8") as f:
                    starter_code = f.read()
                break
        if starter_code is None:
            sys.exit("阶段 B 需要起始代码：--starter 文件 或 --mutant 名称")

    # runs/<模型短名>/run<k>/：取已有 run 目录的最大编号 +1
    model_dir_name = re.sub(r"[^A-Za-z0-9._-]+", "_", model_spec)
    runs_dir = os.path.join(p.dir, "runs", model_dir_name)
    ks = []
    if os.path.isdir(runs_dir):
        for d in os.listdir(runs_dir):
            m = re.fullmatch(r"run(\d+)", d)
            if m and os.path.isdir(os.path.join(runs_dir, d)):
                ks.append(int(m.group(1)))
    out_dir = os.path.join(runs_dir, f"run{max(ks, default=0) + 1}")
    os.makedirs(out_dir, exist_ok=True)

    j = LuoguJudge(p)
    sess = Session(out_dir, p.id, model_spec, phase,
                   extra={"mutant": mutant, "repeat": repeat})
    runner = AgentRunner(j, client, model, phase=phase,
                         starter_code=starter_code, out_dir=out_dir)
    final, usage, submits = runner.run(sess)
    j.cleanup()
    summary = {"problem": p.id, "model": model_spec, "phase": phase,
               "run": max(ks, default=0) + 1, "final": final,
               "usage": usage, "submits": submits}
    with open(os.path.join(out_dir, "summary.json"), "w", encoding="utf-8") as f:
        json.dump(summary, f, ensure_ascii=False, indent=2)
    print(json.dumps(summary, ensure_ascii=False, indent=2))
    return summary


def cmd_run(args):
    _run_one(args.problem, args.model, args.phase, args.mutant,
             args.starter, args.repeat)


def cmd_batch(args):
    """一题 × 全部可用模型 × 重复次数，顺序执行。"""
    from . import providers
    models = providers.list_models()
    if not models:
        sys.exit("没有可用模型：请在 .env 里填 API key（至少一家）")
    print(f"批量实验：{args.problem} × {models} × {args.repeat} 次")
    results = []
    for m in models:
        for r in range(1, args.repeat + 1):
            print(f"\n===== {m} 第 {r}/{args.repeat} 轮 =====")
            try:
                results.append(_run_one(args.problem, m, args.phase,
                                        args.mutant, args.starter, r))
            except Exception as e:
                print(f"[失败] {m} run{r}: {e}")
                results.append({"model": m, "run": r, "error": str(e)})
    ok = [r for r in results if r.get("final", {}).get("status") == "fixed"]
    from .agent import MAX_SUBMITS
    print(f"\n批量完成：{len(ok)}/{len(results)} 轮在 {MAX_SUBMITS} 次内修复")
    out = os.path.join(EXP_ROOT, args.problem, "runs", "batch_summary.json")
    with open(out, "w", encoding="utf-8") as f:
        json.dump(results, f, ensure_ascii=False, indent=2)
    print(f"汇总已写 {out}")


def main():
    ap = argparse.ArgumentParser(prog="runner")
    sub = ap.add_subparsers(dest="cmd", required=True)

    f = sub.add_parser("fetch", help="从洛谷拉题生成 experiments/<pid>/")
    f.add_argument("--pid", required=True, help="洛谷题号，如 P1908")
    f.add_argument("--lang", default=None, help="提交语言代号（默认 28=C++14）")
    f.add_argument("--category", default="classic",
                   choices=["classic", "reskin", "novel"])
    f.add_argument("--title", default=None)
    f.set_defaults(func=cmd_fetch)

    m = sub.add_parser("models", help="列出可用模型")
    m.set_defaults(func=cmd_models)

    r = sub.add_parser("run", help="跑一次实验（判题走洛谷浏览器）")
    r.add_argument("--problem", required=True, help="题目编号（experiments/ 下目录名）")
    r.add_argument("--model", required=True, help="provider:model，如 deepseek:deepseek-chat")
    r.add_argument("--phase", choices=["A", "B"], default="A")
    r.add_argument("--mutant", default="overflow")
    r.add_argument("--starter", default=None,
                   help="阶段 B 起始代码路径（默认用 assets/mutants/<mutant>.cpp）")
    r.add_argument("--repeat", type=int, default=0)
    r.set_defaults(func=cmd_run)

    b = sub.add_parser("batch", help="一题 × 全部可用模型 × 重复次数")
    b.add_argument("--problem", required=True)
    b.add_argument("--repeat", type=int, default=1)
    b.add_argument("--phase", choices=["A", "B"], default="A")
    b.add_argument("--mutant", default="overflow")
    b.add_argument("--starter", default=None)
    b.set_defaults(func=cmd_batch)

    args = ap.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
