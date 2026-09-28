# [ICPC 2017 Urumqi R] Friends

Some people benefit from large and diverse networks of friends, while others prefer a smaller circle of friends and acquaintances. How about functions.

Consider several functions with coefficients $p$ and $\mu (0 \le \mu < p)$, denoted by

$$f_0, f_1, \dots, f_{p-1}.$$

Here $p$ is a prime number and $\mu$ is an integer.

The value of function $f_i$ at an integer $x \in \{0, 1, \dots, p-1\}$ is defined as

$$f_i(x) = ((x^{i + 1} \bmod p) + (\mu^{i + 1} \bmod p)) \bmod 7.$$

We say a subset $S$ of $\{0, 1, \dots, p - 1\}$ is a circle of friends, if all functions in $S$ share the same value at no less than half of positions in $\{0, 1, \dots, p-1\}$. More specifically, a circle of friends is a subset $S=\{a_1, a_2, \dots, a_u\}$ of $\{0, 1, \dots, p-1\}$. It presents $u$ functions $f_{a_1}, f_{a_2}, \dots, f_{a_u}$, and they have the same value at $v$ distinct locations in $\{0, 1, \dots , p - 1\}$ where $2v \ge p$.

Furthermore, we say a circle of friends is “valuable” if it is maximal. That is to say that each bigger set containing a “valuable circle of friends” is not a circle of friends.

Please find and list all “valuable circles of friends”.

The inputs contains several test cases. The first line contains an integer $T (1 \le T \le 2^{10})$ which is the total number of test cases.

For each test case, a line contains two integers $p$ and $\mu$ where $p$ is a prime number and $p \le 100$.

For each test case, if the total number of “valuable circles of friends” is $F$, output $F + 1$ lines.

Each of the first $F$ lines outputs a subset of $\{0, 1, \dots , p - 1\}$, each of which should be a list of sorted numbers. All valuable circles of friends should be outputted according to the lexicographic order. The last line contains the string “END” as a terminator. 