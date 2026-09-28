# REFERENCES.md — 研究方向文献对照

> 本项目各实验发现 ↔ 已有文献的映射，以及我们的差异化贡献点。
> 检索日期：2026-09-28。

## 一、基础背景（AI × 算法设计）

- **AlphaCode**（DeepMind, Science 2022）与 **AlphaCode 2**：大规模采样 + 聚类的竞赛编程系统
- **AlphaDev**（Nature 2023）/ **FunSearch**（Nature 2024）/ **AlphaEvolve**（2025）：
  搜索式算法发现及其边界（本课题最初的文献起点，见 docs/experiment-design.md）
- **[Competitive Programming with Large Reasoning Models](https://arxiv.org/abs/2502.06807)**（arXiv 2502.06807）：
  o1/o1-ioi/o3 在竞赛编程上的表现与难度分层

## 二、自调试与迭代修复（我们的主干范式）

- **Teaching Large Language Models to Self-Debug**（Chen et al., Google）：执行反馈驱动的自调试奠基工作
- **[DePro](https://huggingface.co/papers)**：测试用例驱动的 LLM 竞程调试实证研究——与本项目最接近
- **PerfCodeGen**（2024）：运行时反馈改进生成代码
- **Fine-Grained Diagnostic Feedback for LLM Code Refinement**：论证"测试失败信息是弱信号"
- **Code Repair with LLMs: Exploration-Exploitation Tradeoff**（[alphaXiv](https://www.alphaxiv.org)）：把迭代修复建模为探索-利用权衡

**我们的差异**：上述工作大多给模型**丰富反馈**（失败测试内容/报错全文）。本实验刻意采用
**OI 黑盒贫信息反馈**（仅测试点通过/失败 + WA/TLE 判定）+ **提交配额约束**（每次反馈有真实代价，
K=5）+ **真实 OJ 在线判题**（洛谷），测量的是"资源受限、信息受限"下的调试能力——
这是真实竞赛环境与上述实验室设定的关键差别。

## 三、推理失控 / 过度思考（我们记录的烧穿现象）

- **[Stop Overthinking: A Survey on Efficient Reasoning](https://arxiv.org/html/2503.16419v2)**（arXiv 2503.16419, TMLR）：
  正式定义 overthinking 问题与缓解策略分类
- **[A Survey of Efficient Reasoning for Large Reasoning Models](https://arxiv.org/html/2503.21614v1)**（arXiv 2503.21614）
- **Efficient Reasoning Models: A Survey**（[OpenReview](https://openreview.net/forum?id=sySqlxj8EB)）

**我们的差异**：现有文献聚焦"简单问题上浪费思考"；我们实测的是**难题上的预算耗尽**——
65K 输出上限被纯推理烧穿、零交付（deepseek 两型号多轮复现，见 P12911/P17237 战役），
以及"信息悖论"（题面越完整思考越深反而零产出）。这是 overthinking 文献中少有的
**任务侧极端样本**。

## 四、竞程基准与难度/类型分析

- **[ProBench](https://arxiv.org/html/2502.20868v1)**（arXiv 2502.20868）：CF+**洛谷**+Nowcoder 三平台基准——直接相关
- **[Code Generation with Small LMs](https://arxiv.org/html/2504.07343v2)**：280 道 CF 题按难度分层评测
- **[Are LLMs Ready for Competitive Programming?](https://dl.acm.org/doi/10.1145/3789595.3789613)**（ACM）：教育视角实证
- Codeforces Elo 对 LLM 评测的偏性讨论（[LinkedIn 分析](https://www.linkedin.com/posts/dayilin_when-elo-lies-hidden-biases-in-codeforces-based-activity-7427797618183045120-cYLg)）

**我们的差异**：题库全部为**训练截止后上传的冷门题**（防记忆污染的天然对照），并按
**题目类型 × 通过方式 × 成本**做细粒度分析（首过率/修复轮数/token 断崖），
而非单一 pass@k。已发现"模式熟题秒杀（5K tokens）vs 需推理题百万 tokens"的成本断崖。

## 五、行为异常样本（对应通用现象文献）

- 样例硬编码（glm-5.3 无题面事件）↔ 规约博弈 / reward hacking 文献
- 调试输出泄漏（两 DS 型号独立复现）↔ 代码卫生/规范遵循失败
- 盲重交 vs 自测驱动 ↔ 探索-利用（见二）

## 六、机制 A/B（上下文工程对修复率的影响）

- P17451 对照实验：无历史保留机制 4 连败+放弃（3.04M tokens）→ 加机制后 2 轮修复（1.46M tokens）
- 对应 Reflexion / agent 记忆管理脉络；我们提供了"长对话截断导致修复盲改"的
  对照证据与低成本解法（固定提交历史消息）

## 七、本课题的定位一句话

在**真实 OJ、黑盒贫信息反馈、提交配额约束**下，测量与剖析 LLM 的算法调试能力边界：
贡献四类一手证据——推理预算失控的任务侧极端样本、类型×成本图谱、
贫信息修复的行为策略分类、以及上下文工程对修复率的因果性 A/B。
