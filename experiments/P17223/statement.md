# [ICPC 2017 Nanning R] The Maximum Unreachable Node Set

In this problem, we would like to talk about unreachable sets of a directed acyclic graph $G = (V,E)$. In mathematics a directed acyclic graph $(DAG)$ is a directed graph with no directed cycles. That is a graph such that there is no way to start at any node and follow a consistently-directed sequence of edges in $E$ that eventually loops back to the beginning again.

A node set denoted by $V_{UR} \subset V$ containing several nodes is known as an unreachable node set of $G$ if, for each two different nodes $u$ and $v$ in $V_{UR}$, there is no way to start at $u$ and follow a consistently-directed sequence of edges in $E$ that finally archives the node $v$. You are asked in this problem to calculate the size of the maximum unreachable node set of a given graph $G$.

The input contains several test cases and the first line contains an integer $T (1 \le T \le 500)$ which is the number of test cases.

For each case, the first line contains two integers $n (1 \le n \le 100)$ and $m (0 \le m \le n(n - 1)/2)$ indicating the number of nodes and the number of edges in the graph $G$. Each of the following $m$ lines describes a directed edge with two integers $u$ and $v (1 \le u, v \le n$ and $u \neq v)$ indicating an edge from the $u$-th node to the $v$-th node. All edges provided in this case are distinct.

We guarantee that all directed graphs given in input are DAGs and the sum of $m$ in input is smaller than $500000$.

For each test case, output an integer in a line which is the size of the maximum unreachable node set of $G$. 