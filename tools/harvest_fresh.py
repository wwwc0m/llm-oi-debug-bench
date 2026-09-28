"""收割洛谷 Cookie（全新临时 profile，完全脱离旧会话）。"""
import os, re, time, tempfile, shutil
from playwright.sync_api import sync_playwright

os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

pw = sync_playwright().start()
tmp = tempfile.mkdtemp(prefix='harvest_')
ctx = pw.chromium.launch_persistent_context(tmp, channel='msedge', headless=False,
    viewport={'width': 1280, 'height': 900})
page = ctx.pages[0] if ctx.pages else ctx.new_page()
page.goto('https://www.luogu.com.cn/auth/login', wait_until='domcontentloaded', timeout=30000)
print('[harvest] 全新浏览器，请登录（5分钟）...', flush=True)
deadline = time.time() + 300
done = False
while time.time() < deadline:
    time.sleep(5)
    try:
        cookies = {c['name']: c['value'] for c in ctx.cookies()}
    except Exception:
        continue
    uid, cid = cookies.get('_uid'), cookies.get('__client_id')
    if uid and cid:
        s = open('.env', encoding='utf-8').read()
        s = re.sub(r'^LUOGU_UID=.*$', f'LUOGU_UID={uid}', s, flags=re.M)
        s = re.sub(r'^LUOGU_CLIENT_ID=.*$', f'LUOGU_CLIENT_ID={cid}', s, flags=re.M)
        open('.env', 'w', encoding='utf-8', newline='\n').write(s)
        print(f'[harvest] 登录成功 uid={uid}', flush=True)
        done = True
        break
if not done:
    print('[harvest] 超时', flush=True)
ctx.close(); pw.stop()
shutil.rmtree(tmp, ignore_errors=True)
