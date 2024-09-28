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

#define SIZE 20

/*****
 * System
 *****/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>


/*****
 * Toolbox
 *****/
#include <mt19937.h>
#include <vec2i.h>
#include <term_engine.h>
#include <glyph.h>

/*****
 * TSP Functions
 *****/
void print_table(const int table[SIZE][SIZE]);
void print_path(const int path[SIZE], int cost);
int find_nearest_neighbor(const int cur, const int table[SIZE][SIZE], const bool visited[SIZE]);
void nearest_neighbor(const int dist[SIZE][SIZE]);
void held_karp(const int dist[SIZE][SIZE], int start);
