# [PacNW 2025] Server Room

Audrey is system administrator for miHoYo! It is her job to manage the server room and keep her favorite video game, Honkai: Star Rail, up and running.

Audrey manages a rectangular server room divided into $r$ rows and $c$ columns. Some cells contain servers, and some of those servers are already on.

The room has poor ventilation. If two orthogonally adjacent servers are both on, the room overheats and every server shuts down. Audrey cannot turn off a server that is already on.

Find the maximum number of currently off servers Audrey can turn on without causing overheating, and count the number of distinct ways to turn on that many servers.


The first line contains two positive integers $r$ and $c$ with $1\le r\cdot c\le400$.

Each of the next $r$ lines contains a string of length $c$ over `0`, `1`, and `2`. A `0` denotes a cell without a server, a `1` an off server, and a `2` an on server. No two initially on servers are adjacent.


Let $s$ be the maximum number of additional servers that can be turned on, and let $w$ be the number of ways to turn on exactly $s$ servers. Output $s$ and $w\bmod998244353$.
