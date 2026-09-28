I'll analyze this problem. Let me start by understanding what's being asked.

The problem gives a number `n` (here 6) and outputs a number (4) and a pattern of `#` and `.`. This is the POI "Projekt planszy" problem. Let me think about the actual problem statement.

This is POI 2020/2021 Round 2 "Projekt planszy" (Board Design). Let me recall: Given `n`, we need to construct an `n×n` board with `#` (black) and `.` (white) cells such that... and output some count.

Let me search my memory. The problem: We want to paint an n×n grid. A "path" is a sequence of cells... Actually let me think about the sample.

n=6, output 4, then grid:
```
...#
....
##..
###.
```

Let me reconsider. I don't have the exact statement. Let me think about what's known about this problem.

Actually, I should reason from the sample. Output first line is 4, then a 6x6 grid.

Hmm, let me think about POI plansza problems. This might be: find the maximum k such that there's a board with a "good" property, and the count 4 is the minimum number of something.

Let me think differently — perhaps I should just try to find the actual problem. The title "Projekt planszy" = "Board Design/Project". 

Let me recall: I believe this problem asks: design an n×n board of black/white cells such that every 2×2 sub-square has exactly an odd number of black cells? Or such that there's no monochromatic 2x2? 

Let me test with the sample grid:
```
Row0: . . . #
Row1: . . . .
Row2: # # . .
Row3: # # # .
```
Wait n=6 but only 4 rows shown? Let me recount the sample:

```
4
...#
....
##..
###.
```
That's 4 rows of 4 characters each. And n=6. Hmm, so maybe the board is 4×4 and n=6 is something else? Or the output is a 4×4 grid.

Wait, the sample input is 6, output is 4 then a 4x4 grid. So maybe the answer is a board of size... Hmm.

Actually maybe output is: first line = k (the answer, 4), then k rows of k characters? So k=4, board 4×4.

Let me reconsider: maybe the problem is: find largest k such that you can place a k×k board pattern... no.

Let me think about the actual POI problem. Let me recall POI 2020/21 R2 problems: "Koleje" (Railways), "Piksel" (?), "Projekt planszy", "Zapiekanki"... 

Hmm. Let me think about "Projekt planszy" more carefully. I recall it's about designing a board where each row and column... 

Actually, I found in memory: The problem "Projekt planszy" (Board design): You're given n. You must output a board of size n×n? But sample shows 4x4 for n=6.

Wait, maybe the first number 4 is not k but something else, and the grid is 4×4. Let me re-read: input 6, output 4 then 4 lines. Perhaps the problem: find maximum m such that an m×m board exists with property P, output m and the board.

Let me hypothesize the property and check. Grid (4×4):
```
. . . #
. . . .
# # . .
# # # .
```
Count of #: row0 has 1, row1 has 0, row2 has 2, row3 has 3. Total 6.

Hmm interesting. Let me think of "no four corners of a rectangle are all the same color"? Or "each 2x2 has ..."?

Check 2x2 blocks of the 4x4:
- rows0-1, cols0-1: . . / . . → all white
- rows0-1, cols1-2: . . / . . → all white
- rows0-1, cols2-3: . # / . . → 1 black
- rows1-2, cols0-1: . . / # # → 2 black
- rows1-2, cols1-2: . . / # . → 1 black
- rows1-2, cols2-3: . . / . . → 0 black
- rows2-3, cols0-1: # # / # # → 4 black
- rows2-3, cols1-2: # . / # # → 3 black
- rows2-3, cols2-3: . . / # . → 1 black

Not obvious.

Let me reconsider the problem. Maybe this is the problem where you need to find the minimum number of black cells such that every cell... 

Alternatively, maybe the output format: first line is the number of black cells? No, 4 then grid.

Hmm, let me reconsider. Perhaps the input n is the number of black cells or the size, and output first line is the size of the board.

Actually, I think I should look at this from the perspective of known editorial. Let me recall: POI 2020/2021, problem "Projekt planszy". 

I believe the problem is: For a given n, find the maximum k such that there exists a k×k binary matrix where in every row and every column, ... no.

Let me try another angle: maybe it's about a "snake" path or connectivity.

Given the ambiguity, let me search memory harder. "Projekt planszy" — I think it's about designing a board such that the number of "connected components" or something is maximized/minimized.

Actually! I recall now. I think this is the problem: You have an n×n board. You color cells black/white. A "path" goes... The task: find the maximum number of black cells in an n×n board such that no 2×2 block is entirely black? And output the board? Hmm.

Let me reconsider the sample. n=6, output 4, board 4×4. If the board size is the answer k, then maybe the problem is: given n, find max k such that a k×k board exists with property, print k and the board. For n=6, k=4.

Hmm what property would cap at 4 for n=6?

Alternatively, maybe the input is the size and output is the minimum number of black cells, and then the board.

Let me try to find the problem online in memory: "Projekt planszy" is POI 28 (2020/21) stage 2, problem "Pla"? The problems of stage 2: "Koleje", "Piksele"? Hmm.

Actually the POI 2020/21 R2 (II etap) problems were: "Koleje" (railways), "Piksele"? no... Let me recall: II etap 2021: "Zapiekanki", "Koleje", "Impreza", "Projekt planszy"? Not sure.

OK given the difficulty, let me think about the structure. Perhaps I can deduce from the sample grid pattern.

Grid:
```
row0: 0 0 0 1
row1: 0 0 0 0
row2: 1 1 0 0
row3: 1 1 1 0
```
Hmm, this looks like a "staircase" pattern. Rows have counts 1,0,2,3.

Wait, maybe the problem: place n=6 "dominoes" or something on a board, output the board size.

Let me reconsider: maybe the output grid is n×n but there was a formatting issue and it's actually 6×6? Let me recount the sample output lines:
```
4
...#
....
##..
###.
```
4 lines after the "4". Each line 4 chars. So 4×4.

Hmm, unless the sample is truncated. But it's given as the full sample.

Let me think about this problem as: "Find the smallest square board such that you can place n=6 non-attacking ... " no.

New idea: Maybe the problem is about a "tour" — the board represents a graph where # are walls. n is the number of ... and we output the board.

Actually, let me reconsider. The first output 4 might be the answer to "minimum number of black cells needed" and the grid is n×n = 6×6 but the sample only shows part? No, the sample should be complete.

Hold on, maybe input 6 means n=6, and the answer is the largest k such that a k×k "good board" exists, and the board printed is k×k. For n=6, k=4. That's plausible: answer depends on n, and for n=6 answer is 4.

What's the relationship? If n=6 → k=4. Maybe k = floor(2n/3)? 2*6/3=4. Or k = n - 2 = 4. Or k = floor((n+2)/2)=4. Or k = ceil(2n/3)=4.

Hmm.

Let me think about the property from the grid. The grid:
```
...#
....
##..
###.
```
Maybe the property is: it's a valid "maze" or the black cells form a specific shape.

Let me count black cells: 1+0+2+3 = 6 = n! Interesting, the number of black cells equals n=6.

So maybe: place exactly n black cells on a k×k board such that some condition holds, maximize k. For n=6, max k=4, and the board has 6 black cells.

What condition? Perhaps: no two black cells are adjacent (including diagonally)? Let's check: black cells at (0,3),(2,0),(2,1),(3,0),(3,1),(3,2). (2,0) and (3,0) are vertically adjacent — so not that.

Perhaps: no 2×2 block has all black or all white? Check rows2-3 cols0-1: all black → violates.

Perhaps: every black cell is connected to form a path? The black cells: (2,0)-(2,1)-(3,0)-(3,1)-(3,2) form a connected blob plus (0,3) isolated. Hmm.

Maybe: the black cells form a "staircase" representing partitions.

Let me think about "Projekt planszy" differently. Maybe it's: We want to design a board such that reading rows as binary numbers gives a specific sequence, or the board is a "Young tableau".

Alternatively, maybe the problem is the classic: "Find maximum number of black cells in n×n grid with no 2×2 all-black" but here output is a board.

I'm spending too long guessing. Let me try to recall the actual POI problem statement. 

I'm now fairly confident: POI 2020/21 II etap problem "Projekt planszy" (Board design). Statement (from memory): Bajtek wants to design a board of size n×n... Actually I think the problem involves "kolorowanie" and the answer is about the maximum k such that there's a k×k board where each row and column has a distinct number of black cells? 

Let's test: rows counts 1,0,2,3 (distinct!). Columns: col0: rows 2,3 → 2; col1: rows2,3 → 2. Not distinct. So no.

Maybe each row and column has a distinct count? Col0=2, col1=2 → same, so no.

Hmm. Let me reconsider: maybe the property is that the board is symmetric or something.

Let me try: "no three black cells in a line"? 

Let me try to think about what makes 4 special for n=6.

Alternative: maybe it's a graph problem. n=6 is the number of vertices, output the board representing an adjacency... no, board is 4×4.

Let me try yet another interpretation: The input n=6, output first line 4 = answer. Then the 4×4 grid is an example construction. The problem might be: "Given n, find the minimum number of black cells in an n×n board such that every 2×2 sub-board contains at least one black cell" — that's a covering problem. But then output would be n×n board, not 4×4.

Wait wait. Maybe I misread and the grid is 4 rows but each row is 6 chars? Let me look again very carefully at the problem text:

```
4
...#
....
##..
###.
```
"...#" is 4 chars, "...." 4 chars, "##.." 4 chars, "###." 4 chars. So 4×4.

Hmm OK. So output is 4 and a 4×4 grid.

Let me hypothesize: The problem asks for the largest k such that there exists a k×k binary matrix with exactly ... hmm.

Actually, maybe the problem is: given n, find the maximum k such that there's a k×k board where the number of black cells in row i plus column i ... 

Let me look at the grid as a permutation-like structure. Actually, let me consider the possibility that this is a known problem: "Projekt planszy" = construct an n×n board with no monochromatic rectangle (i.e., no 2×2 all same color). The maximum size for a 2-coloring with no monochromatic rectangle is related to bipartite Ramsey / Zarankiewicz. But that's about rectangles.

Actually the classic: The largest n for which there's a 2-coloring of n×n with no monochromatic 2×2 is n=4? No, that's not right either. Zig-zag coloring avoids monochromatic 2×2 for all n.

Hmm, what about: no monochromatic rectangle (not necessarily 2×2 adjacent, any four corners)? For 2 colors, the max n is 4? Actually the bipartite Ramsey number b(2,2)=... For a 2-coloring of K_{n,n} with no monochromatic C_4, max n is 4? Let me recall: b(2,2) = 5? The bipartite Ramsey number b(2,2) is 5, meaning any 2-coloring of K_{5,5} has a monochromatic C_4, and K_{4,4} can be colored without. So max n = 4!

That fits! n=6 input, answer 4. And the board is a 4×4 coloring with no monochromatic rectangle (four corners same color).

Let me verify the grid has no monochromatic rectangle:
```
row0: 0 0 0 1
row1: 0 0 0 0
row2: 1 1 0 0
row3: 1 1 1 0
```
Check pairs of rows for two columns where both entries are 0 (white) in both rows, or both 1 (black).

Row0=0001, Row1=0000: common 0s at cols 0,1,2 → rectangle of 0s (rows 0,1 cols 0,1) → monochromatic! So this grid has a monochromatic rectangle. So not that property (unless the property is the opposite: must contain...).

Hmm. So that's not it, or the property is different.

Let me re-examine. Maybe the property is: every 2×2 (adjacent) sub-square has exactly 2 black? Check rows0-1 cols0-1: 0,0,0,0 → 0 black. No.

Maybe: the board represents a valid solution to "no 2x2 all same" but the sample violates it. So no.

Let me reconsider. Maybe the answer 4 is the maximum number of black cells in each row? Row3 has 3. No.

Maybe 4 is the size and the board must satisfy: each row and column has at least one black cell? Row1 has none. No.

Hmm, let me reconsider the problem completely. Maybe the input n=6 is the number of columns/rows of a target and the output is a different thing.

Actually, maybe the problem is: "Given n, find the minimum k such that a k×k board can 'contain' all n×n patterns..." no.

Let me think about the first line 4 differently. Maybe the output is: the number of black cells is 4? But I counted 6. Let me recount:
```
...#
....
##..
###.
```
Row0: positions: '.','.','.','#' → 1 black.
Row1: 0 black.
Row2: '#','#','.','.' → 2 black.
Row3: '#','#','#','.' → 3 black.
Total = 6. So not 4.

Maybe the first line 4 is the number of black cells in the last row? Or the side length.

Given the sample: n=6 → k=4. Let me think k = n-2 = 4. Or k = floor(2n/3).

Actually, maybe the problem is: "Find the maximum k such that you can place k non-attacking rooks on an n×n board..." no.

Let me try to actually recall the POI problem. I'll think about POI 28 (2020/21) II stage. The problems were:
- "Koleje" (Railways)
- "Piksele"? 
- "Projekt planszy"?
- "Zapiekanki"?

Hmm, actually I recall "Projekt planszy" might be from a different contest.

Let me try a web-memory approach: Luogu P12911. The problem ID P12911. Title "棋盘 / Projekt planszy". POI 2020/2021 R2.

I think I might recall the statement now: It's about a board where you want to place black cells such that the number of "paths" ... Actually, I recall a problem about "the board is good if from any cell you can reach any other cell moving through cells of the same color"? No.

Let me think about the given construction as a hint. It looks like a "staircase" of black cells in the bottom-left and a single black at top-right.

Maybe the property is: every row and every column has a different number of black cells? Rows: 1,0,2,3 distinct. Columns: col0=2,col1=2,col2=1,col3=1 → not distinct.

Maybe the property is: the sequence of row counts is a permutation of 0..k-1? Rows: 1,0,2,3 = permutation of {0,1,2,3}. Yes! Rows counts are 1,0,2,3 which is a permutation of 0,1,2,3.

Columns: 2,2,1,1 → not a permutation. Hmm.

Maybe only rows matter? "Each row has a distinct number of black cells" → rows 1,0,2,3 distinct. But columns not required.

Hmm, or maybe the property is about rows AND the board is k×k with k rows each having distinct count from 0 to k-1. Then any k works trivially (row i has i black cells). So max k for given n? If n is total black cells = 6, and we need k rows with counts 0..k-1 summing to n: sum = k(k-1)/2 ≤ n. For n=6: k(k-1)/2 ≤ 6 → k=4 gives 6 ≤ 6. Yes! k=4 works exactly (0+1+2+3=6). k=5 gives 10 > 6. So max k=4!

That's promising! The board has row counts 1,0,2,3 (permutation of 0..3), total 6 = n. And k=4.

But wait, the input n=6 would be the number of black cells, and we need a board with k rows, each row having a distinct number of black cells, and total black cells = n. But then why is the board 4×4 (k columns)? Maybe the board must be square k×k.

Hmm, but if we just need k rows with distinct counts 0..k-1, columns don't matter, so why square? Maybe additionally each column must also have distinct counts? But the sample columns are 2,2,1,1, not distinct. So no.

Let me reconsider. Maybe the property is different: maybe it's about the number of black cells in each row being distinct AND each row is a "prefix" or something.

Actually, let me reconsider the whole thing. Perhaps the problem is: We have an n×n board (n=6). We want to choose a set of black cells. The board is "good" if ... and we output the maximum number of black cells? No, output is 4.

Hmm, let me reconsider: maybe the problem asks for the maximum k such that there's a k×k sub-board... no.

Let me try to think about the answer formula. n=6→4. Could be many formulas.

Let me look at the structure again:
```
...#
....
##..
###.
```
Rows (top to bottom) have 1,0,2,3 black cells. If we think of it as: row with i black cells has them as a left-aligned block? Row0 has 1 black but at the RIGHT (position 3). Row2 has 2 black at left. Row3 has 3 black at left. Row1 has 0.

Hmm, row0's black is at the right. Interesting.

Maybe the board represents a "Ferrers diagram" / Young diagram? Rows sorted: 3,2,1,0. If left-aligned: 
```
###
##.
#..
...
```
But the actual is different.

Let me consider that the board might encode a partition or a graph.

Alternatively, maybe the problem is about "queens" or "rooks" and the black cells are non-attacking. Let's check: black cells (0,3),(2,0),(2,1),(3,0),(3,1),(3,2). Two in same row (row2 has 2, row3 has 3) so not non-attacking rooks.

Let me step back and think about what POI problem could have this sample.

Actually, I wonder if the problem is: "Given n, find the minimum number of black cells needed so that every cell is 'attacked'..." and the answer 4 is the minimum number, with the board being 6×6. But the board shown is 4×4. Unless... the board shown is actually 6×6 and I'm miscounting due to formatting.

Let me very carefully look at the raw text:
```
样例1：
输入：
6
输出：
4
...#
....
##..
###.
```
Definitely 4 lines of 4 chars. So 4×4.

Hmm, unless the sample output is:
```
4
...#
....
##..
###.
```
where the first line "4" is part of the output and then a 4×4 grid. Yes.

OK here's another thought: maybe the problem is "Projekt planszy" where you're given n and must output ANY board of size n×n that is "good", plus the first line is the number of black cells? But n=6 and board 4×4 contradicts.

Unless the input is not n=6 as size but as number of black cells, and output is minimum board size? Then min board size for 6 black cells with property = 4, board 4×4 with 6 black cells. That matches my earlier hypothesis!

So the problem: Given n (number of black cells), find the smallest k such that there's a k×k board with exactly n black cells satisfying some property, output k and the board.

For n=6, k=4, board has 6 black cells. 

Now what property? Let's look for a property that this board satisfies and that constrains k.

Total cells k²=16, black=6, white=10.

Maybe the property: no 2×2 all-black and no 2×2 all-white? We saw rows2-3 cols0-1 all black. So no.

Maybe the property: the black cells form a connected region and white cells form a connected region? Black cells: (0,3) isolated from the blob at bottom-left. So black not connected. White cells: is (0,3) black, rest... white region includes (0,0),(0,1),(0,2),(1,*),(2,2),(2,3),(3,3) — connected. So white connected, black not.

Hmm.

Maybe property: every row and column has at least one black? Row1 has none. No.

Maybe property: it's a "good" board where a "good" board means you can pair up cells... 

Let me think about the answer 4 for n=6. If the board is k×k and must have exactly n black cells, then k² ≥ n → k ≥ 3. For n=6, k=3 (9 cells) could hold 6 black. But answer is 4, so there's a constraint forcing k≥4.

What constraint forces k=4 for n=6? Maybe each row and column has a distinct number of black cells (like a "graceful" or "Costas" property). With k=4, row counts must be a permutation of 0..3, sum=6. Column counts also a permutation of 0..3, sum=6. Total black = 6. But sample columns are 2,2,1,1 — not a permutation. So no.

Maybe only rows distinct: then k=4 with row counts 0,1,2,3 (sum 6). For k=3, row counts 0,1,2 sum=3≠6, and max distinct counts with k=3 is 0,1,2 sum 3. To have sum 6 with k=3 we'd need counts like 2,2,2 (not distinct). So if rows must be distinct, k=3 can have sum at most 3 <6, k=4 can have sum up to 6. So minimum k=4. That matches!

So property: each row has a distinct number of black cells, and total = n. Then min k is smallest k with k(k-1)/2 ≥ n. For n=6: 3*4/2=6 ≥6 → k=4. For n=5: k=4 (6≥5). Hmm but then for n=6, exact sum 6 = 0+1+2+3. Good.

But why square (k columns)? Maybe columns also need distinct counts? But sample violates. Unless the sample I'm reading is wrong.

Wait, let me re-examine the sample. Maybe the grid is:
```
...#
....
##..
###.
```
Columns: col0 = rows: '.','.','#','#' = 2 black. col1 = '.','.','#','#' = 2. col2='.','.','.','#'=1. col3='#','.','.','.'=1. So columns 2,2,1,1.

Hmm not distinct.

Alternative: maybe the property is that the board is "self-describing": the number of black cells in row i equals the number in column i? Let's check: row0=1,col0=2; no.

Maybe: the board's rows, when sorted, ... 

Let me consider: maybe the problem is about "no two rows are equal" and "no two columns are equal"? Rows: 0001,0000,1100,1110 — all distinct. Columns: 0011,0011,0001,1000 — col0=col1=0011, not distinct. So no.

Hmm. Let me reconsider whether the input might be the board size and output the max black cells. n=6 → answer 4? That seems too small (max black in 6×6 is 36).

Maybe n=6 is something else entirely.

Let me try to find the actual problem. I'll think about "Projekt planszy" POI. 

I now recall! I think this is the problem where you need to construct a board such that the black cells form a "path" from one corner to another, and n is the length... Hmm.

Actually, let me think about the name "Projekt planszy" = "Board design". Maybe the task: Bajtek designs an n×n board. A board is "interesting" if ... You output the maximum k such that ... 

Let me try to use the structure: the black cells form a shape. (0,3) is isolated. The rest form an L/staircase.

What if the property is: the black cells form a "polyomino" that can tile or that has no 2×2... 

Alternatively, maybe the board is a "maze" where black = wall, and the property is that there's a unique path, and n is the path length. The answer 4 = path length? 

Hmm, let me count something: the white cells form a region. Or maybe the answer is the number of "turns" in a path.

Let me look at the black cells as a path: (0,3) then... they're not connected.

Let me consider the white cells: (0,0),(0,1),(0,2),(1,0),(1,1),(1,2),(1,3),(2,2),(2,3),(3,3). Is this a path? (0,0)-(0,1)-(0,2)-(1,2)-(1,1)-(1,0)... it's a blob.

Hmm.

Maybe the problem is: "Find the minimum number of black cells to place so that no 2×2 is all white" (dominating set for 2×2). For n=6, minimum is... For an n×n board, min black to hit all 2×2 is floor(n/2)*floor(n/2)? For n=6, that's 3*3=9. Not 4.

Maybe "no 2×2 all black" max black = n² - floor(n/2)²? no.

Let me try to think about the answer being 4 for n=6 in a different way. Maybe the answer is the number of black cells needed, and the board is 6×6. But the sample shows 4×4... 

Unless the sample output's first line "4" is the answer and the grid is 6×6 but rendered with the first 4 rows shown? No, samples are complete.

Hold on, maybe the grid is 4×4 because the problem is: given n, output the smallest k such that a k×k board can be "projected"... 

I think I need to find the actual problem. Let me try to recall harder or deduce from constraints (n up to? time 1s, mem 64MB).

Given it's an OI problem with a construction output, likely n up to 10^5 or so, and we output a board of size O(n) or O(sqrt n). The output for n=6 is a 4×4 board (16 chars). If output size is O(n), for n=10^5 output would be huge but maybe fine.

Actually, if the board is k×k with k(k-1)/2 ≥ n, then k ~ sqrt(2n), output size k² ~ 2n = O(n). For n=10^5, output 2*10^5 chars, fine.

Hmm, but the problem might have n up to 10^9 and output the board... no, output would be too big.

Let me reconsider: maybe n is small, like n ≤ 1000.

Let me try to think about the actual known solution. I have a vague memory of a POI problem "Projekt planszy" where the answer is that you need to construct a board with specific row/column sums, and the first line is the size.

Actually, you know what, let me reconsider the possibility that this is about "magic" properties. The board:
```
...#
....
##..
###.
```
Let me compute for each cell whether it's black and look at diagonals.

Another thought: maybe the board represents the "difference" or the problem is about a "skew" board.

Let me try to see if the black cells correspond to a set of n=6 cells with no three collinear or something. Black cells: (0,3),(2,0),(2,1),(3,0),(3,1),(3,2). 

Hmm, (2,0),(2,1) same row. Not a general position set.

Let me try yet another interpretation: maybe the output is the board where '#' means a "queen" and the property is no two queens attack. But (2,0) and (3,0) same column. No.

OK let me just try to search my memory for "P12911". Luogu problem numbering P129xx is recent (2024-2025). POI 2020/21 R2.

I think I recall now: This might be the problem "Projekt planszy" where the statement is: 
"A board of size n×n is given. Some cells are black. A board is called 'good' if ... . Find the maximum k such that there exists a good board of size k×k with exactly n black cells" — hmm.

Actually, let me reconsider. Let me think about the phrase "Projekt planszy". Maybe it's: design a board such that the number of black cells in each row, read as a sequence, is a "permutation" or "graceful labeling".

Let me try to think about the condition that gives k=4 for n=6 and see if the board satisfies it.

Condition candidate: "The number of black cells in each row and each column is distinct." Sample fails columns.

Condition candidate: "The board has exactly one black cell in each row and column" (permutation matrix) — sample fails.

Condition candidate: "The number of black cells in row i is exactly i" — sample rows 1,0,2,3 not 0,1,2,3 in order. But as a set yes.

Condition candidate: "The board is a 'staircase' where row i has black cells forming a prefix of length a_i with a_i distinct" — sample row0's black is at position 3, not prefix.

Hmm, row0 has black at (0,3) which is the rightmost. Maybe the board is symmetric under transpose+reverse or something.

Let me transpose the board:
```
Original:
0 0 0 1
0 0 0 0
1 1 0 0
1 1 1 0

Transpose:
0 0 1 1
0 0 1 1
0 0 0 1
1 0 0 0
```
Not equal to original.

Reverse rows:
```
1 1 1 0
1 1 0 0
0 0 0 0
0 0 0 1
```
Not equal.

Hmm.

Let me try to think about the problem from the "editorial" angle. POI problems often have a clean combinatorial answer. The sample n=6 → 4. Let me guess the answer function and see if it's a known sequence.

If answer = floor(sqrt(2n)): n=6 → floor(3.46)=3. No.
If answer = ceil((1+sqrt(1+8n))/2): for n=6, (1+sqrt(49))/2 = 4. Yes! That's the inverse of k(k-1)/2. So k is the smallest with k(k-1)/2 ≥ n. For n=6, k=4.

Hmm, but if the answer were just about triangular numbers, the problem would be trivial (just output k and a board). Maybe there's more.

Actually, maybe the problem is: given n, find the maximum number of black cells in a board such that ... no.

Let me reconsider the sample output. What if the first line 4 is not the answer but part of the board? Like the board is 5×4? No.

What if the output format is: first line = k, then k lines each of length k. And the problem: given n, find the smallest k such that there's a k×k board with exactly n black cells and no two black cells in the same row or column (i.e., a partial permutation)? Then max black in k×k is k, so need k ≥ n. For n=6, k=6, but answer is 4. No.

What if the property is "no two black cells adjacent (edge-adjacent)"? Max independent set in k×k grid is ceil(k²/2). For n=6, need k²/2 ≥ 6 → k≥4 (16/2=8≥6). And k=3: 9/2→5 <6. So k=4! And we need a board with 6 non-adjacent black cells in 4×4. But sample has