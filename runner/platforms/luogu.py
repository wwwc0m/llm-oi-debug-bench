"""洛谷后端。

拉题：支持匿名（解析 SPA 页面内嵌 JSON）与带 Cookie（_contentOnly 直返 JSON）。
提交/读结果：必须带登录 Cookie（LUOGU_UID / LUOGU_CLIENT_ID，浏览器导出）。

注意事项：
  * 提交走非官方接口，有风控；两次提交强制间隔 ≥60s。
  * 记录 status 码映射按社区文档 0f-0b/luogu-api-docs 编写，联调时需实测校验。
"""
import json
import os
import re
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
        uid = os.environ.get("LUOGU_UID", "")
        cid = os.environ.get("LUOGU_CLIENT_ID", "")
        self.logged_in = bool(uid and cid)
        if self.logged_in:
            self.sess.cookies.set("_uid", uid, domain=".luogu.com.cn")
            self.sess.cookies.set("__client_id", cid, domain=".luogu.com.cn")

    # ---------- 内部 ----------

    def _get_data_json(self, path: str) -> dict:
        """GET 页面；带 Cookie 时期望直返 JSON，匿名时解析内嵌 JSON。"""
        r = self.sess.get(BASE + path, params={"_contentOnly": "1"}, timeout=30)
        r.raise_for_status()
        try:
            return r.json()
        except ValueError:
            m = re.search(r"<script[^>]*>\s*(\{\"instance\".*?)</script>",
                          r.text, re.S)
            if not m:
                raise RuntimeError("洛谷返回无法解析的页面（可能触发风控，稍后再试）")
            return json.loads(m.group(1))

    # ---------- 接口实现 ----------

    def fetch_problem(self, ref: dict) -> dict:
        pid = ref["pid"]
        j = self._get_data_json(f"/problem/{pid}")
        prob = j.get("currentData", {}).get("problem") or \
            j.get("data", {}).get("problem")
        if not prob:
            raise RuntimeError(f"未取到题目 {pid} 数据：{list(j.keys())}")

        content = prob.get("content") or {}
        parts = [content.get(k) for k in
                 ("background", "description", "formatI", "formatO", "hint")
                 if content.get(k)]
        statement_md = f"# {prob.get('name', pid)}\n\n" + "\n\n".join(parts)

        samples = [{"input": s[0], "output": s[1]}
                   for s in prob.get("samples", [])]

        limits = prob.get("limits") or {}
        tl = max(limits["time"]) if limits.get("time") else None
        ml_kb = max(limits["memory"]) if limits.get("memory") else None
        n_points = len(limits.get("time") or [])

        return {"statement_md": statement_md,
                "samples": samples,
                "time_limit_ms": tl,
                "memory_limit_mb": round(ml_kb / 1024) if ml_kb else None,
                "test_point_count": n_points,
                "accept_languages": prob.get("acceptLanguages")}

    def submit(self, code: str, lang_name: str, ref: dict) -> str:
        if not self.logged_in:
            raise ValueError("提交需要登录：请在 .env 配置 LUOGU_UID / LUOGU_CLIENT_ID")
        # 限速：保护账号
        wait = MIN_SUBMIT_INTERVAL_S - (time.time() - _LAST_SUBMIT[0])
        if wait > 0:
            time.sleep(wait)
        _LAST_SUBMIT[0] = time.time()

        pid = ref["pid"]
        lang = ref.get("lang", "28")
        # CSRF token 每次页面加载由 <meta name="csrf-token"> 下发
        g = self.sess.get(f"{BASE}/problem/{pid}", timeout=30)
        m = re.search(r'<meta\s+name="csrf-token"\s+content="([^"]+)"', g.text)
        if not m:
            raise RuntimeError("未从页面取到 csrf-token（登录态可能失效）")
        csrf = m.group(1)
        r = self.sess.post(
            f"{BASE}/fe/api/problem/submit/{pid}",
            data={"code": code, "lang": lang, "enableO2": "1"},
            headers={"x-csrf-token": csrf, "x-luogu-type": "content-only",
                     "referer": f"{BASE}/problem/{pid}", "origin": BASE},
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
            data = self._get_data_json(f"/record/{rid}")
            rec = (data.get("currentData") or {}).get("record") or {}
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
