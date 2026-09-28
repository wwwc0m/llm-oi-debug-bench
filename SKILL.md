# SKILL.md — Agent 操作手册

> 本文档面向**被要求操作本项目的 AI agent**（ZCode / Claude Code / 其他 CLI agent）。
> 目标：按标准流程跑实验、记录进度、不破坏数据。环境准备见 [REQUIREMENTS.md](REQUIREMENTS.md)。

## 项目是什么

评测各大 LLM 在**洛谷 OI 黑盒反馈**下调试算法代码的能力：
模型读题写码 → 本地预检（编译+样例）→ 提交洛谷 → 只拿到"第 N 点 WA/TLE"级反馈 →
循环修正，**每轮实验最多 5 次真实提交**，产出逐轮代码与结果供论文分析。

## 一分钟上手

```bash
cd llm-oi-debug-bench
python tests/test_smoke.py          # 离线自检，必须通过（不需要任何密钥）
python -m runner.cli models         # 确认 4 个被试模型可用
python -m runner.cli run --problem P1129 --model deepseek:deepseek-v4-pro
```

## 核心命令

| 命令 | 用途 |
|------|------|
| `python -m runner.cli fetch --pid Pxxxx` | 从洛谷拉题到 `experiments/Pxxxx/`（只读请求，无验证码） |
| `python -m runner.cli run --problem Pxxxx --model provider:model` | 跑一次实验（一题 × 一模型） |
| `python -m runner.cli batch --problem Pxxxx --repeat N` | 一题 × 全部可用模型 × N 次重复 |
| `python -m runner.cli models` | 列出 `.env` 中可用的模型 |
| `python tests/test_smoke.py` | 离线冒烟测试 |

## 实验流程（每个 run 的生命周期）

1. `run` 启动后自动：加载本地题库 → 启动 Edge → LLM 循环（compile / run_sample / submit 三工具）
2. **提交前本地预检**：编译失败或题面样例未过 → 退回给模型（**不计入 5 次**，记录在 session.jsonl 的 `local_gate:*` 条目）
3. 预检通过 → 浏览器自动填码、自动点提交 → **弹出图片验证码时需要人工点击**（这是人机环节，见下方铁律）
4. 结果页自动解析逐测试点状态，黑盒摘要回传模型，直到 AC 或 5 次用尽
5. 产物落盘 `experiments/<题号>/runs/<模型>/run<k>/`：`roundN.cpp`、`roundN.result.json`、
   `session.jsonl`（完整轨迹）、`summary.json`（结论）
6. **自动追加进度行到 `PROGRESS.md`**（无需手工维护）

## Agent 铁律

1. **不要绕过/自动化人机验证码**。浏览器弹出验证码时提醒用户点击；等待窗口 180 秒。
   不要使用打码服务、指纹伪装、无头浏览器等对抗手段。
2. **不要提交 `.env`**（含 API key 与洛谷 Cookie）；不要修改 `experiments/*/runs/` 下已有产物。
3. **两次真实提交之间至少间隔 60 秒**（代码已内置限速，不要绕过）；尊重洛谷
   "个人练习免费、自动化批量需走开放平台"的声明。
4. 跑实验前确认：`.env` 三项齐全（`ZHIPU_API_KEY`、`DEEPSEEK_API_KEY`、
   `LUOGU_UID`+`LUOGU_CLIENT_ID` 洛谷 Cookie，导出方法见 REQUIREMENTS.md 第 3 步；
   **缺洛谷 Cookie 时提交必失败**）；用户在电脑前（需要点验证码）；
   `python tests/test_smoke.py` 通过。
5. 实验中断/失败：run 目录会保留部分 session.jsonl，属正常记录，不要删除；
   重新运行会自动创建 run<k+1>。
6. 每次实验结束（无论成败）检查 `PROGRESS.md` 是否新增了对应行；batch 结束检查
   `experiments/<题号>/runs/batch_summary.json`。
7. **严禁删除 runs/ 目录**（哪怕看似"半截"）。session.jsonl 里存着每次工具调用的完整代码，
'
   '   是中断恢复的唯一依据；清理必须先核对 quota.json 与提交记录并经用户确认。
'
   '   （两次教训：P17254 四次提交、P17304/P17344 首版代码，均因误删丢失/靠洛谷救回。）

## 数据结构速查

```
experiments/<题号>/
├── problem.json          # 元信息：难度/时限/样例/source
├── statement.md          # 题面缓存（实验不联网拉题）
├── assets/               # 参考解与注错变体（如已配置）
└── runs/<模型>/run<k>/    # 每轮实验产物（roundN.cpp / roundN.result.json / session.jsonl / summary.json）
```

## 分析（论文用）

- `runs/*/session.jsonl`：逐轮工具调用、代码、黑盒反馈、`local_gate:*` 本地退回记录
- 指标：首过率 / K(=5) 轮内修复率 / 伪修复率 / 回归率 / 按 WA-TLE 分层的修复率
- 图表脚本位于 `analysis/`（待建）
