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


search selectGoalState(SDL_Renderer* renderer, search s) {
    SDL_Event event;
    search ret = s;
    tile closest;

    while (SDL_WaitEvent(&event)) {
        switch (event.type) {
            case SDL_MOUSEMOTION:
                // used for mouse movement
                continue;
            case SDL_MOUSEBUTTONDOWN:
                int mouse_x, mouse_y;
                SDL_GetMouseState(&mouse_x, &mouse_y);
                if (mouse_y < MENU_HEIGHT) {
                    // don't do anything if it is in the menu bar. That will be handled elsewhere
                    return ret;
                }

                closest = getClosestTile(mouse_x, mouse_y);
                ret.goal = (closest.xIndex + (closest.yIndex * NUM_SQUARES));

                if (s.goal == -1) {
                    ret.goalx = closest.x;
                    ret.goaly = closest.y;
                    colorTile(renderer, ret.goalx, ret.goaly, GOAL_COLOR.r, GOAL_COLOR.g, GOAL_COLOR.b);
                } else {
                    // reset the original goal state tile to be the background color
                    colorTile(renderer, s.goalx, s.goaly, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);

                    ret.goalx = closest.x;
                    ret.goaly = closest.y;

                    // TODO look into why display blanks without this. Fast enough it doesn't really matter?
                    usleep(5000);
                    // Color the new goal state
                    colorTileByIndex(renderer, ret.goal, GOAL_COLOR.r, GOAL_COLOR.g, GOAL_COLOR.b);
                }

                return ret;
            case SDL_KEYDOWN:
                // if the user clicks away or anything, don't do anything
                return ret;
        }
    }
    return ret;
}
search selectStartState(SDL_Renderer* renderer, search s) {
    SDL_Event event;
    search ret = s;
    tile closest;

    while (SDL_WaitEvent(&event)) {
        switch (event.type) {
            case SDL_MOUSEMOTION:
                // used for mouse movement
                continue;
            case SDL_MOUSEBUTTONDOWN:
                int mouse_x, mouse_y;
                SDL_GetMouseState(&mouse_x, &mouse_y);
                if (mouse_y < MENU_HEIGHT) {
                    // don't do anything if it is in the menu bar. That will be handled elsewhere
                    return ret;
                }

                closest = getClosestTile(mouse_x, mouse_y);
                ret.start = (closest.xIndex + (closest.yIndex * NUM_SQUARES));

                if (s.start == -1) {
                    ret.startx = closest.x;
                    ret.starty = closest.y;
                    colorTile(renderer, ret.startx, ret.starty, START_COLOR.r, START_COLOR.g, START_COLOR.b);
                } else {
                    // reset the original start state tile to be the background color
                    colorTile(renderer, s.startx, s.starty, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);

                    ret.startx = closest.x;
                    ret.starty = closest.y;

                    // TODO look into why display blanks without this. Fast enough it doesn't really matter?
                    usleep(5000);
                    // Color the new start state
                    colorTileByIndex(renderer, ret.start, START_COLOR.r, START_COLOR.g, START_COLOR.b);
                }

                return ret;
            case SDL_KEYDOWN:
                // if the user clicks away or anything, don't do anything
                return ret;
        }
    }
    return ret;
}

int main() {
    screen disp = init_display();

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
                if (event.key.keysym.sym == SDLK_t) {
                    // clear the screen
                    char text[] = "Start";
                    draw_text(disp.renderer, text, 0, 0, TILE_WIDTH, MENU_HEIGHT, 255, 255, 0);
                    continue;
                }
                if (event.key.keysym.sym == SDLK_c) {
                    // clear the screen
                    draw_grid(disp.renderer);
                    continue;
                }
                if (event.key.keysym.sym == SDLK_s) {
                    // select the start state
                    search res;
                    res = selectStartState(disp.renderer, search1);
                    search1.start = res.start;
                    search1.startx = res.startx;
                    search1.starty = res.starty;

                    continue;
                }

                if (event.key.keysym.sym == SDLK_g) {
                    search res;

                    res = selectGoalState(disp.renderer, search1);
                    search1.goal = res.goal;
                    search1.goalx = res.goalx;
                    search1.goaly = res.goaly;

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
