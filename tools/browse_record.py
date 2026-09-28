"""用系统 Edge 读取洛谷评测记录的逐测试点详情。

数据获取双通道：
  A) 拦截页面 XHR 的 JSON 响应（结构化，含 detail.subtasks）
  B) 兜底：解析 DOM（测试点色块的 title/文本）

运行：python tools/browse_record.py [rid]
不带参数时自动取记录列表最新一条。
"""
import json
import os
import re
import sys

from dotenv import load_dotenv
from playwright.sync_api import sync_playwright

load_dotenv()
UID = os.environ["LUOGU_UID"]
CID = os.environ["LUOGU_CLIENT_ID"]


def main():
    rid = sys.argv[1] if len(sys.argv) > 1 else None

    with sync_playwright() as p:
        browser = p.chromium.launch(channel="msedge", headless=False)
        ctx = browser.new_context(viewport={"width": 1280, "height": 900})
        ctx.add_cookies([
            {"name": "_uid", "value": UID, "domain": ".luogu.com.cn", "path": "/"},
            {"name": "__client_id", "value": CID, "domain": ".luogu.com.cn", "path": "/"},
        ])
        page = ctx.new_page()
        captured = []
        page.on("response", lambda r: captured.append(r) if (
            "record" in r.url and r.status == 200) else None)

        if rid is None:
            page.goto(f"https://www.luogu.com.cn/record/list?user={UID}",
                      wait_until="domcontentloaded", timeout=30000)
            page.wait_for_timeout(4000)
            print("列表页标题:", page.title())
            ids = re.findall(r"/record/(\d+)", page.content())
            if not ids:
                print("DOM 里没找到记录链接")
                browser.close()
                sys.exit(1)
            rid = ids[0]
            print("最新 rid:", rid)

        page.goto(f"https://www.luogu.com.cn/record/{rid}",
                  wait_until="domcontentloaded", timeout=30000)
        page.wait_for_timeout(3000)
        print("详情页标题:", page.title())

        # 在页面上下文内 fetch JSON 接口（携带浏览器全部 Cookie/指纹，风控已过）
        probe = page.evaluate(
            "async rid => {"
            "  const r = await fetch(`/record/${rid}?_contentOnly=1`,"
            "    {headers: {'x-luogu-type': 'content-only'}});"
            "  const t = await r.text();"
            "  let j = null;"
            "  try { j = JSON.parse(t); } catch (e) {}"
            "  return {status: r.status, head: t.slice(0, 120), json: j};"
            "}", rid)
        print("in-page fetch 状态:", probe["status"], "| 响应头120字:", probe["head"][:120].replace("\n", " "))
        payload = probe["json"]
        record_json = (payload.get("currentData") or {}).get("record") if payload else None

        if record_json:
            print("记录元信息:", {k: record_json.get(k)
                                 for k in ("rid", "status", "score", "lang", "time", "memory")})
            detail = record_json.get("detail") or {}
            print("detail 键:", list(detail.keys()))
            with open("results/luogu_record_probe.json", "w", encoding="utf-8") as f:
                json.dump({"rid": rid, "record": record_json}, f,
                          ensure_ascii=False, indent=2)
            print("结构化数据已存 results/luogu_record_probe.json")
        else:
            print("未捕获结构化数据。全部捕获响应：")
            for r in captured:
                ct = r.headers.get("content-type", "")
                body_head = ""
                try:
                    body_head = r.text()[:150].replace("\n", " ")
                except Exception:
                    body_head = "<不可读>"
                print(f"  [{r.status}] {r.url[:90]}")
                print(f"        type={ct} | body[:150]={body_head}")

        # 通道 B：正文文本 + 完整 HTML 存档
        body_text = page.inner_text("body")
        print("=== 页面正文（前3000字）===")
        print(body_text[:3000])
        with open("results/luogu_record_page.html", "w", encoding="utf-8") as f:
            f.write(page.content())
        print("完整 HTML 已存 results/luogu_record_page.html")
        browser.close()


if __name__ == "__main__":
    main()
