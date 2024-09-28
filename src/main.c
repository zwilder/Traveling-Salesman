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

/* 
 * This file could (will) be cleaned up when the program is "finished". I always
 * like main.c to be C L E A N. File currently includes the application logic
 * (events/update/draw), state info, etc. I'll probably slap the majority of
 * this into a "main_loop.c" file or some such later.
 */

typedef enum {
    STATE_MENU      = 0,
    STATE_EXAMPLE   = 1,
    STATE_INFO      = 2
} AppStates;


int g_state = STATE_MENU;
TSP_Data *g_data = NULL;

bool handle_events(void);
void update(void);
void draw(void);
void generate_example(void);

int main(int argc, char** argv) {
    /*
     * The command line switch would be super cool to use for doing a terminal
     * version or a graphical version... I could use SDL to draw points on a
     * screen, then step through and draw lines between points for the optimium
     * path... eventually.
     */
    bool running = true;
    // Initialize the terminal and screenbuffer
    term_init();
    init_screenbuf();
    g_data = init_tsp_data();

    // Main Loop
    scr_clear();
    draw(); // This advances with keypresses, so need to draw the screen before entering loop
    while(running) {
        running = handle_events();
        update();
        draw();
    }
    
    // Destroy the screen buffer and return the terminal
    destroy_tsp_data(g_data);
    close_screenbuf();
    term_close();
    return 0;
}

bool handle_events(void) {
    char ch = '\0';
    bool result = true;
    SList *menu = NULL;
    if(g_state == STATE_MENU) {
        menu = create_slist("The Traveling Salesman Problem! - Example Program");
        slist_push(&menu,"Zach Wilder, 2024");
        slist_push(&menu,"abq");
        slist_push(&menu,"Generate new example");
        slist_push(&menu,"What is this?");
        slist_push(&menu,"Quit");
        clear_screen(g_screenbuf);
        ch = draw_menu_basic(menu);
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
        if(ch == 'q') {
            result = false;
        }
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
    }
}

void draw(void) {
    clear_screen(g_screenbuf);
    SList *str = NULL, *strtmp = NULL;
    int i = 0;
    if(g_state == STATE_EXAMPLE) {
        draw_colorstr(0,0,"EXAMPLE STATE!",mt_rand(RED,WHITE),BLACK);
        //Display pretty table

        //Display paths under table (NN and HK)

        //Highlight current location in path, and current cost in table
        /* To highlight in table, From (previous step) is X and To (current step)
         * is Y
         */
    } else if (g_state == STATE_INFO) {
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
    }
    draw_screen(g_screenbuf);
    destroy_slist(&str);
}

void generate_example(void) {
    // Make a distance table
    // Temporary, before we start using random numbers for x,y points and
    // finding distances using man_dist(A,B)
    int x,y;
    // SIZE 6
    if(!g_data) return;
    int dist[SIZE][SIZE] = {
        {  0,  10,  15,  30,  40,  50 },  // A
        { 10,   0,  35,  25,  20,  60 },  // B
        { 15,  35,   0,  10,  50,  70 },  // C
        { 30,  25,  10,   0,  30,  80 },  // D
        { 40,  20,  50,  30,   0,  15 },  // E
        { 50,  60,  70,  80,  15,   0 }   // F
    };

    for(x = 0; x < SIZE; x++) {
        for(y = 0; y < SIZE; y++) {
            g_data->dist[x][y] = dist[x][y];
        }
    }

    //Print L O A D I N G in center of screen
    //(This doesn't show up at all if it goes quick, but for large SIZE it looks
    //nice)
    clear_screen(g_screenbuf);
    draw_colorstr(g_screenW/2 - 6, g_screenH, 
            "L O A D I N G", 
            mt_rand(BRIGHT_BLACK, BRIGHT_WHITE), BLACK);
    draw_screen(g_screenbuf);
    //Generate paths
    g_data->hk_path = held_karp(g_data->dist,0);
    
    //TSP_Path *hkpath = held_karp(dist, 0);
    //destroy_tsp_path(hkpath);
    //print_table(dist);
    //printf("Nearest Neighbor Heuristic:\n");
    //nearest_neighbor(dist);
    //printf("Held-Karp Algorithm:\n");
    //held_karp(dist, 0);

}

/*
 * Example tables of different sizes below - can copy/paste them above.
 * Remember to change SIZE!
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
/*
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
*/
/*
// Size 20 - Good example here of how much more efficient Held-Karp can be
// over Nearest Neighbor
int dist[SIZE][SIZE] = {
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
