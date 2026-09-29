"""从模板生成论文正文（保留前四页，替换第26段起，严格按格式规范）。"""
import os

import docx.enum.text
from docx import Document
from docx.shared import Pt, Cm
from docx.enum.text import WD_ALIGN_PARAGRAPH, WD_LINE_SPACING
from docx.oxml.ns import qn

TEMPLATE = '../2025年秋《算法设计与分析实验》实验报告-班级-学号-姓名（模板）.docx'
OUTPUT = '../AI在算法设计中不能做什么-实验报告.docx'
FIGURES = 'docs/figures'

doc = Document(TEMPLATE)

# ============================================================
# 工具函数（严格按 THESIS_FORMAT.md 规范）
# ============================================================

def set_font(run, cn_font='宋体', size=Pt(12), bold=False):
    """设置中西文字体。cn_font为中文字体名，西文自动用Times New Roman。"""
    run.font.name = 'Times New Roman'  # 西文字体
    run._element.rPr.rFonts.set(qn('w:eastAsia'), cn_font)  # 中文字体
    run.font.size = size
    run.font.bold = bold

def add_body(doc, text, indent=True):
    """正文：宋体小4号(12pt)，1.5倍行距，两端对齐。"""
    p = doc.add_paragraph()
    run = p.add_run(text)
    set_font(run, '宋体', Pt(12), False)
    p.alignment = WD_ALIGN_PARAGRAPH.JUSTIFY
    pf = p.paragraph_format
    pf.line_spacing_rule = WD_LINE_SPACING.ONE_POINT_FIVE
    pf.space_after = Pt(6)
    if indent:
        pf.first_line_indent = Pt(24)  # 首行缩进2字符
    return p

def add_h1(doc, text, first_chapter=False):
    """章标题：黑体小2号(18pt)加粗居中，章前分页。"""
    if not first_chapter:
        # 分页符
        pb = doc.add_paragraph()
        pb_run = pb.add_run()
        pb_run.add_break(docx.enum.text.WD_BREAK.PAGE)
        pb.paragraph_format.space_after = Pt(0)
    p = doc.add_paragraph()
    run = p.add_run(text)
    set_font(run, '黑体', Pt(18), True)
    p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    p.paragraph_format.space_before = Pt(12)
    p.paragraph_format.space_after = Pt(12)
    return p

def add_h2(doc, text):
    """节标题：黑体4号(14pt)加粗。"""
    p = doc.add_paragraph()
    run = p.add_run(text)
    set_font(run, '黑体', Pt(14), True)
    p.paragraph_format.space_before = Pt(12)
    p.paragraph_format.space_after = Pt(6)
    return p

def add_h3(doc, text):
    """子节标题：黑体小4号(12pt)加粗。"""
    p = doc.add_paragraph()
    run = p.add_run(text)
    set_font(run, '黑体', Pt(12), True)
    p.paragraph_format.space_before = Pt(6)
    p.paragraph_format.space_after = Pt(4)
    return p

def add_figure(doc, path, width_cm=14, caption=''):
    """插图+图标题（图标题在图下方，黑体小4号）。"""
    p = doc.add_paragraph()
    p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    run = p.add_run()
    run.add_picture(path, width=Cm(width_cm))
    p.paragraph_format.space_before = Pt(6)
    if caption:
        cp = doc.add_paragraph()
        cp.alignment = WD_ALIGN_PARAGRAPH.CENTER
        cr = cp.add_run(caption)
        set_font(cr, '黑体', Pt(12))  # 黑体小4号
        cp.paragraph_format.space_after = Pt(6)

def add_table(doc, headers, rows, caption=''):
    """表格（表标题在上方，黑体小4号；表内容宋体5号）。"""
    if caption:
        cp = doc.add_paragraph()
        cp.alignment = WD_ALIGN_PARAGRAPH.CENTER
        cr = cp.add_run(caption)
        set_font(cr, '黑体', Pt(12))  # 黑体小4号
        cp.paragraph_format.space_before = Pt(6)
    table = doc.add_table(rows=len(rows)+1, cols=len(headers))
    table.style = 'Table Grid'
    for j, h in enumerate(headers):
        cell = table.rows[0].cells[j]
        cell.text = ''
        run = cell.paragraphs[0].add_run(h)
        set_font(run, '宋体', Pt(10.5), True)  # 宋体5号加粗
        cell.paragraphs[0].alignment = WD_ALIGN_PARAGRAPH.CENTER
    for i, row in enumerate(rows):
        for j, val in enumerate(row):
            cell = table.rows[i+1].cells[j]
            cell.text = ''
            run = cell.paragraphs[0].add_run(str(val))
            set_font(run, '宋体', Pt(10.5))  # 宋体5号
            cell.paragraphs[0].alignment = WD_ALIGN_PARAGRAPH.CENTER
    doc.add_paragraph()  # 表后空行

def add_ref(doc, text):
    """参考文献：宋体小4号。"""
    p = doc.add_paragraph()
    run = p.add_run(text)
    set_font(run, '宋体', Pt(12), False)
    p.paragraph_format.line_spacing_rule = WD_LINE_SPACING.ONE_POINT_FIVE
    return p

# ============================================================
# 删除模板正文（第26段起）
# ============================================================
body_elements = list(doc.element.body)
target_idx = None
para_count = 0
for i, el in enumerate(body_elements):
    if el.tag.endswith('}p'):
        if para_count == 26:
            target_idx = i
            break
        para_count += 1
if target_idx is not None:
    to_remove = [el for el in body_elements[target_idx:] if not el.tag.endswith('}sectPr')]
    for el in to_remove:
        doc.element.body.remove(el)
    print(f'已删除模板正文 {len(to_remove)} 个元素')

# ============================================================
# 第1章（首章，不加分页符——前四页已有内容）
# ============================================================
add_h1(doc, '1  AI在算法设计中不能做什么——实验设计', first_chapter=True)

add_h2(doc, '1.1  问题背景')
add_body(doc, '近年来，以DeepSeek、GPT为代表的大语言模型（LLM）在代码生成领域取得了显著进展，'
    '能够根据自然语言描述生成可编译、可运行的程序。然而，"能写代码"不等于"能设计算法"。'
    'Google DeepMind的AlphaCode、FunSearch等工作已经证明，LLM可以成为算法搜索的有力工具，'
    '但这些系统均依赖人类预先定义的评估函数和外部验证器，模型本身并不具备独立判断算法正确性的能力。')
add_body(doc, '本实验提出一个具体的研究问题：在模拟信息学竞赛（OI）的黑盒评测环境下，'
    '大模型能否通过迭代调试修复自己的算法代码？具体而言，模型提交代码后仅获得"第N个测试点'
    '通过/未通过"以及WA（答案错误）、TLE（超时）等判定信息，无法看到测试数据内容。'
    '这种贫信息反馈环境是真实竞赛的常态，也是检验AI算法调试能力的最严格场景。')

add_h2(doc, '1.2  实验系统设计')
add_body(doc, '我们构建了一套完整的自动化实验系统。系统由三个核心模块组成：（1）LLM代理：'
    '通过API调用大模型，模型可使用三个工具——本地编译（g++编译并查看报错）、'
    '样例自测（在题面样例上运行比对）、提交判题（提交至洛谷在线评测平台，获得黑盒反馈）。'
    '（2）本地预检门：提交前强制编译并通过全部题面样例，防止浪费提交额度。'
    '（3）提交配额管理：每题最多5次真实提交，跨运行累计，样例未过的退回同样消耗额度。')
add_body(doc, '被试模型为DeepSeek-V4.1-Flash（思考档位low），输出上限256K tokens。'
    '评测平台为洛谷（luogu.com.cn），所有题目均为C++语言（C++17标准）。')

add_h2(doc, '1.3  题库设计')
add_body(doc, '实验题库共46题，涵盖难度6~8三个档位。选题标准为：（1）非模板题；'
    '（2）题面不超过2000字；（3）优先选取2024年后上传的冷门题目以降低训练数据污染风险。'
    '实验过程中发现，相当比例的题目（尤其经典赛题）模型能首提交直接通过，'
    '这些实例对研究"修复能力"无增量信息，故在数据分析中予以剔除。'
    '最终参与A/B分类分析的有效实例为20题26轮。')

# ============================================================
# 第2章
# ============================================================
add_h1(doc, '2  核心发现：错误深度决定修复能力')

add_h2(doc, '2.1  A/B分类标准')
add_body(doc, '为消除"修好了叫浅伤、修不好叫深伤"的循环论证风险，我们采用独立于结果的'
    '代码diff判定法：对每次提交的首版与最终版代码进行函数名集合和算法关键词的比对，'
    '若算法框架保留率≥50%且修复仅涉及边界条件、数值精度、输出格式等层面，判定为浅伤（A类）；'
    '若模型在首版算法框架内大量修改（行变更率最高达92%）但始终无法达到满分，判定为深伤（B类）。')

add_h2(doc, '2.2  核心数据')
add_body(doc, '图2-1展示了A/B两类的修复率对比：A类（浅伤）9题全部修复成功（100%），'
    'B类（深伤）11题全部修复失败（0%），中间无过渡带。')
add_figure(doc, f'{FIGURES}/fig1_ab_rate.png', 12, '图2-1  A/B类修复率对比')
add_body(doc, '表2-1列出了全部20个有效实例的详细信息。')
add_table(doc,
    ['编号', '题目', '提交', '首分', '轨迹', '结局'],
    [
        ['A1','P17451 文回文','2/5','0%','0→100%','修复'],
        ['A2','P17238 Petal Dance','2/5','92%','92→100%','修复'],
        ['A3','P15840 材料切割','2/5','55%','55→100%','修复'],
        ['A4','P17465 Bell级数','4/5','0%','0→60→90→100%','修复'],
        ['A5','P17344 AROD','3/5','CE','CE→CE→100%','修复'],
        ['A6','P17304 XOR&LCA','2/5','CE','CE→100%','修复'],
        ['A7','P17432 彼方的她','2/5','98%','98→100%','修复'],
        ['A8','P7982 琴琴的树','2/5','20%','20→100%','修复'],
        ['A9','P7951 右方之火','5/5','退回','0→37→91→98→100%','修复'],
        ['B1','P17254 线性筛2','5/5','62%','62→62→62→62→62%','平台'],
        ['B2','P16926 Deadlocked','5/5','28%','25→28→40→40%','平台'],
        ['B3','P5612 Ynoi','5/5','30%','30→35→45→55→55%','爬坡'],
        ['B4','P5655 基础数论','5/5','60%','60→60→60→60→60%','死平台'],
        ['B5','P5611 Ynoi D2T2','5/5','0%','0→22→4→22→33%','锯齿'],
        ['B6','P5649 Sone1','3/5','0%','0→0→0%','全零'],
        ['B7','P7999 翻转序列','2/5','退回','0→63%','弃疗'],
        ['B8','P7922 Pyramid','3/5','26%','26→26→26%','死平台'],
        ['B9','P7952 天动万象','3/5','64%','64→77→77%','爬坡'],
        ['B10','P7979 世界未末日','4/5','0%','0→0→0→超时','全零'],
        ['B11','P7983 practiceZ','5/5','64%','64→64→71→71→71%','爬坡'],
    ],
    '表2-1  全部20个有效实例的修复轨迹')

add_h2(doc, '2.3  修复轨迹分析')
add_body(doc, '图2-2直观展示了两类实例的修复轨迹差异。左图为A类（浅伤）：修复轨迹呈阶梯状，'
    '每轮提交稳步上升直至满分，平均1.78轮完成修复。右图为B类（深伤）：轨迹在某一分数区间'
    '形成平台后停滞，无论投入多少次提交和多少token都无法突破。')
add_figure(doc, f'{FIGURES}/fig2_trajectories.png', 16, '图2-2  A类阶梯登顶（左）与B类平台滞留（右）')

add_h2(doc, '2.4  独立验证：框架锁定效应')
add_body(doc, '代码diff分析揭示了一个关键现象：B类实例中模型的行变更率并不低（最高达92%），'
    '说明模型确实在大量修改代码。然而，算法框架（核心函数名和算法关键词）从未变过。'
    '例如P5655的模型在5次提交中陆续加入了Pollard Rho、快速乘等数论工具，'
    '但始终未跳出首版"暴力+优化"的框架；P5649的模型写了15个函数实现LCT，'
    '三版代码始终无法通过任何一个测试点。')
add_body(doc, '这证明深伤的本质是"框架锁定"：模型知道首版算法框架"大概对"'
    '（因此不愿彻底重写），但该框架存在无法通过局部修补解决的算法层缺陷。')

# ============================================================
# 第3章
# ============================================================
add_h1(doc, '3  深入分析：边界条件与失败模式')

add_h2(doc, '3.1  首提交得分与结局的关系')
add_body(doc, '图3-1展示了首提交得分区间与修复结局的交叉分析。一个意外发现是：'
    '首分60~89%的"中间带"6例中5例归入B类（修复失败），比例远高于其他区间。')
add_figure(doc, f'{FIGURES}/fig3_firstscore_heatmap.png', 12, '图3-1  首提交得分与结局热力图')
add_body(doc, '解释：中间分意味着"算法大致对但有关键缺陷"。模型此时最容易产生"再修一下就好了"'
    '的错觉，在原框架内反复调整（恰好消耗全部配额），而非跳出框架重写。'
    '相比之下，高分段（≥90%）的模型只需修一两个边界条件即可满分（归入A类），'
    '低分段（<20%）的模型有时会果断放弃首版思路重新设计（也可能成功归入A类），'
    '唯独中间带陷入了"半对半错"的修补陷阱。')

add_h2(doc, '3.2  失败模式分类学')
add_body(doc, '除A/B分类外，实验过程中观察到多种决策层异常行为，归纳为表3-1所示的五类。')
add_table(doc,
    ['失败模式', '典型实例', '行为描述', '消耗tokens'],
    [
        ['推理发散','P15860','三档预算全部烧穿，交付恒为零','200K-390K'],
        ['过度验证','P16531','60轮自测全绿却从不提交','3,131K'],
        ['过早弃疗','P5642','获得72%部分分后立即放弃','365K'],
        ['自信矛盾','P7999','本地全过但系统退回后写总结而非重试','989K'],
        ['框架锁定','B类全部','在首版框架内大量修改但分数停滞','550K-1,780K'],
    ],
    '表3-1  失败模式分类学')
add_body(doc, '值得注意的是"过度验证"与"过早弃疗"构成一对镜像：前者不敢提交（60轮自测后仍不动作），'
    '后者不敢继续（一次部分分后立即放弃）。两者均属于决策层面的失败，与算法能力无关。')

add_h2(doc, '3.3  思考档位旋钮')
add_body(doc, '我们发现调整模型的思考深度档位（reasoning_effort）可以可控地将"深思一次过"'
    '转化为"浅思迭代修复"。同一题P17465在high档位下思考139K tokens后首提交满分；'
    '在low档位下首提交仅0分，随后通过黑盒反馈逐轮爬升（0→60→90→100），'
    '4轮恰好用完配额登顶。这一发现提供了多轮调试样本的按需生产机制，'
    '也说明"深思"与"迭代"是同一能力的两种表达形式。')

add_h2(doc, '3.4  成本断崖')
add_body(doc, '图3-2展示了不同结局类别的token消耗对比。修复失败（B类）的平均成本约为'
    '修复成功（A类）的1.6倍、秒杀（首提交通过）的16倍。')
add_figure(doc, f'{FIGURES}/fig4_cost.png', 12, '图3-2  各结局类别的token消耗对比')
add_body(doc, '这一"成本断崖"现象具有实际意义：在实际应用中，无法提前判断一个错误是浅伤还是深伤，'
    '但B类的token消耗约为A类的1.6倍。若配额有限，应设置合理的止损策略。')

# ============================================================
# 第4章
# ============================================================
add_h1(doc, '4  实验总结')

add_h2(doc, '4.1  遇到问题及解决方法')
add_body(doc, '（1）LLM推理超时。早期实验中模型对难题的思考时间经常超过API连接超时限制，'
    '导致请求失败。解决方法：改用流式请求（streaming），思考token持续流动使连接保持活跃；'
    '同时将默认输出上限从64K提升至256K（官方上限384K），使深思型问题得以在预算内完成。')
add_body(doc, '（2）洛谷平台限流。高频自动化提交触发了平台的反爬机制，验证码升级为'
    '难以通过的形态。解决方法：实现跨进程提交锁（同一时刻仅允许一路提交流程）、'
    '账号级最短提交间隔（60秒）、以及浏览器配置持久化以积累信任度。')
add_body(doc, '（3）历史上下文丢失。长对话中模型的首版代码被截断，导致后续修复'
    '"失忆盲改"。解决方法：引入固定位置的"提交历史"消息，每轮提交后更新，'
    '确保模型随时能看到全部历史代码版本。对照实验显示此改进使修复率从0%提升至100%。')

add_h2(doc, '4.2  感想')
add_body(doc, '本实验最深刻的发现是：AI在算法设计中的能力边界不在"题目类型"'
    '（几何、数论、博弈等类型假设均被反例击破），而在"错误深度"。'
    '浅伤（实现层错误）可以修，深伤（算法层错误）修不了——而且模型自己不知道'
    '错在哪一层，所以会无止境地修补一个方向错误的框架。')
add_body(doc, '另一个感悟是"决策层"问题的普遍性。模型不仅会"做不出来"，还会'
    '"不敢提交"（过度验证）、"不敢继续"（过早弃疗）、"不认账"（自信与系统判定矛盾时'
    '选择相信自己）。这些行为与人类的心理状态惊人地相似，提示大模型可能已经内化了'
    '某种"自我评估"机制，尽管这个机制远不可靠。')

add_h2(doc, '4.3  意见或建议')
add_body(doc, '（1）对于将LLM用于算法设计的从业者：建议设置"止损线"——当连续2~3次提交'
    '分数无显著提升时，应果断放弃当前框架重写，而非继续修补。')
add_body(doc, '（2）对于课程设计：本实验的自动化框架（题库管理、LLM代理、洛谷判题、'
    '轨迹记录）可以作为后续学生的研究工具，扩展方向包括多模型对比、交互式反馈'
    '（提供失败测试数据内容）以及更精细的错误深度量化。')

# ============================================================
# 参考文献
# ============================================================
add_h1(doc, '参考文献')
refs = [
    '[1] Chen X, Lin X, Wang D, et al. Teaching Large Language Models to Self-Debug. arXiv:2304.05128, 2023.',
    '[2] Romera-Paredes B, Barekatain M, Novikov A, et al. Mathematical discoveries from program search with large language models. Nature, 2024, 625: 468-475.',
    '[3] Sui Y, Zhou Y, et al. Stop Overthinking: A Survey on Efficient Reasoning for Large Language Models. arXiv:2503.16419, 2025.',
    '[4] El-Kishky A, et al. Competitive Programming with Large Reasoning Models. arXiv:2502.06807, 2025.',
    '[5] Mankowitz D J, et al. Faster sorting algorithms discovered using deep reinforcement learning. Nature, 2023, 618: 257-263.',
    '[6] ProBench: Benchmarking Large Language Models in Competitive Programming. arXiv:2502.20868, 2025.',
]
for r in refs:
    add_ref(doc, r)

# ============================================================
doc.save(OUTPUT)
print(f'论文已生成: {OUTPUT}')
