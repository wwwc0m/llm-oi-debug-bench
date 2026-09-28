"""Codeforces 后端：onlinejudge 库负责登录/提交，官方 API 轮询结果。

凭据（.env）：CODEFORCES_HANDLE / CODEFORCES_PASSWORD
建议为本实验注册专用小号；CF 登录偶尔出验证码，出号需人工介入。
"""
import os
import time

import requests
from bs4 import BeautifulSoup

from onlinejudge.service.codeforces import CodeforcesProblem, CodeforcesService

from .base import PlatformBackend, _CF_VERDICT_MAP


class CodeforcesBackend(PlatformBackend):
    name = "codeforces"

    def __init__(self):
        self.session = requests.Session()
        self._service = CodeforcesService()
        self._logged = False

    # ---------- 内部 ----------

    def _login(self):
        if self._logged:
            return
        handle = os.environ.get("CODEFORCES_HANDLE")
        password = os.environ.get("CODEFORCES_PASSWORD")
        if not handle or not password:
            raise ValueError("请在 .env 配置 CODEFORCES_HANDLE / CODEFORCES_PASSWORD")
        self._service.login(get_credentials=lambda: (handle, password),
                            session=self.session)
        self._logged = True

    def _problem(self, ref: dict) -> CodeforcesProblem:
        return CodeforcesProblem.from_url(ref["url"])

    def _resolve_language(self, problem, lang_name: str):
        langs = problem.get_available_languages(session=self.session)
        if lang_name:
            for l in langs:
                if lang_name.lower() in l.name.lower():
                    return l
        # 兜底：取最新可用的 64 位 g++
        for l in langs:
            if "GNU G++" in l.name and "64" in l.name:
                return l
        for l in langs:
            if "GNU G++" in l.name:
                return l
        raise ValueError(f"找不到可用语言，候选：{[l.name for l in langs]}")

    # ---------- 接口实现 ----------

    def fetch_problem(self, ref: dict) -> dict:
        p = self._problem(ref)
        # 题面：匿名抓取题目页，抽出题面主体
        r = self.session.get(ref["url"], timeout=30)
        r.raise_for_status()
        soup = BeautifulSoup(r.text, "html.parser")
        node = soup.select_one(".problem-statement") or soup
        for kill in node.select("script, style, .header input"):
            kill.decompose()
        statement_html = str(node)

        # 样例：用 onlinejudge 库的解析（.input/.output 块）
        samples = []
        for case in p.download_sample_cases(session=self.session):
            samples.append({
                "input": case.input.data.decode("utf-8", "replace"),
                "output": case.output.data.decode("utf-8", "replace"),
            })
        return {"statement_html": statement_html, "samples": samples}

    def submit(self, code: str, lang_name: str, ref: dict) -> str:
        self._login()
        p = self._problem(ref)
        lang = self._resolve_language(p, lang_name)
        sub = p.submit_code(code.encode("utf-8"), lang.id, session=self.session)
        contest = p.get_contest()
        contest_id = getattr(contest, "contest_id", None) or getattr(contest, "id", None)
        return f"{contest_id}:{sub.get_id()}"

    def wait_result(self, rid: str, ref: dict, timeout_s: int = 600) -> dict:
        contest_id, sub_id = rid.split(":", 1)
        sub_id = int(sub_id)
        handle = os.environ.get("CODEFORCES_HANDLE")
        api = "https://codeforces.com/api/contest.status"
        deadline = time.time() + timeout_s
        while time.time() < deadline:
            r = self.session.get(api, params={
                "contestId": int(contest_id), "from": 1, "count": 100,
                "handle": handle,
            }, timeout=30)
            r.raise_for_status()
            for entry in r.json().get("result", []):
                if entry["id"] == sub_id:
                    v = entry.get("verdict")
                    if v:  # None = 排队/评测中
                        passed = entry.get("passedTestCount")
                        return {
                            "verdict": _CF_VERDICT_MAP.get(v, v),
                            "passed": passed,
                            "total": None,
                            "detail": f"{_CF_VERDICT_MAP.get(v, v)}"
                                      f"（通过测试点 {passed} 个）",
                        }
            time.sleep(5)
        return {"verdict": "UNKNOWN", "passed": None, "total": None,
                "detail": f"等待 {timeout_s}s 未出结果（rid={rid}）"}
