"""洛谷浏览器后端：真实 Edge 完成提交与逐测试点结果读取。

提交策略（两级降级）：
  1. 页面内 fetch POST 提交接口——浏览器上下文自带 Cookie/指纹/Referer，
     若服务端认可则全自动拿到 rid；
  2. 若返回验证码错误：打开提交界面并提示人工完成（点击提交/过 Turnstile），
     监听 URL 跳转到 /record/{rid} 自动继续。

结果读取：打开 /record/{rid}，解析"测试点信息"区块（DOM 文本正则，已实测）。
"""
import os
import re
import time

from playwright.sync_api import sync_playwright

BASE = "https://www.luogu.com.cn"

_PT_RE = re.compile(r"(\d+(?:\.\d+)?m?s/[0-9.]+[KMG]B)\s*(AC|WA|TLE|MLE|RE|UKE)\s*#(\d+)")
_RECORD_RE = re.compile(r"/record/(\d+)")


class LuoguBrowser:
    def __init__(self, headless: bool = False):
        self._pw = sync_playwright().start()
        self.browser = self._pw.chromium.launch(channel="msedge", headless=headless)
        self.ctx = self.browser.new_context(viewport={"width": 1280, "height": 900})
        uid, cid = os.environ.get("LUOGU_UID", ""), os.environ.get("LUOGU_CLIENT_ID", "")
        if uid and cid:
            self.ctx.add_cookies([
                {"name": "_uid", "value": uid, "domain": ".luogu.com.cn", "path": "/"},
                {"name": "__client_id", "value": cid, "domain": ".luogu.com.cn", "path": "/"},
            ])
        self.page = self.ctx.new_page()

    # ---------- 提交 ----------

    def submit(self, code: str, lang: str, ref: dict, human_wait_s: int = 120) -> str:
        pid = ref["pid"]
        page = self.page
        page.goto(f"{BASE}/problem/{pid}", wait_until="domcontentloaded", timeout=30000)
        page.wait_for_timeout(2500)

        # 策略1：页面内 fetch（同源，自动带全部头与 Cookie）
        probe = page.evaluate(
            "async ([pid, code, lang]) => {"
            "  const meta = document.querySelector('meta[name=\\\"csrf-token\\\"]');"
            "  const csrf = meta ? meta.content : '';"
            "  const body = new URLSearchParams({code, lang, enableO2: '1'});"
            "  const r = await fetch(`/fe/api/problem/submit/${pid}`, {"
            "    method: 'POST', headers: {'content-type': 'application/x-www-form-urlencoded',"
            "      'x-csrf-token': csrf, 'x-luogu-type': 'content-only'}, body});"
            "  const t = await r.text(); let j = null;"
            "  try { j = JSON.parse(t); } catch (e) {}"
            "  return {status: r.status, json: j, head: t.slice(0, 150)};"
            "}", [pid, code, lang or "28"])
        j = probe.get("json") or {}
        rid = j.get("rid") or (j.get("data") or {}).get("rid")
        if rid:
            return str(rid)
        if "Captcha" in (j.get("errorType") or ""):
            print("[浏览器] 接口要求验证码 → 转人工模式：请在弹出的浏览器里完成提交")
            return self._human_submit(code, ref, human_wait_s)
        raise RuntimeError(f"提交失败：HTTP {probe.get('status')} {probe.get('head')}")

    def _human_submit(self, code: str, ref: dict, wait_s: int) -> str:
        """打开提交界面，把代码放入编辑器，等待人工点击提交后捕获 rid。"""
        page = self.page
        try:
            page.get_by_text("提交", exact=True).first.click(timeout=5000)
        except Exception:
            page.goto(f"{BASE}/problem/{ref['pid']}", wait_until="domcontentloaded")
            page.wait_for_timeout(2000)
            page.get_by_text("提交", exact=True).first.click(timeout=5000)
        page.wait_for_timeout(1500)
        # 尝试把代码写进编辑器（codemirror 或 textarea）
        page.evaluate(
            "code => {"
            "  const ta = document.querySelector('textarea');"
            "  const cm = document.querySelector('.CodeMirror');"
            "  if (cm && cm.CodeMirror) { cm.CodeMirror.setValue(code); return 'cm'; }"
            "  if (ta) { const s = Object.getOwnPropertyDescriptor("
            "    window.HTMLTextAreaElement.prototype, 'value').set;"
            "    s.call(ta, code); ta.dispatchEvent(new Event('input', {bubbles: true}));"
            "    return 'ta'; }"
            "  return 'none';"
            "}", code)
        print(f"[浏览器] 代码已填入，等待人工提交（{wait_s}s 内）…")
        deadline = time.time() + wait_s
        while time.time() < deadline:
            m = _RECORD_RE.search(page.url)
            if m:
                return m.group(1)
            page.wait_for_timeout(2000)
        raise TimeoutError("等待人工提交超时")

    # ---------- 结果 ----------

    def wait_result(self, rid: str, ref: dict, timeout_s: int = 300) -> dict:
        page = self.page
        deadline = time.time() + timeout_s
        while time.time() < deadline:
            page.goto(f"{BASE}/record/{rid}", wait_until="domcontentloaded", timeout=30000)
            page.wait_for_timeout(4000)
            text = page.inner_text("body")
            if "评测中" in text or "等待中" in text or "Judging" in text:
                time.sleep(5)
                continue
            status = None
            for kw, v in (("Accepted", "AC"), ("Wrong Answer", "WA"),
                          ("Time Limit Exceeded", "TLE"),
                          ("Memory Limit Exceeded", "MLE"),
                          ("Runtime Error", "RE"), ("Compile Error", "CE"),
                          ("Unaccepted", "PARTIAL")):
                if kw in text:
                    status = "PARTIAL" if v == "PARTIAL" else v
                    break
            pts = _PT_RE.findall(text)
            passed = sum(1 for _, s, _ in pts if s == "AC")
            total = len(pts)
            detail = (f"{status or 'UNKNOWN'}（通过测试点 {passed}/{total}）"
                      if total else (status or "UNKNOWN"))
            return {"verdict": status or "UNKNOWN", "passed": passed or None,
                    "total": total or None, "detail": detail,
                    "points": [{"case": n, "verdict": s, "res": r}
                               for r, s, n in pts]}
        return {"verdict": "UNKNOWN", "passed": None, "total": None,
                "detail": f"等待 {timeout_s}s 未出结果（rid={rid}）"}

    def close(self):
        try:
            self.browser.close()
        finally:
            self._pw.stop()
