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

int main(int argc, char** argv) {
    /*
     * The command line switch would be super cool to use for doing a terminal
     * version or a graphical version... I could use SDL to draw points on a
     * screen, then step through and draw lines between points for the optimium
     * path... eventually.
     */
    init_genrand(time(NULL)); // Seed the prng
    term_init(); // Initialize terminal interface
    init_screenbuf(); // Start screen buffer
    g_data = init_tsp_data(); // Global data

    main_loop(); // Main loop, obviously

    destroy_tsp_data(g_data); // Cleanup global data
    close_screenbuf(); // Close the screen buffer
    term_close(); // Return the terminal to the user
    return 0;
}
