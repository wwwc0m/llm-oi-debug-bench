"""论文图表生成器。运行：python tools/make_figures.py"""
import json
import os
import sys

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np

plt.rcParams['font.sans-serif'] = ['SimHei', 'Microsoft YaHei', 'DejaVu Sans']
plt.rcParams['axes.unicode_minus'] = False

OUT = 'docs/figures'
os.makedirs(OUT, exist_ok=True)

# ============================================================
# 图1：A/B 修复率对比（核心结论图）
# ============================================================
fig, ax = plt.subplots(figsize=(8, 5))
cats = ['A类\n浅伤修复\n(9题)', 'B类\n深伤失败\n(11题)']
rates = [100, 0]
colors = ['#2ecc71', '#e74c3c']
bars = ax.bar(cats, rates, color=colors, width=0.5, edgecolor='black', linewidth=1.2)
for bar, rate in zip(bars, rates):
    ax.text(bar.get_x() + bar.get_width()/2, rate + 2, f'{rate}%',
            ha='center', va='bottom', fontsize=18, fontweight='bold')
ax.set_ylabel('黑盒反馈修复率 (%)', fontsize=13)
ax.set_title('错误深度决定修复能力：浅伤 vs 深伤', fontsize=15, fontweight='bold')
ax.set_ylim(0, 115)
ax.spines['top'].set_visible(False)
ax.spines['right'].set_visible(False)
plt.tight_layout()
plt.savefig(f'{OUT}/fig1_ab_rate.png', dpi=200)
plt.close()
print('✓ 图1')

# ============================================================
# 图2：修复轨迹对比（A类阶梯 vs B类平台）
# ============================================================
fig, axes = plt.subplots(1, 2, figsize=(14, 6))

# A类轨迹
ax = axes[0]
a_trajectories = {
    'P7951 右方之火': [0, 37, 91, 98, 100],  # 样例退回→24/65→59/65→64/65→65/65
    'P17465 Bell级数': [0, 60, 90, 100],
    'P7982 琴琴的树': [20, 100],
    'P17238 PetalDance': [92, 100],
}
for label, traj in a_trajectories.items():
    ax.plot(range(1, len(traj)+1), traj, 'o-', markersize=8, linewidth=2, label=label)
ax.set_xlabel('提交轮次', fontsize=12)
ax.set_ylabel('通过测试点百分比 (%)', fontsize=12)
ax.set_title('A类：浅伤修复轨迹（阶梯登顶）', fontsize=13, fontweight='bold')
ax.legend(fontsize=9, loc='lower right')
ax.set_ylim(-5, 108)
ax.grid(True, alpha=0.3)
ax.spines['top'].set_visible(False)
ax.spines['right'].set_visible(False)

# B类轨迹
ax = axes[1]
b_trajectories = {
    'P5612 Ynoi': [30, 35, 45, 55, 55],
    'P5655 基础数论': [60, 60, 60, 60, 60],
    'P7922 Pyramid': [26, 26, 26],
    'P7983 practiceZ': [64, 64, 71, 71, 71],
    'P5649 Sone1': [0, 0, 0],
}
for label, traj in b_trajectories.items():
    ax.plot(range(1, len(traj)+1), traj, 's--', markersize=7, linewidth=2, label=label)
ax.axhline(y=100, color='green', linestyle=':', alpha=0.5, label='满分线')
ax.set_xlabel('提交轮次', fontsize=12)
ax.set_ylabel('通过测试点百分比 (%)', fontsize=12)
ax.set_title('B类：深伤失败轨迹（平台滞留）', fontsize=13, fontweight='bold')
ax.legend(fontsize=9, loc='lower right')
ax.set_ylim(-5, 108)
ax.grid(True, alpha=0.3)
ax.spines['top'].set_visible(False)
ax.spines['right'].set_visible(False)

plt.suptitle('修复轨迹对比：阶梯登顶 vs 平台滞留', fontsize=15, fontweight='bold', y=1.02)
plt.tight_layout()
plt.savefig(f'{OUT}/fig2_trajectories.png', dpi=200, bbox_inches='tight')
plt.close()
print('✓ 图2')

# ============================================================
# 图3：首提交得分 × 结局 热力图
# ============================================================
fig, ax = plt.subplots(figsize=(8, 5))
# 首分区间 × A/B 类计数
data = np.array([
    [3, 0],   # ≥90%
    [1, 5],   # 60-89%
    [3, 2],   # 20-59%
    [2, 4],   # <20%
])
labels_y = ['≥90%', '60-89%', '20-59%', '<20%']
labels_x = ['A类(修复成功)', 'B类(修复失败)']
im = ax.imshow(data, cmap='RdYlGn', aspect='auto', vmin=0, vmax=6)
for i in range(len(labels_y)):
    for j in range(len(labels_x)):
        ax.text(j, i, str(data[i, j]), ha='center', va='center',
                fontsize=16, fontweight='bold')
ax.set_xticks(range(len(labels_x)))
ax.set_xticklabels(labels_x, fontsize=12)
ax.set_yticks(range(len(labels_y)))
ax.set_yticklabels(labels_y, fontsize=12)
ax.set_xlabel('结局', fontsize=13)
ax.set_ylabel('首提交得分区间', fontsize=13)
ax.set_title('首提交得分与修复结局的关系\n（中间带 60-89% 几乎全灭 → 深伤陷阱）', fontsize=14, fontweight='bold')
plt.colorbar(im, ax=ax, label='实例数')
plt.tight_layout()
plt.savefig(f'{OUT}/fig3_firstscore_heatmap.png', dpi=200)
plt.close()
print('✓ 图3')

# ============================================================
# 图4：成本对比（token 消耗按类别）
# ============================================================
fig, ax = plt.subplots(figsize=(10, 6))
categories = ['一次过\n(已清除)', 'A类\n修复成功', 'B类\n修复失败', '发散/\n过度验证']
avg_prompt = [50, 500, 800, 1200]   # 千tokens
avg_completion = [25, 100, 130, 170]
x = np.arange(len(categories))
w = 0.35
b1 = ax.bar(x - w/2, avg_prompt, w, label='Prompt (K)', color='#3498db', edgecolor='black')
b2 = ax.bar(x + w/2, avg_completion, w, label='Completion (K)', color='#e67e22', edgecolor='black')
for bars in [b1, b2]:
    for bar in bars:
        h = bar.get_height()
        ax.text(bar.get_x() + bar.get_width()/2, h + 10, f'{h:.0f}K',
                ha='center', va='bottom', fontsize=10)
ax.set_xlabel('结局类别', fontsize=13)
ax.set_ylabel('平均 Token 消耗（千）', fontsize=13)
ax.set_title('成本断崖：修复失败的代价是秒杀的 16 倍', fontsize=14, fontweight='bold')
ax.set_xticks(x)
ax.set_xticklabels(categories, fontsize=11)
ax.legend(fontsize=11)
ax.spines['top'].set_visible(False)
ax.spines['right'].set_visible(False)
plt.tight_layout()
plt.savefig(f'{OUT}/fig4_cost.png', dpi=200)
plt.close()
print('✓ 图4')

print(f'\n全部图表已生成到 {OUT}/')
