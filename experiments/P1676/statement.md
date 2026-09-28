# 「CMOI R0」Parallel Universe Shifter / Lattices in Circle

原《[USACO05FEB] Aggressive Cows G》参见 [P1824](https://www.luogu.com.cn/problem/P1824)。

$$\text{Answer}=\pi n^2+\mathrm O(n^{\frac{517}{824}}).$$

![](bilibili:BV1RM4m1U7oU)

$\small\color{white}/35^{\text{th}}\text{Problem by AtC}.$

求与原点距离不超过 $n(1\leq n\leq 10^{12})$ 的整点个数。

一行一个正整数 $n$。

一行一个正整数，即答案。注意它可能大于 $2^{64}$。

### 样例 $1$ 解释

符合条件的 $5$ 个点是 $(0,0),(1,0),(0,1),(0,-1),(-1,0)$。

### 数据范围

|$\text{Subtask}$|$\text{Special Constraints}$|$\text{Time Limit}$|$\text{Points}$|
| :----------: | :----------: | :----------: | :----------: |
|$1$|$1\leq n\leq 2\times 10^3$|$0.25\text s$|$1$|
|$2$|$10^4\leq n\leq 10^7$|$1\text s$|$4$|
|$3$|$10^8\leq n\leq 10^9$|$1\text s$|$10$|
|$4$|$10^9\leq n\leq 10^{10}$|$3\text s$|$15$|
|$5$|$10^{10}\leq n\leq 10^{11}$|$4\text s$|$30$|
|$6$|$10^{11}\leq n\leq 10^{12}$|$4\text s$|$40$|