# [ICPC 2018 Xuzhou R] Rikka with Data Structures

As we know, Rikka is poor at data structures. Yuta is worrying about this situation, so he gives Rikka some tasks about data structures to practice. Here is one of them:

Yuta has an array $A$ with $n$ numbers, denoted by $A[1], A[2], \cdots, A[n]$. Then he makes $m$ operations on it. There are three types of operations:

* $\text{1 l r k}$: for each index $i$ in $[l, r]$, change the value of $A[i]$ into $(A[i] + k)$;
* $\text{2 l r k}$: for each index $i$ in $[l, r]$, change the value of $A[i]$ into $k$;
* $\text{3 l r x}$: Yuta wants Rikka to count the number of different indices $y$ with $l \le y \le r$ such that $\max \lbrace A[\min \lbrace x, y \rbrace ], A[\min \lbrace x, y \rbrace +1], \cdots, A[\max \lbrace x, y \rbrace ] \rbrace = \max \lbrace A[x], A[y] \rbrace$.

It is too difficult for Rikka. Can you help her?

The input contains several test cases, and the first line contains a single integer $T$ ($1 \le T \le 200$), the number of test cases.

For each test case, the first line contains two integers $n$ ($1 \le n \le 10^5$) and $m$ ($1 \le m \le 10^5$).

The second line contains $n$ integers $A[1], A[2], \cdots, A[n]$ ($1 \le A[i] \le 10^9$).

Then $m$ lines follow, each line of which describes an operation, containing four integers as mentioned above, satisfying $1 \le l \le r \le n$, $1 \le k \le 10^9$ and $1 \le x \le n$.

The input guarantees that there are at most $10$ test cases with $n > 10^3$ or $m > 10^3$.

For each query, an operation of type $3$, output a single line with a single integer, the answer to this query. 