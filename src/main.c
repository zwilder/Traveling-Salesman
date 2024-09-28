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
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>

/* 
 * "Given a list of cities and the distances between each pair of cities,
 * what is the shortest possible route that visits each city exactly once
 * and returns to the origin city?"
 *
 * Brute force O(n!) - Exact solution, can get out of hand when n > 11
 * Nearest Neighbor Heuristic O(n^2) - Approximate, doesn't guarantee
 *  shortest path (Averages about 25% less efficient than an exact solution)
 * Held-Karp Algorithm O(n^2 * 2n) - Exact solution
 *
 * For this program, I'll assume that all Nodes are connected to every other
 * Node (Graph is fully connected)
 *
 * This implementation currently uses fixed, 2D Arrays. An improvement could
 * be made using a simple data structure to allocate memory dynamically - if I
 * ever wanted to have the program take user input and spit out a path, this
 * would be necessary. Maximum SIZE is (theoretically) 30, but anything over 25
 * breaks printing. The Held-Karp dynamic programming array is huge (2^n), so it
 * is allocated dynamically, and that routine takes a while to complete at n >
 * 20.
 *
 * An idea: pretty printing the table with COLOR for the path, highlighting the
 * costs in each step in different shades. Or having a [Press to continue]
 * prompt and inverting the fg/bg colors for each step... I could have the path
 * printed below the table (centered of course) and then invert which step we
 * are on as well. 
 */

#define SIZE 15

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
    //int dp[1 << SIZE][SIZE]; // 1 << SIZE is a neat way of saying 2 raised to the SIZE
    //int prev[1 << SIZE][SIZE];
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

int main(int argc, char** argv) {
    /*
     * Example tables of different sizes below - remember to change SIZE!
     */
    /*
    // SIZE 5
    int dist[SIZE][SIZE] = {
        {0, 35, 50, 75, 60}, // A
        {35, 0, 85, 40, 90}, // B
        {50, 85, 0, 30, 55}, // C
        {75, 40, 30, 0, 25}, // D
        {60, 90, 55, 25, 0} // E
    };
    */
    /*
    // SIZE 6
    int dist[SIZE][SIZE] = {
        {  0,  10,  15,  30,  40,  50 },  // A
        { 10,   0,  35,  25,  20,  60 },  // B
        { 15,  35,   0,  10,  50,  70 },  // C
        { 30,  25,  10,   0,  30,  80 },  // D
        { 40,  20,  50,  30,   0,  15 },  // E
        { 50,  60,  70,  80,  15,   0 }   // F
    };
    */
    // SIZE 15
    int dist[SIZE][SIZE] = {
        {  0,  10,  15,  30,  100,  200,  90,  120,  80,  110,  150,  70,  130,  160,  180 },  // A
        { 10,   0,  35,  25,  90,  180,  55,  70,  30,  50,  100,  90,  40,  60,  80 },  // B
        { 15,  35,   0,  10,  85,  175,  45,  65,  55,  60,  90,  75,  20,  50,  70 },  // C
        { 30,  25,  10,   0,  60,  150,  20,  40,  30,  45,  60,  55,  30,  35,  50 },  // D
        { 100, 90,  85,  60,   0,  70,  80,  75,  60,  80,  90,  100,  55,  50,  40 },  // E
        { 200, 180, 175, 150, 70,   0,  60,  90,  120,  50,  40,  30,  20,  10,  15 },  // F
        { 90,  55,  45,  20,  80,  60,   0,  20,  30,  40,  50,  70,  30,  20,  25 },  // G
        { 120, 70,  65,  40,  75,  90,  20,   0,  50,  10,  15,  30,  45,  60,  80 },  // H
        { 80,  30,  55,  30,  60,  120,  30,  50,   0,  15,  25,  35,  45,  60,  70 },  // I
        { 110, 50,  60,  45,  80,  50,  40,  10,  15,   0,  30,  60,  70,  80,  90 },  // J
        { 150, 100,  90,  60,  90,  40,  50,  15,  25,  30,   0,  10,  20,  30,  40 },  // K
        { 70,  90,  75,  55,  100, 30,  70,  30,  35,  60,  10,   0,  30,  25,  35 },  // L
        { 130, 40,  20,  30,  55,  20,  30,  45,  45,  70,  20,  30,   0,  15,  25 },  // M
        { 160, 60,  50,  35,  50,  10,  20,  60,  60,  80,  30,  25,  15,   0,  10 },  // N
        { 180, 80,  70,  50,  40,  15,  25,  80,  70,  90,  40,  35,  25,  10,   0 }   // O
    };
    /*
    // Size 20 - Good example here of how much more efficient Held-Karp can be
    const int dist[SIZE][SIZE] = {
        { 0,  54,  72,  38,  81,  25,  97,  66,  89,  42,  32,  75,  48,  60,  13,  84,  91,  53,  74,  18 },
        { 54,  0,  64,  12,  95,  45,  23,  76,  85,  19,  71,  14,  37,  80,  61,  27,  39,  56,  92,  59 },
        { 72, 64,  0,  58,  13,  92,  41,  70,  28,  49,  77,  33,  82,  93,  24,  35,  90,  17,  98,  51 },
        { 38, 12, 58,   0,  65,  69,  55,  43,  18,  20,  63,  11,  97,  71,  31,  96,  50,  22,  86,  34 },
        { 81, 95, 13,  65,   0,  88,  75,  54,  29,  64,  21,  99,  38,  67,  91,  62,  26,  73,  45,  93 },
        { 25, 45, 92,  69,  88,   0,  27,  95,  14,  31,  58,  42,  90,  52,  68,  47,  44,  86,  61,  72 },
        { 97, 23, 41,  55,  75,  27,   0,  15,  91,  89,  73,  59,  66,  81,  24,  92,  37,  48,  80,  99 },
        { 66, 76, 70,  43,  54,  95,  15,   0,  85,  67,  94,  39,  32,  28,  50,  31,  82,  74,  53,  40 },
        { 89, 85, 28,  18,  29,  14,  91,  85,   0,  59,  56,  96,  41,  84,  13,  38,  67,  48,  70,  79 },
        { 42, 19, 49,  20,  64,  31,  89,  67,  59,   0,  78,  62,  57,  43,  72,  94,  25,  34,  33,  65 },
        { 32, 71, 77,  63,  21,  58,  73,  94,  56,  78,   0,  91,  29,  85,  15,  62,  71,  47,  81,  20 },
        { 75, 14, 33,  11,  99,  42,  59,  39,  96,  62,  91,   0,  82,  26,  87,  49,  58,  55,  40,  81 },
        { 48, 37, 82,  97,  38,  90,  66,  32,  41,  57,  29,  82,   0,  15,  93,  44,  85,  28,  47,  74 },
        { 60, 80, 93,  71,  67,  52,  81,  28,  84,  43,  85,  26,  15,   0,  24,  61,  75,  87,  62,  49 },
        { 13, 61, 24,  31,  91,  68,  24,  50,  13,  72,  15,  87,  93,  24,   0,  32,  29,  36,  91,  17 },
        { 84, 27, 35,  96,  62,  47,  92,  31,  38,  94,  62,  49,  44,  61,  32,   0,  54,  73,  28,  45 },
        { 91, 39, 90,  50,  26,  44,  37,  82,  67,  25,  71,  58,  85,  75,  29,  54,   0,  80,  95,  61 },
        { 53, 56, 17,  22,  73,  86,  48,  74,  48,  34,  47,  55,  28,  87,  36,  73,  80,   0,  21,  60 },
        { 74, 92, 98,  86,  45,  61,  80,  53,  70,  33,  81,  40,  47,  62,  91,  28,  95,  21,   0,  69 },
        { 18, 59, 51,  34,  93,  72,  99,  40,  79,  65,  20,  81,  74,  49,  17,  45,  61,  60,  69,   0 },
    };
    */
    /*
    // Size 25 - This is about as big as this implementation can handle before
    // 'splodin. 
    int dist[SIZE][SIZE] = {
        { 0,  54,  72,  38,  81,  25,  97,  66,  89,  42,  32,  75,  48,  60,  13,  84,  91,  53,  74,  18,  99,  26,  62,  87,  44 },
        { 54,  0,  64,  12,  95,  45,  23,  76,  85,  19,  71,  14,  37,  80,  61,  27,  39,  56,  92,  59,  43,  72,  50,  30,  94 },
        { 72, 64,  0,  58,  13,  92,  41,  70,  28,  49,  77,  33,  82,  93,  24,  35,  90,  17,  98,  51,  36,  47,  53,  85,  79 },
        { 38, 12, 58,   0,  65,  69,  55,  43,  18,  20,  63,  11,  97,  71,  31,  96,  50,  22,  86,  34,  76,  28,  83,  61,  66 },
        { 81, 95, 13,  65,   0,  88,  75,  54,  29,  64,  21,  99,  38,  67,  91,  62,  26,  73,  45,  93,  16,  30,  53,  74,  19 },
        { 25, 45, 92,  69,  88,   0,  27,  95,  14,  31,  58,  42,  90,  52,  68,  47,  44,  86,  61,  72,  33,  84,  71,  39,  21 },
        { 97, 23, 41,  55,  75,  27,   0,  15,  91,  89,  73,  59,  66,  81,  24,  92,  37,  48,  80,  99,  29,  85,  77,  50,  62 },
        { 66, 76, 70,  43,  54,  95,  15,   0,  85,  67,  94,  39,  32,  28,  50,  31,  82,  74,  53,  40,  21,  36,  49,  88,  61 },
        { 89, 85, 28,  18,  29,  14,  91,  85,   0,  59,  56,  96,  41,  84,  13,  38,  67,  48,  70,  79,  45,  60,  32,  15,  74 },
        { 42, 19, 49,  20,  64,  31,  89,  67,  59,   0,  78,  62,  57,  43,  72,  94,  25,  34,  33,  65,  30,  40,  79,  86,  22 },
        { 32, 71, 77,  63,  21,  58,  73,  94,  56,  78,   0,  91,  29,  85,  15,  62,  71,  47,  81,  20,  46,  33,  42,  77,  68 },
        { 75, 14, 33,  11,  99,  42,  59,  39,  96,  62,  91,   0,  82,  26,  87,  49,  58,  55,  40,  81,  54,  88,  61,  20,  39 },
        { 48, 37, 82,  97,  38,  90,  66,  32,  41,  57,  29,  82,   0,  15,  93,  44,  85,  28,  47,  74,  58,  96,  78,  13,  81 },
        { 60, 80, 93,  71,  67,  52,  81,  28,  84,  43,  85,  26,  15,   0,  24,  61,  75,  87,  62,  49,  71,  64,  53,  30,  46 },
        { 13, 61, 24,  31,  91,  68,  24,  50,  13,  72,  15,  87,  93,  24,   0,  32,  29,  36,  91,  17,  69,  77,  68,  95,  39 },
        { 84, 27, 35,  96,  62,  47,  92,  31,  38,  94,  62,  49,  44,  61,  32,   0,  54,  73,  28,  45,  34,  57,  79,  50,  86 },
        { 91, 39, 90,  50,  26,  44,  37,  82,  67,  25,  71,  58,  85,  75,  29,  54,   0,  80,  95,  61,  41,  93,  48,  66,  53 },
        { 53, 56, 17,  22,  73,  86,  48,  74,  48,  34,  47,  55,  28,  87,  36,  73,  80,   0,  21,  60,  72,  65,  44,  71,  88 },
        { 74, 92, 98,  86,  45,  61,  80,  53,  70,  33,  81,  40,  47,  62,  91,  28,  95,  21,   0,  69,  26,  55,  78,  19,  93 },
        { 18, 59, 51,  34,  93,  72,  99,  40,  79,  65,  20,  81,  74,  49,  17,  45,  61,  60,  69,   0,  63,  27,  38,  53,  72 },
        { 99, 43, 36,  76,  16,  33,  29,  21,  45,  30,  46,  54,  58,  71,  69,  34,  41,  72,  26,  63,   0,  82,  49,  31,  47 },
        { 26, 72, 47,  28,  30,  84,  85,  36,  60,  40,  33,  88,  96,  64,  77,  57,  93,  65,  55,  27,  82,   0,  22,  51,  67 },
        { 62, 50, 53,  83,  53,  71,  77,  49,  32,  79,  42,  61,  78,  53,  68,  79,  48,  44,  78,  38,  49,  22,   0,  41,  20 },
        { 87, 30, 85,  61,  74,  39,  50,  88,  15,  86,  77,  20,  13,  30,  95,  50,  66,  71,  19,  53,  31,  51,  41,   0,  92 },
        { 44, 94, 79,  66,  19,  21,  62,  61,  74,  22,  68,  39,  81,  46,  39,  86,  53,  88,  93,  72,  47,  67,  20,  92,   0 }
    };
    */
    /*
    // Size 30 - This breaks the current implementation! I couldn't get it to
    // run 
    int dist[SIZE][SIZE] = {
        {  0,  68,  55,  72,  58,  83,  97,  71,  76,  44,  90,  93,  35,  92,  66,  38,  81,  87,  84,  98,  63,  77,  69,  54,  85,  79,  94,  88,  50,  100 },
        { 68,   0,  47,  91,  76,  99,  73,  87,  44,  59,  68,  66,  91,  53,  40,  80,  73,  56,  42,  64,  98,  65,  62,  67,  72,  81,  58,  77,  88,  83 },
        { 55,  47,   0,  89,  69,  85,  77,  54,  81,  38,  64,  56,  72,  79,  53,  63,  49,  67,  44,  88,  99,  84,  63,  82,  41,  70,  87,  92,  61,  58 },
        { 72,  91,  89,   0,  58,  82,  94,  77,  56,  68,  79,  84,  50,  63,  91,  60,  82,  92,  87,  65,  77,  89,  46,  55,  67,  76,  80,  74,  62,  81 },
        { 58,  76,  69,  58,   0,  90,  95,  88,  53,  76,  59,  81,  47,  66,  45,  84,  77,  55,  80,  99,  44,  56,  90,  49,  75,  97,  62,  83,  98,  87 },
        { 83,  99,  85,  82,  90,   0,  79,  74,  55,  67,  96,  93,  89,  62,  44,  56,  88,  46,  84,  61,  91,  68,  83,  50,  66,  87,  75,  79,  57,  60 },
        { 97,  73,  77,  94,  95,  79,   0,  66,  89,  85,  73,  51,  92,  84,  76,  90,  63,  47,  66,  55,  71,  93,  86,  64,  95,  58,  60,  67,  49,  82 },
        { 71,  87,  54,  77,  88,  74,  66,   0,  57,  44,  75,  56,  67,  42,  59,  81,  83,  77,  91,  65,  93,  53,  64,  79,  72,  82,  58,  71,  89,  60 },
        { 76,  44,  81,  56,  53,  55,  89,  57,   0,  50,  97,  87,  44,  73,  55,  84,  56,  60,  66,  77,  43,  81,  92,  60,  75,  70,  48,  66,  85,  57 },
        { 44,  59,  38,  68,  76,  67,  85,  44,  50,   0,  65,  61,  49,  84,  53,  72,  93,  57,  77,  58,  74,  86,  62,  53,  56,  66,  49,  81,  90,  73 },
        { 90,  68,  64,  79,  59,  96,  73,  75,  97,  65,   0,  66,  79,  53,  46,  87,  89,  92,  75,  55,  81,  72,  80,  60,  62,  85,  49,  67,  77,  50 },
        { 93,  66,  56,  84,  81,  93,  51,  56,  87,  61,  66,   0,  59,  85,  64,  45,  88,  63,  90,  77,  48,  74,  61,  73,  55,  79,  92,  84,  67,  90 },
        { 35,  91,  72,  50,  47,  89,  92,  67,  44,  49,  79,  59,   0,  45,  55,  70,  64,  58,  76,  83,  60,  50,  62,  80,  93,  66,  75,  90,  56,  78 },
        { 92,  53,  79,  63,  66,  62,  84,  42,  73,  84,  53,  85,  45,   0,  58,  80,  50,  97,  89,  77,  73,  68,  59,  55,  61,  67,  87,  91,  46,  57 },
        { 66,  40,  53,  91,  45,  44,  76,  59,  55,  53,  46,  64,  55,  58,   0,  62,  47,  81,  73,  80,  63,  79,  75,  53,  67,  49,  78,  85,  90,  74 },
        { 38,  80,  63,  60,  84,  56,  90,  81,  84,  72,  87,  45,  70,  80,  62,   0,  74,  60,  59,  76,  66,  73,  85,  77,  43,  61,  67,  58,  77,  46 },
        { 81,  73,  49,  82,  77,  88,  63,  83,  56,  93,  89,  88,  64,  50,  47,  74,   0,  68,  59,   99,  72,  87,  66,  55,  80,  85,  74,  61,  46,  69 },
        { 87,  56,  67,  92,  55,  46,  47,  77,  60,  57,  92,  63,  58,  97,  81,  60,  68,   0,  64,  76,  85,  66,  59,  42,  91,  70,  56,  77,  90,  67 },
        { 84,  42,  44,  87,  80,  84,  66,  91,  66,  77,  75,  90,  76,  89,  73,  59,  59,  64,   0,  53,  83,  55,  87,  77,  65,  59,  90,  85,  66,  79 },
        { 98,  64,  88,  65,  99,  61,  55,  65,  77,  58,  55,  77,  83,  77,  80,  76,  99,  76,  53,   0,  56,  81,  66,  88,  74,  53,  69,  59,  71,  82 },
        { 63,  98,  99,  77,  44,  91,  71,  93,  43,  74,  81,  48,  60,  73,  63,  66,  72,  85,  83,  56,   0,  77,  90,  89,  54,  77,  73,  64,  82,  67 },
        { 77,  65,  84,  89,  56,  68,  93,  53,  81,  86,  72,  74,  50,  68,  79,  73,  87,  66,  55,  81,  77,   0,  59,  73,  84,  70,  99,  91,  85,  78 },
        { 69,  62,  63,  46,  90,  83,  86,  64,  92,  62,  80,  61,  62,  59,  75,  85,  66,  59,  87,  66,  90,  59,   0,  71,  93,  88,  69,  54,  49,  67 },
        { 54,  67,  82,  55,  49,  50,  64,  79,  60,  53,  60,  73,  80,  55,  53,  77,  55,  42,  77,  88,  89,  73,  71,   0,  85,  78,  66,  56,  71,  83 },
        { 85,  72,  41,  67,  75,  66,  95,  72,  75,  56,  62,  55,  93,  61,  67,  43,  80,  91,  65,  74,  54,  84,  93,  85,   0,  46,  57,  63,  56,  78 },
        { 79,  81,  70,  76,  97,  87,  58,  82,  70,  66,  85,  79,  66,  67,  49,  61,  85,  70,  59,  53,  77,  70,  88,  78,  46,   0,  65,  83,  91,  73 },
        { 94,  58,  87,  80,  62,  75,  60,  58,  48,  49,  49,  92,  75,  87,  78,  67,  74,  56,  90,  69,  73,  99,  69,  66,  57,  65,   0,  61,  77,  56 },
        { 88,  77,  92,  74,  83,  79,  67,  71,  66,  81,  67,  84,  90,  91,  85,  58,  61,  77,  85,  59,  64,  91,  54,  56,  63,  83,  61,   0,  79,  62 },
        { 50,  88,  61,  62,  98,  57,  49,  89,  85,  90,  77,  67,  56,  46,  90,  77,  46,  90,  66,  71,  82,  85,  49,  71,  56,  91,  77,  79,   0,  50 },
        { 100, 83,  58,  81,  87,  60,  82,  60,  57,  73,  50,  90,  78,  57,  74,  46,  69,  67,  79,  82,  67,  78,  67,  83,  78,  73,  56,  62,  50,   0 }
    };
    */
    print_table(dist);
    printf("Nearest Neighbor Heuristic:\n");
    nearest_neighbor(dist);
    printf("Held-Karp Algorithm:\n");
    held_karp(dist, 0);
    return 0;
}
