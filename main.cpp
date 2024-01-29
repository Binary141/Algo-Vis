#include <iostream>
#include "display.h"
#include <SDL2/SDL.h>
#include "search.h"


int main() {
    screen disp = init_display();

    SDL_Event event;

    int should_quit = 0;

    // create the list of all the tiles on the grid
    int* states = (int*) malloc((NUM_SQUARES * NUM_SQUARES) * sizeof(int));

    // draws grid, resets states array to be all empty, and draw state buttons
    reset(disp.renderer, states);

    search search1;
    search1.start = EMPTY_SPACE;
    search1.startx = EMPTY_SPACE;
    search1.starty = EMPTY_SPACE;
    search1.goal = EMPTY_SPACE;
    search1.goalx = EMPTY_SPACE;
    search1.goaly = EMPTY_SPACE;

    search1.states = states;

    color textColor{0, 0, 0};

    color backgroundColor{125, 125, 125};

    tile closest;
    while (SDL_WaitEvent(&event) && !should_quit) {

        // SDL_BlitSurface(image, NULL, disp.surface, NULL);
        // SDL_UpdateWindowSurface(disp.window);
        switch (event.type) {
            case SDL_QUIT:
                should_quit = 1;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_q) {
                    // If the 'q' button is pressed, quit the application
                    should_quit = 1;
                    break;
                }
                if (event.key.keysym.sym == SDLK_c) {
                    // clear the screen
                    // draws grid, resets states array to be all empty, and draw state buttons
                    reset(disp.renderer, states);
                    continue;
                }
                if (event.key.keysym.sym == SDLK_s) {
                    // color the button so the user knows
                    backgroundColor = {100, 200, 100};

                    drawStartButton(disp.renderer, textColor, backgroundColor);

                    selectStartState(disp.renderer, &search1);

                    backgroundColor = {125, 125, 125};

                    drawStartButton(disp.renderer, textColor, backgroundColor);

                    for(int i = 0; i < NUM_SQUARES * NUM_SQUARES; i++) {
                        printf("%d\n", search1.states[i]);
                    }

                    continue;
                }

                if (event.key.keysym.sym == SDLK_g) {
                    // color the button so the user knows
                    backgroundColor = {200, 100, 100};

                    drawGoalButton(disp.renderer, textColor, backgroundColor);

                    selectGoalState(disp.renderer, &search1);

                    backgroundColor = {125, 125, 125};

                    drawGoalButton(disp.renderer, textColor, backgroundColor);

                    for(int i = 0; i < NUM_SQUARES * NUM_SQUARES; i++) {
                        printf("%d\n", search1.states[i]);
                    }

                    continue;
                }
            case SDL_MOUSEBUTTONDOWN:
                // generate a wall
                int mouse_x, mouse_y;
                SDL_GetMouseState(&mouse_x, &mouse_y);

                // don't do anything if it is in the menu bar at the moment
                // adjust here for menu selections
                if (mouse_y < MENU_HEIGHT + TILE_BORDER_WIDTH + 1) {
                    // +1 comes from adding a little padding just in case
                    continue;
                }

                closest = getClosestTile(mouse_x, mouse_y);

                // color the tile if it is the left mouse button
                if( event.button.button == SDL_BUTTON_LEFT ) {

                    colorTile(disp.renderer, closest.x, closest.y, 0, 0, 255);
                    search1.states[closest.xIndex + (closest.yIndex * NUM_SQUARES)] = WALL;
                    continue;
                }
                if( event.button.button == SDL_BUTTON_RIGHT ) {
                    // clear the tile if it is the right mouse button
                    colorTile(disp.renderer, closest.x, closest.y, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);
                    search1.states[closest.xIndex + (closest.yIndex * NUM_SQUARES)] = EMPTY_SPACE;
                    continue;
                }
        }
    }
    destroy_window(disp.renderer, disp.window);

    return 0;
}
