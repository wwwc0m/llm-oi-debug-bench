"""平台判题后端的统一接口。

各后端实现三件事：
  fetch_problem  —— 拉取题面与样例（缓存到题目目录，保证实验可复现）
  submit         —— 提交代码，返回记录 ID
  wait_result    —— 轮询直到出结果，返回归一化判定

归一化 verdict：AC / WA / TLE / MLE / RE / CE / 其他（原样透传）。
passed/total 可为 None（平台不透露测试点总数时）。
"""
from abc import ABC, abstractmethod


class PlatformBackend(ABC):
    name = "base"

    @abstractmethod
    def fetch_problem(self, ref: dict) -> dict:
        """返回 {"statement_html": str, "samples": [{"input": str, "output": str}]}"""

    @abstractmethod
    def submit(self, code: str, lang_name: str, ref: dict) -> str:
        """提交代码，返回平台记录 ID（字符串）。"""

    @abstractmethod
    def wait_result(self, rid: str, ref: dict, timeout_s: int = 600) -> dict:
        """返回 {"verdict": str, "passed": int|None, "total": int|None, "detail": str}"""


_CF_VERDICT_MAP = {
    "OK": "AC", "ACCEPTED": "AC",
    "WRONG_ANSWER": "WA",
    "TIME_LIMIT_EXCEEDED": "TLE",
    "MEMORY_LIMIT_EXCEEDED": "MLE",
    "RUNTIME_ERROR": "RE",
    "COMPILATION_ERROR": "CE",
    "IDLENESS_LIMIT_EXCEEDED": "WA",
    "PRESENTATION_ERROR": "WA",
    "CHALLENGED": "WA",
    "SKIPPED": "WA", "REJECTED": "WA",
}
