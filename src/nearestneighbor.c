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

int find_nearest_neighbor(const int cur, int **table, const bool visited[SIZE]) {
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

TSP_Path* nearest_neighbor(int **dist) {
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

    //print_path(path, cost);
    return make_tsp_path(path,cost);
}
