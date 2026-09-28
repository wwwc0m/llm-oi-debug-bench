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
import tempfile
import time

from playwright.sync_api import sync_playwright

BASE = "https://www.luogu.com.cn"

_PT_RE = re.compile(r"(\d+(?:\.\d+)?m?s/[0-9.]+[KMG]B)\s*(AC|WA|TLE|MLE|RE|UKE)\s*#(\d+)")
_RECORD_RE = re.compile(r"/record/(\d+)")
_LOCK_PATH = os.path.join(tempfile.gettempdir(), "bench_submit.lock")


def _acquire_submit_lock(timeout_s: int = 360):
    """跨进程提交互斥：同一时刻只允许一路浏览器走 填码→验证码→提交 流程。"""
    deadline = time.time() + timeout_s
    while time.time() < deadline:
        try:
            fd = os.open(_LOCK_PATH, os.O_CREAT | os.O_EXCL | os.O_WRONLY)
            os.write(fd, str(os.getpid()).encode())
            os.close(fd)
            return
        except FileExistsError:
            try:
                if time.time() - os.path.getmtime(_LOCK_PATH) > 360:  # 陈旧锁清理
                    os.remove(_LOCK_PATH)
                    continue
            except OSError:
                pass
            time.sleep(3)
    raise TimeoutError("等待提交锁超时（其他进程提交中）")


def _release_submit_lock():
    try:
        os.remove(_LOCK_PATH)
    except OSError:
        pass


class LuoguBrowser:
    # 持久化配置目录：浏览器 Cookie/历史跨实验保留，积累 Cloudflare 信任
    # （每次换全新临时配置 = 同 IP 反复出现"全新浏览器"，反而更可疑）
    # 并行跑多路实验时用环境变量 BENCH_BROWSER_PROFILE 指定各自独立目录
    PROFILE_DIR = os.environ.get(
        "BENCH_BROWSER_PROFILE",
        os.path.join(os.getcwd(), "browser_profile"))

    def __init__(self, headless: bool = False):
        self._pw = sync_playwright().start()
        self.ctx = self._pw.chromium.launch_persistent_context(
            self.PROFILE_DIR, channel="msedge", headless=headless,
            viewport={"width": 1280, "height": 900})
        uid, cid = os.environ.get("LUOGU_UID", ""), os.environ.get("LUOGU_CLIENT_ID", "")
        if uid and cid:
            self.ctx.add_cookies([
                {"name": "_uid", "value": uid, "domain": ".luogu.com.cn", "path": "/"},
                {"name": "__client_id", "value": cid, "domain": ".luogu.com.cn", "path": "/"},
            ])
        try:
            self.ctx.grant_permissions(["clipboard-read", "clipboard-write"],
                                       origin="https://www.luogu.com.cn")
        except Exception:
            pass
        self.page = self.ctx.pages[0] if self.ctx.pages else self.ctx.new_page()

    # ---------- 提交 ----------

    def submit(self, code: str, lang: str, ref: dict, human_wait_s: int = 180) -> str:
        """对话式提交（实测流程）：题目页点"提交答案"→ CM6 编辑器剪贴板填充 →
        自动点"提交评测"→ 若出人机验证等人工完成 → 捕获 /record/{rid} 跳转。"""
        page = self.page
        print(f"[浏览器] 打开题目页 {ref['pid']} ...", flush=True)
        page.goto(f"{BASE}/problem/{ref['pid']}", wait_until="domcontentloaded",
                  timeout=30000)
        page.wait_for_timeout(3000)
        try:
            page.bring_to_front()  # 弹到前台，方便人工过验证
        except Exception:
            pass
        # 打开提交对话框（工具栏菜单里的"提交答案"入口）
        print("[浏览器] 点击 提交答案 ...", flush=True)
        page.get_by_text("提交答案", exact=True).first.click(timeout=10000)
        page.wait_for_timeout(2500)
        # 填充代码：剪贴板 + Ctrl+A / Ctrl+V（CodeMirror 6）
        print("[浏览器] 写入剪贴板并填充编辑器 ...", flush=True)
        page.evaluate("c => navigator.clipboard.writeText(c)", code)
        page.locator(".cm-content").first.click(timeout=10000)
        page.keyboard.press("Control+a")
        page.keyboard.press("Control+v")
        page.wait_for_timeout(1000)
        n = page.evaluate(
            "() => (document.querySelector('.cm-content')||{textContent:''}).textContent.length")
        print(f"[浏览器] 代码已填入编辑器（{n} 字符），点击 提交评测 ...", flush=True)
        page.get_by_text("提交评测", exact=True).first.click(timeout=10000)

        deadline = time.time() + human_wait_s
        warned = False
        auto_tried = 0
        while time.time() < deadline:
            page.wait_for_timeout(2000)
            m = _RECORD_RE.search(page.url)
            if m:
                return m.group(1)
            if not warned:
                # 洛谷图片点选验证码文案为"请点击…"开头；Turnstile 为 iframe
                body = page.inner_text("body")[:3000]
                has_captcha_text = any(kw in body for kw in
                                       ("验证", "人机", "请点击", "Click"))
                if has_captcha_text or self._srcless_iframe():
                    print("[浏览器] 检测到人机验证：请在浏览器窗口人工完成，提交会自动继续…",
                          flush=True)
                    try:
                        page.bring_to_front()
                    except Exception:
                        pass
                    warned = True
        raise TimeoutError(f"等待提交跳转超时（{human_wait_s}s）；当前 URL: {page.url}")

    def _srcless_iframe(self) -> bool:
        """洛谷图片点选验证码渲染在无 src 的内嵌 iframe 中（约 300x200 可见块）。"""
        try:
            return self.page.evaluate(
                "() => [...document.querySelectorAll('iframe')]"
                ".some(f => !f.src && f.offsetWidth > 50 && f.offsetHeight > 50)")
        except Exception:
            return False

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
            if status is None and not pts:
                # 页面可能未加载完成（P16926 UNKNOWN 事故）：继续轮询，超时才返回 UNKNOWN
                time.sleep(5)
                continue
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
            self.ctx.close()
        finally:
            self._pw.stop()
