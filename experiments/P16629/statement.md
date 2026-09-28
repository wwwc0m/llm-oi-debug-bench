# [GKS 2017 #E] Blackhole

Alice is trying to prevent dangerous black holes from threatening Earth. Right now, there are three black holes that are different points in 3-D space. Alice will create exactly three containment spheres, and all three must have the same radius. Spheres do not interfere with each other, so they may overlap.

Alice must place these spheres so that each black hole is covered by at least one sphere. Moreover, to ensure stability, the total set of points covered by at least one sphere must form a single connected area.

Alice wants to solve this critical problem as inexpensively as possible. What is the minimum radius that she can use?

The input starts with one line with one integer $T$: the number of test cases. $T$ test cases follow. Each test case consists of three lines. The i-th of those lines consists of three integers $X_i$, $Y_i$, and $Z_i$, representing the 3-D coordinates of the i-th black hole.


For each test case, output one line Case #x: y, where x is the test case number (starting from 1) and y is a rational representing the minimum radius that Alice can use to solve the problem. y will be considered correct if it is within an absolute or relative error of $10^{-6}$ of the correct answer. See the FAQ for an explanation of what that means, and what formats of real numbers we accept.

Note that the last two sample cases would not appear in the Small dataset.

In Sample Case #1, the smallest radius we can use is $1/3$. Our three spheres should be centered at $(-2/3, 0, 0), (0, 0, 0)$, and $(2/3, 0, 0)$.

### Limits

$-1000 \le X_i \le 1000$, for all i.

For all j $\ne$ k, $(X_j, Y_j, Z_j) \ne (X_k, Y_k, Z_k)$. (No two of the points have the same coordinates.)

**Small dataset (Test set 1 - Visible)**

$Y_i = 0$, for all i.

$Z_i = 0$, for all i.

**Large dataset (Test set 2 - Hidden)**

$-1000 \le Y_i \le 1000$, for all i.

$-1000 \le Z_i \le 1000$, for all i.
