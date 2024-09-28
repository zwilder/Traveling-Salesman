# The Traveling Salesman Problem

"Given a list of cities and the distances between each pair of cities,
what is the shortest possible route that visits each city exactly once
and returns to the origin city?"

Solutions:
- Brute force O(n!) - Exact solution, can get out of hand when n > 11
- Nearest Neighbor Heuristic O(n^2) - Approximate, doesn't guarantee
shortest path (Averages about 25% less efficient than an exact solution)
- Held-Karp Algorithm O(n^2 * 2n) - Exact solution

For this program, I'll assume that all Nodes are connected to every other
Node (Graph is fully connected).

This implementation currently uses fixed, 2D Arrays. An improvement could
be made using a simple data structure to allocate memory dynamically - if I
ever wanted to have the program take user input and spit out a path, this
would be necessary. Maximum SIZE is (theoretically) 30, but anything over 25
breaks printing. The Held-Karp dynamic programming array is huge (2^n), so it
is allocated dynamically, and that routine takes a while to complete at n >
20.

TODOs/ideas: pretty printing the table with COLOR for the path, highlighting the
costs in each step in different shades. Or having a [Press to continue]
prompt and inverting the fg/bg colors for each step... I could have the path
printed below the table (centered of course) and then invert which step we
are on as well. 
