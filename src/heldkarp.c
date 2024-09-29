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

TSP_Path* held_karp(int **dist, int start) {
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
        return NULL;
    }
    for(i = 0; i < (1 << SIZE); i++) {
        dp[i] = malloc(SIZE * sizeof(int));
        prev[i] = malloc(SIZE * sizeof(int));
        if(!(dp[i]) || !(prev[i])) {
            printf("Failed to allocate memory for [%d]!\n",i);
            return NULL;
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
    //print_path(path, result);

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
    
    return make_tsp_path(path, result);
}
