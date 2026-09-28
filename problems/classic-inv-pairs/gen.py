"""隐藏测试生成器：classic-inv-pairs。

覆盖：边界（n=1、全相等）、随机、正序、逆序、极端规模。
只生成 .in；.ans 由 cli build 用参考解跑出。
"""
import random


def generate(tests_dir: str):
    rng = random.Random(20260928)

    def write(name, n, arr):
        with open(f"{tests_dir}/{name}.in", "w", encoding="utf-8", newline="\n") as f:
            f.write(f"{n}\n" + " ".join(map(str, arr)) + "\n")

    write("01-min", 1, [7])                                   # 最小规模
    write("02-all-eq-small", 40, [5] * 40)                    # 全相等（小）
    write("03-rand-small", 50, [rng.randint(1, 20) for _ in range(50)])
    write("04-rand-mid", 200, [rng.randint(1, 10**9) for _ in range(200)])
    write("05-sorted-max", 100000, list(range(1, 100001)))     # 正序，0 逆序对
    arr = list(range(100000, 0, -1))
    write("06-rev-max", 100000, arr)                           # 逆序，满逆序对
    write("07-rand-max", 100000, [rng.randint(1, 10**9) for _ in range(100000)])
    write("08-all-eq-max", 100000, [42] * 100000)              # 全相等（大）
    write("09-rand-dup-max", 99999, [rng.randint(1, 100) for _ in range(99999)])
    write("10-rand-max2", 100000, [rng.randint(1, 10**9) for _ in range(100000)])
