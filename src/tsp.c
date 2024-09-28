/*
* TSP
* Copyright (C) Zach Wilder 2024
* 
* This file is a part of TSP
*
* TSP is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* TSP is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with TSP.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <tsp.h>

// SIZE is #define'd in tsp.h

void print_table(const int table[SIZE][SIZE]) {
    int x,y;
    for(x = 0; x < SIZE; x++) {
        printf("\t%c", 'A' + x);
    } 
    for (y = 0; y < SIZE; y++) {
        printf("\n%c\t", 'A' + y);
        for(x = 0; x < SIZE; x++) {
            printf("%d\t", table[x][y]);
        }
    }
    printf("\n");
}

void print_path(const int path[SIZE], int cost) {
    int i;
    printf("Path, total cost %d:\n\t", cost);
    for(i = 0; i < SIZE; i++) {
        printf("%c%s", 'A' + path[i], (i<(SIZE-1)?"->":"->A\n"));
    }
}

int find_nearest_neighbor(const int cur, const int table[SIZE][SIZE], const bool visited[SIZE]) {
    // Return the node with the lowest cost 
    int i = 0;
    int cost = 999;
    int next = cur;
    for(i = 0; i < SIZE; i++) {
        if((i != cur) && !visited[i]) {
            if(table[cur][i] < cost) {
                cost = table[cur][i];
                next = i;
            }
        }
    }
    return next;
}

void nearest_neighbor(const int dist[SIZE][SIZE]) {
    /*
     * Nearest Neighbor Heuristic Algorithm
     * Quick and easy approach to solving the TSP - knowing where we start, all
     * we have to do is keep track of what spots have been visited, then move to
     * the unvisited spot with the lowest cost. 
     */
    bool visited[SIZE] = {false};
    int path[SIZE];
    int i = 0;
    int cur = 0; // Start at A, this could be passed in
    int next = 0;
    int cost = 0;

    visited[cur] = true; // Mark first node as visited
    path[0] = cur;
    // We know where we are at (cur), so we need to figure out where to go.
    // Check unvisited nodes (visited[i] == false), find the smallest cost
    for(i = 1; i < SIZE; i++) {
        next = find_nearest_neighbor(cur, dist, visited);
        path[i] = next;
        cost += dist[cur][next];
        cur = next;
        visited[cur] = true;
    }
    cost += dist[cur][0]; // Add in the cost of the return

    print_path(path, cost);
}

void held_karp(const int dist[SIZE][SIZE], int start) {
    /*
     * Held-Karp Algorithm - Dynamic Programming
     * This uses some bitmath magic to keep track of path costs/visited nodes
     * and then reconstructs the shortest path. 
     * the "dynamic programming table" dp is a bitmask - dp[subset][end] where:
     *  - Subset represents the nodes that have been visited
     *  - End is the last node in the current path
     *  - dp[subset][end] stores the minimum cost to reach 'end' after visiting
     *    nodes in 'subset'
     *  - Since this current implementation uses a fixed SIZE for the number of
     *    nodes, the upper limit of SIZE would be 30 (2^31 is more than
     *    INT_MAX). This could (probably) be improved by allocating memory
     *    dynamically - or if the points are on an X,Y grid calculating the
     *    distance (manhattan) instead of storing the costs.
     */
    int **dp;
    int **prev;
    int path[SIZE + 1];
    int subset, last, newcost, cost, end, i, cur, next;
    int result = INT_MAX;

    // Allocate memory for dp/prev
    dp = malloc((1 << SIZE) * sizeof(int *));
    prev = malloc((1 << SIZE) * sizeof(int *));
    if(!dp || !prev) {
        printf("Failed to allocate memory for dp/prev!\n");
        return;
    }
    for(i = 0; i < (1 << SIZE); i++) {
        dp[i] = malloc(SIZE * sizeof(int));
        prev[i] = malloc(SIZE * sizeof(int));
        if(!(dp[i]) || !(prev[i])) {
            printf("Failed to allocate memory for [%d]!\n",i);
            return;
        }
    }

    // Start by filling the dp table with absurdly high values
    for(subset = 0; subset < (1 << SIZE); subset++) {
        for(i = 0; i < SIZE; i++) {
            dp[subset][i] = INT_MAX;
        }
    }

    dp[1 << start][start] = 0; // Starting point has no cost

    /*
     * Iterate over subsets - for each subset of nodes, calculate the cost of
     * reaching each node 'last' by extending paths from every other node 'i'
     */
    for(subset = 0; subset < (1 << SIZE); subset++) {
        for(last = 0; last < SIZE; last++) {
            if(!(subset & (1 << last))) {
                continue;
            }

            // Try visiting each possible previous node
            for(i = 0; i < SIZE; i++) {
                if(i == last || !(subset & (1 << i))) {
                    continue;
                }

                // check this bit magic. 
                if (dp[subset ^ (1 << last)][i] != INT_MAX) {    
                    /*
                     * subset ^ (1 << last) removes the 'last' node from the
                     * subset.
                     *  - 1 << last is a bitmask where only the bit from last is
                     *    'on' (1)
                     *  - XOR (^) flips the bit containing last node, turning it
                     *    'off' (0)
                     *  => subset ^ (1 << last) is the subset of nodes, without
                     *     the last node
                     * dp[subset ^ (1 << last)][i] is the minimum cost of
                     * visiting the reduced subset of nodes and ending at 'i'
                     *
                     * Example: 3 Nodes, A B C, 0111. Considering travel to C
                     * from B.
                     *  - subset is {A, B, C} - 0111
                     *  - Want cost of visiting {A,B} and then travel to C
                     *  - last = C = 1 << 2 = 0100
                     *  - subset ^ (1 << last) = 0111 ^ 0100 = 0011 = {A,B}
                     *  - dp[subset ^ (1 << last)][i] = dp[cost to visit
                     *    A,B][end at B]
                     *  - dist[i][last] = dist[B][C] = cost of travel from B to
                     *    C
                     *  => New cost is the minimum cost to vist A,B (ending at
                     *    B), added to the cost of visiting C from B.
                     */
                    newcost = dp[subset ^ (1 << last)][i] + dist[i][last]; 
                    if(newcost < dp[subset][last]) {
                        dp[subset][last] = newcost;
                        prev[subset][last] = i; // track path
                    }
                }
            }
        }
    }
    
    // Calculate the cost of returning to the start node (completing the tour)
    for(last = 0; last < SIZE; last++) {
        cost = dp[(1 << SIZE) - 1][last] + dist[last][start];
        if(cost < result) {
            result = cost;
            end = last;
        }
    }

    // Backtrack to reconstruct the path
    cur = (1 << SIZE) - 1;
    for(i = SIZE - 1; i > 0; i--) {
        path[i] = end;
        next = cur ^ (1 << end);
        end = prev[cur][end];
        cur = next;
    }
    path[0] = start;

    // Print the results!
    print_path(path, result);

    // Free allocated memory
    for(i = 0; i < (1 << SIZE); i++) {
        if(dp[i]) {
            free(dp[i]);
        }
        if(prev[i]) {
            free(prev[i]);
        }
    }
    if(dp) {
        free(dp);
    }
    if(prev) {
        free(prev);
    }
}
