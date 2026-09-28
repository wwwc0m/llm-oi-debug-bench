"""等待用户在弹出的浏览器里登录洛谷，自动提取 _uid/__client_id 写入 .env。

运行：python tools/harvest_login.py（不预注入任何 Cookie）
"""
import os
import re
import time

from dotenv import load_dotenv
from playwright.sync_api import sync_playwright

load_dotenv()
PROFILE = os.path.join(os.getcwd(), "browser_profile_main")


def main():
    pw = sync_playwright().start()
    ctx = pw.chromium.launch_persistent_context(
        PROFILE, channel="msedge", headless=False,
        viewport={"width": 1280, "height": 900})
    page = ctx.pages[0] if ctx.pages else ctx.new_page()
    try:
        page.goto("https://www.luogu.com.cn/auth/login",
                  wait_until="domcontentloaded", timeout=30000)
        # 清掉可能残留的旧会话
        ctx.clear_cookies()
        page.reload(wait_until="domcontentloaded")
        print("[harvest] 干净浏览器已开，请在窗口中登录（最长 5 分钟）...", flush=True)
        deadline = time.time() + 300
        while time.time() < deadline:
            time.sleep(5)
            try:
                cookies = {c["name"]: c["value"] for c in ctx.cookies()}
            except Exception:
                continue
            uid, cid = cookies.get("_uid"), cookies.get("__client_id")
            if uid and cid:
                s = open(".env", encoding="utf-8").read()
                s = re.sub(r"^LUOGU_UID=.*$", f"LUOGU_UID={uid}", s, flags=re.M)
                s = re.sub(r"^LUOGU_CLIENT_ID=.*$", f"LUOGU_CLIENT_ID={cid}", s, flags=re.M)
                if "LUOGU_UID" not in s:
                    s += f"\nLUOGU_UID={uid}\nLUOGU_CLIENT_ID={cid}\n"
                open(".env", "w", encoding="utf-8", newline="\n").write(s)
                print(f"[harvest] 新账号登录成功！uid={uid} 已写入 .env", flush=True)
                return
        print("[harvest] 5 分钟未登录，退出（可重跑）", flush=True)
    finally:
        ctx.close()
        pw.stop()


if __name__ == "__main__":
    main()
