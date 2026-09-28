"""命令行入口。

用法：
  python -m runner.cli build  --problem problems/classic-inv-pairs   # 生成隐藏测试并跑标程出答案
  python -m runner.cli judge  --problem ... --code path/to/code.cpp  # 判一份代码（调题目/验注错变体用）
  python -m runner.cli run    --problem ... --model deepseek:deepseek-chat [--phase A|B] [--mutant overflow]
  python -m runner.cli fetch  --platform luogu --pid P1001 --id lg1001   # 从平台拉题
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


def cmd_fetch(args):
    """从平台拉取题面/样例/限制，生成 problems/<id>/problem.json。"""
    from .platforms.codeforces import CodeforcesBackend
    from .platforms.luogu import LuoguBackend
    if args.platform == "luogu":
        backend, ref = LuoguBackend(), {"platform": "luogu", "pid": args.pid,
                                        "lang": args.lang or "28"}
    elif args.platform == "codeforces":
        backend, ref = CodeforcesBackend(), {"platform": "codeforces",
                                             "url": args.url}
    else:
        sys.exit(f"未知平台：{args.platform}")
    fetched = backend.fetch_problem(ref)
    out_dir = os.path.join("problems", args.id)
    os.makedirs(out_dir, exist_ok=True)
    meta = {
        "id": args.id,
        "category": args.category,
        "title": args.title or f"{args.platform}:{args.pid or args.url}",
        "statement": "（平台题，题面见 statement.txt）",
        "time_limit_ms": fetched.get("time_limit_ms"),
        "memory_limit_mb": fetched.get("memory_limit_mb"),
        "samples": fetched["samples"],
        "source": ref,
    }
    with open(os.path.join(out_dir, "problem.json"), "w", encoding="utf-8") as f:
        json.dump(meta, f, ensure_ascii=False, indent=2)
    from bs4 import BeautifulSoup
    text = BeautifulSoup(fetched["statement_html"], "html.parser").get_text("\n", strip=True)
    with open(os.path.join(out_dir, "statement.txt"), "w", encoding="utf-8") as f:
        f.write(text)
    print(f"已创建 problems/{args.id}/（题面 {len(text)} 字，样例 {len(fetched['samples'])} 组）")


def cmd_run(args):
    from . import providers
    from .platform_judge import open_judge
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

    j = open_judge(p, backend_hint=args.backend)
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
    r.add_argument("--starter", default=None,
                   help="阶段 B 起始代码路径（默认用 mutants/<mutant>.cpp）")
    r.add_argument("--repeat", type=int, default=0)
    r.add_argument("--backend", default=None,
                   help="强制指定判题后端（默认按题目 source 自动选择）")
    r.set_defaults(func=cmd_run)

    f = sub.add_parser("fetch", help="从平台拉题生成题目目录")
    f.add_argument("--platform", choices=["luogu", "codeforces"], required=True)
    f.add_argument("--pid", default=None, help="洛谷题号，如 P1001")
    f.add_argument("--url", default=None, help="CF 题目 URL")
    f.add_argument("--id", required=True, help="本地题目目录名")
    f.add_argument("--lang", default=None, help="提交语言（洛谷语言代号）")
    f.add_argument("--category", default="classic",
                   choices=["classic", "reskin", "novel"])
    f.add_argument("--title", default=None)
    f.set_defaults(func=cmd_fetch)

    args = ap.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
