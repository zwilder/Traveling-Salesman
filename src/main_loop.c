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

int g_state = STATE_MENU;
TSP_Data *g_data = NULL;

bool handle_events(void);
void update(void);
void draw(void);
void draw_example(void);
void draw_info(void);

bool main_loop(void) {
    bool running = true;
    // Main Loop
    scr_clear();
    draw(); // This advances with keypresses, so need to draw the screen before entering loop
    while(running) {
        running = handle_events();
        update();
        draw();
    }
    return true;
}

bool handle_events(void) {
    char ch = '\0';
    bool result = true;
    SList *menu = NULL;
    if(g_state == STATE_MENU) {
        menu = create_slist("The Traveling Salesman Problem! - An Example Program");
        slist_push(&menu,"Zach Wilder, 2024");
        slist_push(&menu,"abq");
        slist_push(&menu,"Generate new example");
        slist_push(&menu,"What is this?");
        slist_push(&menu,"Quit");
        clear_screen(g_screenbuf);
        draw_box(0,0,SCREEN_WIDTH, SCREEN_HEIGHT, mt_rand(RED,WHITE), BLACK);
        ch = draw_menu_nobox(menu, WHITE, BLACK);
        switch(ch) {
            case 'a': 
                generate_example();
                g_state = STATE_EXAMPLE;
                break;
            case 'b':
                g_state = STATE_INFO;
                break;
            case 'q': 
                result = false; 
                break;
            default: break;
        }
        destroy_slist(&menu);
    } else if (g_state == STATE_INFO) {
        ch = kb_get_bl_char();
        g_state = STATE_MENU;
    } else if (g_state == STATE_EXAMPLE) {
        ch = kb_get_bl_char();
        if(ch == 'q') {
            result = false;
        }
    }
    return result;
}

void update(void) {
    if(g_state == STATE_EXAMPLE) {
        // Advance to next step in path
        g_data->pos += 1;
        if(g_data->pos > SIZE) {
            g_data->pos = 0;
        }
    }
}

void draw(void) {
    clear_screen(g_screenbuf);
    if(g_state == STATE_EXAMPLE) {
        draw_example();
    } else if (g_state == STATE_INFO) {
        draw_info();
    }
    draw_screen(g_screenbuf);
}

void draw_example(void) {
    char fstr[180];
    int i,j,k,rcolor,x,y,xofs,yofs;
    bool hlrow,hlcol;
    fstr[0] = '\0';
    // Display title bar
    rcolor = mt_rand(RED,CYAN);
    snprintf(fstr, 180, "Distances with N=%d", SIZE);
    j = strlen(fstr) / 2;
    draw_hline(0,0,SCREEN_WIDTH,BRIGHT_BLACK);
    draw_colorstr(SCREEN_WIDTH/2 - j, 0, fstr, rcolor + 8,BRIGHT_BLACK);

    //Display pretty table
    yofs = SCREEN_HEIGHT/2 - SIZE/2; // offsets to center table
    xofs = SCREEN_WIDTH/2 - (SIZE*3);
    xofs = 1;
    yofs = 3;
    for(x = 0; x < SIZE; x++) {
        j = get_screen_index(4 + x + 3*x + xofs,yofs -1); 
        hlrow = (x == g_data->hk_path->path[g_data->pos - 1]);
        hlcol = (x == g_data->hk_path->path[g_data->pos]);
        if(hlrow || hlcol) {
            g_screenbuf[j].fg = rcolor + 8;
            g_screenbuf[j].bg = BLACK;
        } else {
            g_screenbuf[j].fg = WHITE;
            g_screenbuf[j].bg = BLACK;
        }
        g_screenbuf[j].ch = 'A' + x;
    }
    for(y = 0; y < SIZE; y++) {
        j = get_screen_index(0+xofs,y+yofs);
        hlrow = (y == g_data->hk_path->path[g_data->pos - 1]);
        hlcol = (y == g_data->hk_path->path[g_data->pos]);
        if(hlrow || hlcol) {
            g_screenbuf[j].fg = rcolor + 8;
            g_screenbuf[j].bg = BLACK;
        } else {
            g_screenbuf[j].fg = WHITE;
            g_screenbuf[j].bg = BLACK;
        }
        g_screenbuf[j].ch = 'A' + y;
        for(x = 0; x < SIZE; x++) {
            fstr[0] = '\0';
            if(0 == g_data->dist[x][y]) {
                snprintf(fstr,180,"   ");
            } else if(g_data->dist[x][y] < 10) {
                snprintf(fstr,180," %d ", g_data->dist[x][y]);
            } else if (g_data->dist[x][y] < 100) {
                snprintf(fstr,180,"%d ", g_data->dist[x][y]);
            } else if (g_data->dist[x][y] < 1000) {
                snprintf(fstr,180,"%d", g_data->dist[x][y]);
            } else {
                snprintf(fstr,180," x ");
            }
            k = g_data->pos;
            i = k - 1; // prev step
            if(k == SIZE) k = 0;
            hlrow = (x == g_data->hk_path->path[i]) && (g_data->hk_path->path[k] == y);
            hlcol = (y == g_data->hk_path->path[i]) && (g_data->hk_path->path[k] == x);
            // Highlight previous x,y and cur x,y
            if(hlrow || hlcol) {
                draw_colorstr(3 + x + 3*x + xofs,y+yofs,fstr,
                        BRIGHT_WHITE,rcolor);
            } else {
                if(0 == g_data->dist[x][y]) {
                    draw_colorstr(3 + x + 3*x + xofs,y+yofs,fstr,
                            BLACK, BLACK);
                } else {
                    draw_colorstr(3 + x + 3*x + xofs,y+yofs,fstr,
                            WHITE, (x % 2)?BRIGHT_BLACK:BLACK);
                }
            }
        }
    }

    //Display paths under table (NN and HK)
    fstr[0] = '\0';
    snprintf(fstr,180,"Held-Karp Path Cost: %d", g_data->hk_path->cost);
    draw_str(0, SCREEN_HEIGHT - 3, fstr);
    fstr[0] = '\0';
    for(i = 0; i < SIZE; i++) {
        j = strlen(fstr);
        snprintf(fstr + j, sizeof(fstr) - j, "%c%s", 'A' + g_data->hk_path->path[i],
                (i<SIZE-1)?"->":"->A");
    }
    draw_str(0, SCREEN_HEIGHT - 2, fstr);

    //Highlight current location in path, and current cost in table
    /* To highlight in table, From (previous step) is X and To (current step)
     * is Y
     */
    j = get_screen_index(g_data->pos+(2*g_data->pos),SCREEN_HEIGHT - 2);
    g_screenbuf[j].fg = BRIGHT_WHITE;
    g_screenbuf[j].bg = rcolor;

    fstr[0] = '\0';
    snprintf(fstr, 180, "Current Pos: %d. Press [q] to quit.", g_data->pos);
    draw_str(0,SCREEN_HEIGHT-1,fstr);
}

void draw_info(void) {
    SList *str = NULL, *strtmp = NULL;
    int i;
    draw_hline(0,0,SCREEN_WIDTH,BRIGHT_BLACK);
    i = SCREEN_WIDTH/2 - 27;
    draw_colorstr(i,0,"What the heck is this? The Traveling Salesman Problem.",WHITE,BRIGHT_BLACK);
    str = slist_linewrap(
        "Given a list of n cities and the distances between each pair of cities, what is the shortest possible route that visits each city exactly once and returns to the origin city?"
        ,75);
    slist_push(&str, " ");
    slist_push(&str, "Solutions:");
    slist_push(&str, " - Brute force O(n!) - Exact solution, can get out of hand when n > 11");
    slist_push(&str, " - Nearest Neighbor Heuristic O(n^2) - Approximate, doesn't guarantee");
    slist_push(&str, "   shortest path (Averages about 25 percent less efficient than an ");
    slist_push(&str, "   exact solution)");
    slist_push(&str, " - Held-Karp Algorithm O(n^2 * 2n) - Exact solution");
    slist_push(&str, " ");
    slist_push(&str, "Fun note: For N locations, there are N!/(2^N) solutions to this problem");
    slist_push(&str, "(assuming the distance between two individual locations is the same forward as");
    slist_push(&str, "backward - B to C is the same as C to B, etc). At 20 locations this is");
    slist_push(&str, "2,375,880,867,360,000 - or two quadrillion, three hundred seventy-five trillion,");
    slist_push(&str, "eight hundred eighty billion, eight hundred sixty-seven million, three hundred");
    slist_push(&str, "sixty thousand possible combinations. ");
    slist_push(&str, " ");
    slist_push(&str, "The SIZE (n) is currently: %d ", SIZE);
    strtmp = str;
    i = 0;
    while(strtmp) {
        draw_str(0,2+i,strtmp->data);
        i++;
        strtmp = strtmp->next;
    }
    destroy_slist(&str);
}
