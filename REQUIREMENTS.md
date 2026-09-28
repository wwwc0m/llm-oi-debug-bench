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

## 首次使用配置（必读）

### 第 1 步：创建 `.env`

```bash
cp .env.example .env
```

`.env` 已被 `.gitignore` 排除，**永远不要提交到仓库**。

### 第 2 步：填写 LLM API Key（4 个被试模型）

| 环境变量 | 值的来源 |
|----------|---------|
| `ZHIPU_API_KEY` | 智谱开放平台 GLM Coding Plan 订阅的 API Key（`xxx.yyy` 长格式）。**注意必须走 coding plan 端点**（代码已内置 `open.bigmodel.cn/api/coding/paas/v4`）；若误用按量付费 key 会报 429"余额不足" |
| `DEEPSEEK_API_KEY` | DeepSeek 开放平台的 `sk-` 开头 key（api.deepseek.com） |

一个 key 可覆盖同厂两个模型（glm-5.3 / glm-5.3-flash 共用一个，deepseek 两个共用一个）。

### 第 3 步：填写洛谷 Cookie（**不填则无法提交判题**）

拉题（fetch）匿名即可，但**提交代码必须带登录态**。需要从浏览器导出两个 Cookie 值：

1. 用浏览器登录 [luogu.com.cn](https://www.luogu.com.cn)（**强烈建议注册专用小号**，勿用主力账号）
2. 按 `F12` 打开开发者工具 → 顶部选 **Application**（应用）标签
   → 左侧 **Storage → Cookies → `https://www.luogu.com.cn`**
3. 在 Cookie 列表中找到这两行，复制各自的 **Value**：

| Cookie 名 | 填入 `.env` 的变量 |
|-----------|-------------------|
| `_uid`（纯数字，你的用户 ID） | `LUOGU_UID` |
| `__client_id`（48 位十六进制） | `LUOGU_CLIENT_ID` |

```ini
LUOGU_UID=2312631
LUOGU_CLIENT_ID=yjpiwjkmpd37...（48位）
```

**Cookie 失效的表现与处理**：洛谷会话过期（数周）、账号在其他设备重新登录、或触发风控时，
提交会报错或页面被反爬拦截。处理：重新按上述步骤导出一次新值，更新 `.env` 即可，无需改代码。

### 第 4 步：验证

```bash
python tests/test_smoke.py      # 离线全链路自检（不需要任何密钥）
python -m runner.cli models     # 应列出 4 个被试模型（验证 API key）
python -m runner.cli fetch --pid P1129   # 验证洛谷连通（匿名即可）
```

三项都通过即可开跑实验。**跑实验时人要在电脑前**：每次真实提交洛谷会弹图片验证码，需人工点击。


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
