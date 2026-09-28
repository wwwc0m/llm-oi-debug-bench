# Bell 级数练习题

::::info[概念解释]{open}
对正整数 $N$，定义集合
$$
\mathcal D(N)=\left\{\left\lfloor\frac Nd\right\rfloor:1\le d\le N\right\}
$$
将 $\mathcal D(N)$ 中的数从小至大依次排列为 $1=d_1<d_2<\cdots<d_K=N$。

设 $F(n)=\sum_{i=1}^n f(i)$ 为 $f(n)$ 的前缀和函数。本题中，称 $f$ 的**块筛**为以下向量：
$$
\mathbf B_f(N)=\left(F(d_1),F(d_2),\ldots,F(d_K)\right)
$$
::::

你需要**动态维护**一个积性函数 $f(x)$ 的块筛。

具体而言，初始给定范围 $N$ 以及 $f$ 在不超过 $N$ 的所有素数幂处的取值。接下来会进行 $Q$ 次操作，每次操作为以下两者之一：

1. 给定素数 $p$，并修改 $f(p),f(p^2),\ldots,f(p^{\lfloor\log_p N\rfloor})$ 的点值；
2. 求出 $f$ 的块筛。

为了避免输出过多，要求“求出 $f$ 的块筛”时，你只需要计算以下函数：
$$
S_f(N)=\bigoplus_{x\in\mathcal D(N)} x\left(F(x)\bmod 998244353\right)
$$

第一行两个正整数 $N,Q$。

接下来 $\pi(N)$ 行，第 $i$ 行 $\lfloor\log_{p_i}N\rfloor$ 个整数，第 $j$ 个数为 $f(p_i^j)$。$p_i$ 表示从小到大的第 $i$ 个素数。

接下来 $Q$ 行，每行给出若干个整数。第一个整数 $\textit{op}$ 表示该次操作的种类：
- 若 $\textit{op}=1$，下一个整数表示该次修改的素数 $p$，后面 $\lfloor\log_pN\rfloor$ 个整数依次为新的 $f(p),f(p^2),\ldots,f(p^{\lfloor\log_pN\rfloor})$；
- 若 $\textit{op}=2$，你需要输出此时 $S_f(N)$ 的值。

若干行，每行一个非负整数，表示询问时 $S_f(N)$ 的值。 

对于 $20\%$ 的数据，$NQ \le 10^7$；  
对于另外的 $30\%$ 数据，保证所有 $\textit{op}=1$ 操作的 $p$ 相同；  
对于 $100\%$ 的数据，$2\le N\le 10^7$，$1\le Q\le 3\times10^4$，$\textit{op}\in\{1,2\}$，$2\le p\le N$ 且为素数，给出的任意 $f(p^k)$ 满足 $0\le f(p^k)<998244353$。

题目时限约为 std 在最慢点用时的 1.5 倍。