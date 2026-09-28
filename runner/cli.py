"""命令行入口。

用法：
  python -m runner.cli build  --problem problems/classic-inv-pairs   # 生成隐藏测试并跑标程出答案
  python -m runner.cli judge  --problem ... --code path/to/code.cpp  # 判一份代码（调题目/验注错变体用）
  python -m runner.cli run    --problem ... --model deepseek:deepseek-chat [--phase A|B] [--mutant overflow]
  python -m runner.cli models                                       # 列出可用模型
"""
import argparse
import glob
import importlib.util
import json
import os
import sys

from .agent import AgentRunner, MAX_SUBMITS
from .judge import Judge, Problem
from .sandbox import compile_cpp, run_case
from .session import Session


def _load_problem(path, require_hidden: bool = True) -> Problem:
    if not os.path.isdir(path):
        path = os.path.join("problems", path)
    return Problem(path, require_hidden=require_hidden)


def cmd_build(args):
    p = _load_problem(args.problem, require_hidden=False)
    tests_dir = os.path.join(p.dir, "tests")
    os.makedirs(tests_dir, exist_ok=True)

    # 1) 数据生成器生成 .in
    gen_path = os.path.join(p.dir, "gen.py")
    spec = importlib.util.spec_from_file_location("gen", gen_path)
    gen = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(gen)
    gen.generate(tests_dir)

    # 2) 标程跑出 .ans
    import tempfile
    workdir = tempfile.mkdtemp(prefix="build_")
    exe, err = compile_cpp(p.reference_solution, workdir)
    if exe is None:
        sys.exit(f"参考解编译失败：\n{err}")
    for in_path in sorted(glob.glob(os.path.join(tests_dir, "*.in"))):
        with open(in_path, encoding="utf-8") as f:
            inp = f.read()
        r = run_case(exe, inp, p.time_limit_ms * 3, p.memory_limit_mb)
        if r["verdict"] != "AC":
            sys.exit(f"参考解在 {in_path} 上异常：{r}")
        with open(os.path.splitext(in_path)[0] + ".ans", "w", encoding="utf-8", newline="\n") as f:
            f.write(r["stdout"])
        print(f"  {os.path.basename(in_path)} -> ans（{r['elapsed_ms']:.0f}ms）")
    print(f"题目 {p.id}：{len(glob.glob(os.path.join(tests_dir, '*.in')))} 组隐藏测试就绪")


def cmd_judge(args):
    p = _load_problem(args.problem)
    with open(args.code, encoding="utf-8") as f:
        code = f.read()
    j = Judge(p)
    res = j.submit(code)
    print(f"\n{'=' * 50}\n{args.code} vs 隐藏测试：{res['passed']}/{res['total']}")
    for r in res["results"]:
        flag = "✓" if r["verdict"] == "AC" else "✗"
        print(f"  {flag} {r['case']:<4} {r['verdict']:<4} {r['elapsed_ms']:>7.0f}ms  {r['peak_mem_mb']:>6.1f}MB")
    j.cleanup()


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
        mutant_path = os.path.join(p.dir, "mutants", f"{args.mutant}.cpp")
        with open(mutant_path, encoding="utf-8") as f:
            starter = f.read()

    j = Judge(p)
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

    b = sub.add_parser("build", help="生成隐藏测试并用标程出答案")
    b.add_argument("--problem", required=True)
    b.set_defaults(func=cmd_build)

    j = sub.add_parser("judge", help="判一份代码")
    j.add_argument("--problem", required=True)
    j.add_argument("--code", required=True)
    j.set_defaults(func=cmd_judge)

    m = sub.add_parser("models", help="列出可用模型")
    m.set_defaults(func=cmd_models)

    r = sub.add_parser("run", help="跑一轮 agent 实验")
    r.add_argument("--problem", required=True)
    r.add_argument("--model", required=True)
    r.add_argument("--phase", choices=["A", "B"], default="A")
    r.add_argument("--mutant", default="overflow")
    r.add_argument("--repeat", type=int, default=0)
    r.set_defaults(func=cmd_run)

    args = ap.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
