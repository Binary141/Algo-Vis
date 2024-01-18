#include <iostream>
#include "display.h"
#include <SDL2/SDL.h>
#include "search.h"


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
