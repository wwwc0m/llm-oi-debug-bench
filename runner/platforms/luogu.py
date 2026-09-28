"""洛谷后端（非官方接口，实验性）。

凭据（.env）：LUOGU_UID / LUOGU_CLIENT_ID
—— 从浏览器登录洛谷后的 Cookie 中导出（F12 → Application → Cookies）。

注意事项：
  * 非官方接口，有 _fd 反爬与登录校验；两次提交强制间隔 ≥60s 降低风控。
  * status 码映射按社区文档 0f-0b/luogu-api-docs 编写，首次联调需实测校验。
  * 优点：个人私有题目可上传，是自编 reskin/novel 题进入真实判题的唯一公开渠道。
"""
import os
import time

import requests

from .base import PlatformBackend

BASE = "https://www.luogu.com.cn"
UA = ("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 "
      "(KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36")

# 洛谷记录 status：0/1 进行中，2 CE，10~14 各类判定（联调时核对）
_STATUS_MAP = {2: "CE", 10: "RE", 11: "MLE", 12: "AC", 13: "TLE", 14: "WA"}

_LAST_SUBMIT = [0.0]
MIN_SUBMIT_INTERVAL_S = 60


class LuoguBackend(PlatformBackend):
    name = "luogu"

    def __init__(self):
        self.sess = requests.Session()
        self.sess.headers["user-agent"] = UA
        uid = os.environ.get("LUOGU_UID")
        cid = os.environ.get("LUOGU_CLIENT_ID")
        if not uid or not cid:
            raise ValueError("请在 .env 配置 LUOGU_UID / LUOGU_CLIENT_ID（浏览器 Cookie 导出）")
        self.sess.cookies.set("_uid", uid, domain=".luogu.com.cn")
        self.sess.cookies.set("__client_id", cid, domain=".luogu.com.cn")

    # ---------- 内部 ----------

    def _content_only(self, path: str) -> dict:
        r = self.sess.get(BASE + path, params={"_contentOnly": "1"}, timeout=30)
        r.raise_for_status()
        return r.json()

    # ---------- 接口实现 ----------

    def fetch_problem(self, ref: dict) -> dict:
        pid = ref["pid"]
        data = self._content_only(f"/problem/{pid}")
        detail = data.get("currentData", {}).get("problem", {})
        statement_html = detail.get("background", "") + detail.get("content", "")
        samples = []
        for s in detail.get("samples", []):
            samples.append({"input": s[0], "output": s[1]})
        limits = detail.get("limits") or {}
        return {"statement_html": statement_html or f"<p>洛谷题目 {pid}</p>",
                "samples": samples,
                "time_limit_ms": limits.get("time"),
                "memory_limit_mb": limits.get("memory")}

    def submit(self, code: str, lang_name: str, ref: dict) -> str:
        # 限速：保护账号
        wait = MIN_SUBMIT_INTERVAL_S - (time.time() - _LAST_SUBMIT[0])
        if wait > 0:
            time.sleep(wait)
        _LAST_SUBMIT[0] = time.time()

        pid = ref["pid"]
        lang = ref.get("lang", "28")  # 28 = C++14（联调时核对语言代号）
        self.sess.get(f"{BASE}/problem/{pid}", timeout=30)  # 刷新反爬 Cookie
        r = self.sess.post(
            f"{BASE}/fe/api/problem/submit/{pid}",
            data={"code": code, "lang": lang, "enableO2": "1"},
            headers={"x-csrf-token": os.environ.get("LUOGU_CLIENT_ID", "")},
            timeout=30,
        )
        r.raise_for_status()
        data = r.json()
        rid = data.get("rid") or (data.get("data") or {}).get("rid")
        if not rid:
            raise RuntimeError(f"洛谷提交失败：{data}")
        return str(rid)

    def wait_result(self, rid: str, ref: dict, timeout_s: int = 600) -> dict:
        deadline = time.time() + timeout_s
        while time.time() < deadline:
            data = self._content_only(f"/record/{rid}")
            rec = data.get("currentRecord", {})
            status = rec.get("status")
            if status in (0, 1):  # 排队/评测中
                time.sleep(5)
                continue
            verdict = _STATUS_MAP.get(status, f"STATUS_{status}")
            detail = rec.get("detail") or {}
            passed = total = None
            cases = []
            for sub in detail.get("subtasks", []):
                for c in sub.get("test_cases", []):
                    cases.append(c.get("status"))
            if cases:
                total = len(cases)
                passed = sum(1 for s in cases if s == 12)
            return {"verdict": verdict, "passed": passed, "total": total,
                    "detail": f"{verdict}（通过测试点 {passed}/{total}）"
                              if total else verdict}
        return {"verdict": "UNKNOWN", "passed": None, "total": None,
                "detail": f"等待 {timeout_s}s 未出结果（rid={rid}）"}
