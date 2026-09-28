# [ICPC 2018 Nanjing R] Mediocre String Problem

Given two strings $s$ and $t$, count the number of tuples $(i, j, k)$ such that

1. $1 \le i \le j \le |s|$
2. $1 \le k \le |t|$.
3. $j - i + 1 > k$.
4. The $i$-th character of $s$ to the $j$-th character of $s$, concatenated with the first character of $t$ to the $k$-th character of $t$, is a palindrome.

A palindrome is a string which reads the same backward as forward, such as "$\texttt{abcba}$" or "$\texttt{xyzzyx}$".

The first line is the string $s$ ($2 \le |s| \le 10^6$).

The second line is the string $t$ ($1 \le |t| < |s|$). 

Both $s$ and $t$ contain only lower case Latin letters.

The number of such tuples. 