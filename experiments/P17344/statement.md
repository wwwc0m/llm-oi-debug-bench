# [ECNA 2025] AROD

Since retiring from a lucrative athletic career, Alex has devoted most of his time to pondering foundational concepts in mathematics. Recently, he has been focusing on the categorization of triangles based on their interior angles, and has invented the acronym AROD to keep track of the four fundamental types:

-   **A =** *acute:* all three angles are less than $90$ degrees

-   **R =** *right:* one angle is $90$ degrees

-   **O =** *obtuse:* one angle is greater than $90$ degrees, but less than $180$ degrees

-   **D =** *degenerate:* one angle is $180$ degrees, or, equivalently, the three vertices are collinear

Alex wonders how often three distinct vertices chosen from a regular grid of $x\textrm{-}y$ points specify a triangle in each of the four AROD categories. More precisely, for positive integers $m_x$ and $m_y,$ he wants to consider all possible ways of choosing three distinct vertices from the set $$V(m_x,m_y) = \{ (x,y) : x \textrm{ and } y \textrm{ are integers, } 0 \leq x \leq m_x, 0 \leq y \leq m_y \}$$ and then categorize each of the corresponding triangles into one of the four categories listed above.

The input is a line containing two positive integers, $m_x$ and $m_y,$ satisfying $m_x + m_y \leq 600.$


Output four lines containing the numbers of times three distinct vertices chosen from $V(m_x,m_y)$ specify an acute, right, obtuse, or degenerate triangle, in that order (one number per line).
