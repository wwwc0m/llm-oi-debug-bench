# [PacNW 2025] Pair-Linked Mokepon

A game of Mokepon consists of $n$ stations. The player starts at station $1$. For every $i$ from $2$ through $n$, there is one special item with identifier $i$, and the player must possess that item to move from station $i-1$ to station $i$. A station may hold any number of items, and a player may carry any number of items. The objective is to reach station $n$.

You and a friend link two games: game A has $n_A$ stations and game B has $n_B$ stations. Every item is identified by a pair $(i,\mathrm A)$ or $(i,\mathrm B)$, indicating its identifier and the game it unlocks. An item may be placed at any station in either game. To advance to station $i$ in a game, either player must already have collected the corresponding item for that game.

Count the distributions of all items among all stations for which both players can reach the final station of their respective games. Two distributions differ if the set of items at some station differs. Output the count modulo the prime $p$.

The only line contains three integers $n_A$, $n_B$, and $p$ ($2\le n_A,n_B\le3\cdot10^3$, $10^8\le p\le10^9+7$). The value $p$ is guaranteed to be prime.


Output the number of winning item distributions modulo $p$.


In the first sample, there are two items, $(2,\mathrm A)$ and $(2,\mathrm B)$. Each can be placed at any of four station-game locations, so there are $4^2=16$ distributions in total. Exactly eight of them allow both players to win.