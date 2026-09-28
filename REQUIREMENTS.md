# REQUIREMENTS.md — 环境与依赖

## 项目地址

- **GitHub（私有）**：`git@github.com:wwwc0m/llm-oi-debug-bench.git`
  （HTTPS：`https://github.com/wwwc0m/llm-oi-debug-bench.git`，需协作者权限）

## 系统要求

| 项 | 要求 | 当前实验机 |
|----|------|-----------|
| 操作系统 | Windows 10/11（依赖 Edge 与 MinGW 路径习惯；Linux 需改 `browser_backend.py` 的 channel 与 `.exe` 后缀） | Windows 10 x64 |
| Python | **3.10+**（代码使用 `X \| None` 语法），实测 3.13 | 3.13.13 |
| 编译器 | g++（MinGW-w64 即可），需在 PATH；判题编译命令 `g++ -O2 -std=c++17` | gcc 16.1.0 (MinGW-w64 UCRT) |
| 浏览器 | Microsoft Edge（Playwright `channel="msedge"` 驱动系统 Edge，无需下载 Chromium） | 系统自带 |
| 网络 | 可访问 `www.luogu.com.cn`、`open.bigmodel.cn`、`api.deepseek.com` | — |

## Python 依赖

```bash
pip install -r requirements.txt
```

（openai、python-dotenv、psutil、requests、beautifulsoup4、matplotlib、scipy、numpy、playwright）
注意：Playwright **不需要** `playwright install`——使用系统 Edge；其自带 CDN 在国内不可达。

## MCP 依赖

**无。** 本项目不依赖任何 MCP 服务器——浏览器自动化由项目内置的 Playwright 代码
（`runner/browser_backend.py`）完成，不使用 browser-use 等 MCP。
若由 ZCode/Claude Code 等 agent 操作，仅需其自带的基础工具（Bash/文件读写）。

## 凭据（`.env`，已被 .gitignore，绝不入库）

```ini
# LLM 被试（4个）
ZHIPU_API_KEY=...        # GLM Coding Plan 订阅 key（端点 open.bigmodel.cn/api/coding/paas/v4）
DEEPSEEK_API_KEY=...     # api.deepseek.com

# 洛谷判题（建议专用小号；浏览器 F12 → Application → Cookies 导出）
LUOGU_UID=...
LUOGU_CLIENT_ID=...
```

被试模型（`runner/providers.py` 注册，`BENCH_MODELS` 环境变量可覆盖）：

| 标识 | 端点 |
|------|------|
| `zhipu:glm-5.3` / `zhipu:glm-5.3-flash` | open.bigmodel.cn/api/coding/paas/v4 |
| `deepseek:deepseek-v4-pro` / `deepseek:deepseek-flash` | api.deepseek.com |

## 洛谷账号说明

- 实验用账号：uid `2312631`（wuc454，专用小号）
- Cookie 有效性：`__client_id` 与 `_uid` 失效（约数周）或账号在别处登录可能导致风控升级，
  表现为题目页也被反爬拦截；重新从浏览器导出 Cookie 更新 `.env` 即可
- 提交通道为非官方网页接口，受图片验证码保护：**每次真实提交需要人工点击一次验证码**；
  两次提交强制间隔 ≥60s（代码内置）。规模化请申请洛谷开放平台（k@luogu.org）

## 自检

```bash
python tests/test_smoke.py      # 无网络/无密钥也可运行，验证全链路逻辑
python -m runner.cli models     # 验证 4 个被试 API 连通
```
