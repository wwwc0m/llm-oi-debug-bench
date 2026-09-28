# [PacNW 2025] Farthest City

The State of Confusion contains $n$ cities, numbered $1$ through $n$, and $n$ bidirectional roads.
Road $i$ connects cities $a_i$ and $b_i$ and has length $d_i$ miles.

All cities are connected. No road connects a city to itself, and there is at most one road between
any pair of cities. The distance between two cities is the length of the shortest path connecting
them.

For every city, find the distance to the city farthest from it.

The first line contains an integer $n$ ($3\le n\le3\cdot10^5$), the number of cities.

Each of the next $n$ lines contains three integers $a_i$, $b_i$, and $d_i$ ($1\le d_i\le10^7$),
describing one road.

Output $n$ integers on one line. The $i$th integer is the distance from city $i$ to its farthest
city.