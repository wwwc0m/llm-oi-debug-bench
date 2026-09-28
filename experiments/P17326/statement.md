# [ICPC 2018 Nanjing R] Magic Potion

There are $n$ heroes and $m$ monsters living in an island. The monsters became very vicious these days, so the heroes decided to diminish the monsters in the island. However, the $i$-th hero can only kill one monster belonging to the set $M_i$. Joe, the strategist, has $k$ bottles of magic potion, each of which can buff one hero's power and let him be able to kill one more monster. Since the potion is very powerful, a hero can only take at most one bottle of potion.

Please help Joe find out the maximum number of monsters that can be killed by the heroes if he uses the optimal strategy.

The first line contains three integers $n, m, k$ ($1 \le n, m, k \le 500$) $\text{---}$ the number of heroes, the number of monsters and the number of bottles of potion.

Each of the next $n$ lines contains one integer $t_i$, the size of $M_i$, and the following $t_{i}$ integers $M_{i, j}$ ($1 \le j \le t_i$), the indices ($1$-based) of monsters that can be killed by the $i$-th hero ($1 \le t_i\le m, 1\leq M_{i, j} \le m$).

Print the maximum number of monsters that can be killed by the heroes. 