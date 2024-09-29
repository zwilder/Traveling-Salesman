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

TSP_Path* make_tsp_path(int path[SIZE + 1], int cost) {
    TSP_Path *newpath = malloc(sizeof(TSP_Path));
    
    int i = 0;
    for(i = 0; i < (SIZE + 1); i++) {
        newpath->path[i] = path[i];
    }

    newpath->cost = cost;
    return newpath;
}

void destroy_tsp_path(TSP_Path *path) {
    if(path) {
        free(path);
    }
}

TSP_Data* init_tsp_data(void) {
    int i = 0;
    TSP_Data *data = malloc(sizeof(TSP_Data));
    data->dist = malloc(SIZE * sizeof(int *));
    for(i = 0; i < SIZE; i++) {
        data->dist[i] = malloc(SIZE * sizeof(int));
    }
    data->hk_path = NULL;
    data->nn_path = NULL;
    data->pos = -1;
    return data;
}

void destroy_tsp_data(TSP_Data *data) {
    int i = 0;
    if(!data) return;
    if(data->dist) {
        for(i = 0; i < SIZE; i++) {
            if(data->dist[i]) {
                free(data->dist[i]);
            }
        }
        free(data->dist);
    }
    destroy_tsp_path(data->hk_path);
    destroy_tsp_path(data->nn_path);

    free(data);
}
