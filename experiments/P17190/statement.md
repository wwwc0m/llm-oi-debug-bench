# [ICPC 2017 Hong Kong R] Triangle

Bob draws $N$ black points and $N$ white points in the plane, and draws directed edges between each pair of them. For each pair of points $p$ and $q$, Bob can draw an edge according to the following rules.

*   If these two points share the same color, put an edge $p \to q$ or $q \to p$ optionally.
*   If they have different colors, suppose that $p$ is white and $q$ is black, then draw the edge $p \to q$ if $\text{dist}(p, q) > D$, or $q \to p$ if not.

The distance function is defined as $\text{dist}(p, q) = |p.x - q.x| + |p.y - q.y|$.

Bob thinks that a beautiful triangle of points $p$, $q$ and $r$ (which is a triple of three points) should satisfy the following conditions:

*   At least one of them is black, and at least one of them is white.
*   $p \to q$, $q \to r$, and $r \to p$ are all the edges between them.

Now Bob wants to know the minimum number and the maximum number of beautiful triangles could exist.

The input contains several test cases, please handle it to the end of file.

For each test case, the first line contains two integers $N$ ($N \le 100000$) and $D$. In the next $N$ lines, each line contains two integers indicating the coordinate of a white point. In the next $N$ lines, each line contains two integers indicating the coordinate of a black point. Some points may share the same coordinate.

All numbers in input are non-negative integers and less than $2^{31}$.

For each test case, output two integers indicating the minimum number and the maximum number of beautiful triangles. 