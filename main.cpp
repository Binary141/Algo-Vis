#include <stdio.h>
#include <stdbool.h>
#include <unistd.h> // usleep()
#include <iostream>
#include "display.h"
#include <SDL2/SDL.h>

struct search {
    int start;
    int startx;
    int starty;
    int goal;
    int goalx;
    int goaly;
};

int main() {
    screen disp = init_display();
    if(!disp.window) {
        printf("Window could not be created with error: %s\n", SDL_GetError());
        return 0;
    }
    if(!disp.renderer) {
        printf("Renderer could not be created with error: %s\n", SDL_GetError());
        return 0;
    }

    SDL_Event event;

    int should_quit = 0;
    int red_iter = 0;
    int blue_iter = 255;

    draw_grid(disp.renderer);

    search search1;
    search1.start = -1;
    search1.startx = -1;
    search1.starty = -1;
    search1.goal = -1;
    search1.goalx = -1;
    search1.goaly = -1;

    tile closest;
    while (SDL_WaitEvent(&event) && !should_quit) {
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
                    draw_grid(disp.renderer);
                    continue;
                }
                if (event.key.keysym.sym == SDLK_s) {
                    // select the start state
                    int done = 0;
                    while (SDL_WaitEvent(&event) && done == 0) {
                        switch (event.type) {
                            case SDL_MOUSEMOTION:
                                continue;
                            case SDL_MOUSEBUTTONDOWN:
                                int mouse_x, mouse_y;
                                SDL_GetMouseState(&mouse_x, &mouse_y);
                                closest = getClosestTile(mouse_x, mouse_y);

                                if (search1.start == -1) {
                                    std::cout << "Start node was created!" << std::endl;
                                    search1.start = (closest.xIndex + (closest.yIndex * NUM_SQUARES));
                                    search1.startx = closest.x;
                                    search1.starty = closest.y;
                                    colorTile(disp.renderer, search1.startx, search1.starty, 0, 255, 0);
                                } else {
                                    colorTile(disp.renderer, search1.startx, search1.starty, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);

                                    search1.start = (closest.xIndex + (closest.yIndex * NUM_SQUARES));
                                    search1.startx = closest.x;
                                    search1.starty = closest.y;

                                    // TODO look into why display blanks without this. Fast enough it doesn't really matter?
                                    usleep(5000);
                                    colorTileByIndex(disp.renderer, search1.start, 0, 255, 0);
                                }

                                done = 1;
                                break;
                            case SDL_KEYDOWN:
                                done = 1;
                                break;
                        }
                    }
                    continue;
                }

                if (event.key.keysym.sym == SDLK_g) {
                    // select the goal state
                    int done = 0;
                    while (SDL_WaitEvent(&event) && done == 0) {
                        switch (event.type) {
                            case SDL_MOUSEMOTION:
                                continue;
                            case SDL_MOUSEBUTTONDOWN:
                                int mouse_x, mouse_y;
                                SDL_GetMouseState(&mouse_x, &mouse_y);
                                closest = getClosestTile(mouse_x, mouse_y);

                                if (search1.goal == -1) {
                                    std::cout << "Goal node was created!" << std::endl;
                                    search1.goal = (closest.xIndex + (closest.yIndex * NUM_SQUARES));
                                    search1.goalx = closest.x;
                                    search1.goaly = closest.y;
                                    colorTile(disp.renderer, search1.goalx, search1.goaly, 255, 0, 0);
                                } else {
                                    colorTile(disp.renderer, search1.goalx, search1.goaly, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);

                                    search1.goal = (closest.xIndex + (closest.yIndex * NUM_SQUARES));
                                    search1.goalx = closest.x;
                                    search1.goaly = closest.y;

                                    // TODO look into why display blanks without this. Fast enough it doesn't really matter?
                                    usleep(5000);
                                    colorTileByIndex(disp.renderer, search1.goal, 255, 0, 0);
                                }

                                done = 1;
                                break;
                            case SDL_KEYDOWN:
                                done = 1;
                                break;
                        }
                    }
                    continue;
                }
            case SDL_MOUSEBUTTONDOWN:
                // generate a wall
                int mouse_x, mouse_y;
                SDL_GetMouseState(&mouse_x, &mouse_y);
                closest = getClosestTile(mouse_x, mouse_y);

                // color the tile if it is the left mouse button
                if( event.button.button == SDL_BUTTON_LEFT ) {
                    // don't try to color if it is in the menu bar area
                    if (mouse_y < MENU_HEIGHT + TILE_BORDER_WIDTH + 1) {
                        // +1 comes from adding a little padding just in case
                        continue;
                    }
                    closest = getClosestTile(mouse_x, mouse_y);

                    colorTile(disp.renderer, closest.x, closest.y, 0, 0, 255);
                    continue;
                }
                if( event.button.button == SDL_BUTTON_RIGHT ) {
                    // clear the tile if it is the right mouse button
                    colorTile(disp.renderer, closest.x, closest.y, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);
                    continue;
                }
        }
    }
    destroy_window(disp.renderer, disp.window);

    return 0;
}
