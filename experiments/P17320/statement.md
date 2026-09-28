# [ICPC 2018 Nanjing R] Cherry and Chocolate

Cherry and Chocolate play a game on a tree. First, Cherry picks a node and paints it pink. Then, Chocolate picks another node and paints it brown. Afterwards, Cherry picks yet another node and paints it pink. The game ends here. Chocolate doesn't get the second move.

For each node $v$, if there is no path from $v$ to the brown node without passing through a pink node, Cherry gets a point.

Cherry wants to maximize her score, and Chocolate wants to minimize it. If both players play optimally, what will Cherry's score be?

The first line contains an integer, $n$ ($3 \le n \le 10^5$), the number of nodes on the tree.

Each of the next $n - 1$ lines contains two integers $a_i$ and $b_i$,($1 \le a_i, b_i \le n$), meaning there is an edge between node $a_i$ and node $b_i$.

A single integer, Cherry's score if both players play optimally. 