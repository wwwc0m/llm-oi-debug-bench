# [ICPC 2026 Xi'an I] XOR and LCA

Yuki has a tree with $2^n$ nodes, labeled from $0$ to $2^n - 1$. The $i$-th edge connects node $u_i$ and node $v_i$.

Let $\operatorname{lca}_{r}(u, v)$ denote the lowest common ancestor of nodes $u$ and $v$ when the tree is rooted at node $r$. You need to help Yuki calculate:

$$
\bigoplus_{0 \le u < v < 2^n} \operatorname{lca}_{u \oplus v}(u, v)
$$

where $\oplus$ denotes the bitwise XOR operation.

This problem contains multiple test cases.

The first line contains a positive integer $t$ $(1 \le t \le 10^4)$, representing the number of test cases.

For each test case:

- The first line contains a positive integer $n$ $(1 \le n \le 21)$.
- The next $2^n - 1$ lines each contain two integers $u_i, v_i$ $(0 \le u_i, v_i < 2^n,\ u_i \ne v_i)$.

It is guaranteed that the input forms a tree, and the sum of $2^n$ over all test cases does not exceed $2^{21}$.

For each test case, output a single line containing an integer representing the answer.

For the first test case:

- When the tree is rooted at node $1$, the lowest common ancestor of nodes $0$ and $1$ is node $1$, so the answer is $\operatorname{lca}_1(0, 1) = 1$.

For the second test case:

- We calculate $\operatorname{lca}_{u \oplus v}(u, v)$ for all pairs $(u, v)$:
    - $(0, 1)$: $0 \oplus 1 = 1$, $\operatorname{lca}_1(0, 1) = 1$.
    - $(0, 2)$: $0 \oplus 2 = 2$, $\operatorname{lca}_2(0, 2) = 2$.
    - $(0, 3)$: $0 \oplus 3 = 3$, $\operatorname{lca}_3(0, 3) = 3$.
    - $(1, 2)$: $1 \oplus 2 = 3$, $\operatorname{lca}_3(1, 2) = 2$.
    - $(1, 3)$: $1 \oplus 3 = 2$, $\operatorname{lca}_2(1, 3) = 2$.
    - $(2, 3)$: $2 \oplus 3 = 1$, $\operatorname{lca}_1(2, 3) = 2$.
- The XOR sum is $1 \oplus 2 \oplus 3 \oplus 2 \oplus 2 \oplus 2 = 2$.
