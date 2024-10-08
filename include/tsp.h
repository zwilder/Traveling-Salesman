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
#ifndef TSP_H
#define TSP_H

/*****
 * If SIZE >16 it breaks printing with the default SCREEN_WIDTH/SCREEN_HEIGHT
 * (80x24). If you want to change SIZE higher than 16, you should change those
 * too
 *****/
#define SIZE 15

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
#include <rect.h>
#include <slist.h>
#include <term_engine.h>
#include <glyph.h>
#include <draw.h>

/*****
 * TSP Structures
 *****/

typedef struct TSP_Path TSP_Path;
typedef struct TSP_Data TSP_Data;

struct TSP_Path {
    int cost;
    int path[SIZE + 1];
};

struct TSP_Data {
    int **dist;
    TSP_Path *hk_path;
    TSP_Path *nn_path;
    int pos;
};

typedef enum {
    STATE_MENU      = 0,
    STATE_EXAMPLE   = 1,
    STATE_INFO      = 2
} AppStates;

/*****
 * TSP Functions
 *****/
TSP_Path* make_tsp_path(int path[SIZE + 1], int cost);
void destroy_tsp_path(TSP_Path *path);

TSP_Data* init_tsp_data(void);
void destroy_tsp_data(TSP_Data *data);

/*****
 * Nearest Neighbor Functions
 * nearestneighbor.c
 *****/
int find_nearest_neighbor(const int cur, int **table, const bool visited[SIZE]);
TSP_Path* nearest_neighbor(int **dist);

/*****
 * Held-Karp Functions
 * heldkarp.c
 *****/
TSP_Path* held_karp(int **dist, int start);

/*****
 * main_loop.c
 *****/
bool main_loop(void);

/*****
 * gen_example.c
 *****/
void generate_example(void);

/*****
 * Global variables
 *****/
extern int g_state;
extern TSP_Data *g_data;

#endif //TSP_H
