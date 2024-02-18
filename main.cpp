#include <iostream>
#include "display.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "search.h"
#include <unistd.h> // usleep()
#include <thread>

extern int doneSearching;
int isSearching = 0;

search getDefaultSearch() {
    search search1;
    search1.start = EMPTY_SPACE;
    search1.startx = EMPTY_SPACE;
    search1.starty = EMPTY_SPACE;
    search1.goal = EMPTY_SPACE;
    search1.goalx = EMPTY_SPACE;
    search1.goaly = EMPTY_SPACE;
    return search1;
}

int main() {
    screen disp = init_display();

    SDL_Event event;

    int should_quit = 0;
    settings setting = getDisplaySettings();

    // create the list of all the tiles on the grid
    int* states;
    states = (int*) malloc((setting.numTiles * setting.numTiles) * sizeof(int));

    // draws grid, resets states array to be all empty, and draw state buttons
    reset(disp.renderer, disp.texture1, states);
    usleep(5000);
    testTexture(disp.renderer, disp.texture2, states);

    // draw the start and goal buttons
    drawStatusBar(disp.renderer, disp.statusTexture, textColor, backgroundColor);

    // we shouldn't really be directly accessing the texture1 and texture2 vars
    // outside of here except for performing a swap on then
    disp.currTexture = disp.texture1;
    disp.backTexture = disp.texture2;

    search search1 = getDefaultSearch();
    search1.stateSize = (setting.numTiles * setting.numTiles);
    search1.numTiles = setting.numTiles;

    search1.states = states;

    tile closest;
    int thing = 0;

    SDL_Rect texr;
    texr.x = 0;
    texr.y = 0;
    texr.w = 100;
    texr.h = 200;

    SDL_Surface* textureSurface;
    int hasRan = 0;

    while (isSearching || (SDL_WaitEvent(&event) && !should_quit)) {
        switch (event.type) {
            case SDL_QUIT:
                should_quit = 1;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_m:
                        printf("Saving?\n");
                        IMG_SavePNG(disp.surface, "testing.png");
                        continue;
                    case SDLK_p:
                        // demo to swap the textures

                        disp.backTexture = disp.currTexture;

                        // alternate the current
                        if (!thing) {
                            disp.currTexture = disp.texture2;
                            thing = 1;
                        } else {
                            disp.currTexture = disp.texture1;
                            thing = 0;
                        }

                        SDL_RenderCopy(disp.renderer,
                                       disp.currTexture,
                                       NULL,
                                       NULL);
                        SDL_RenderPresent(disp.renderer);
                        continue;
                    case SDLK_q:
                        should_quit = 1;
                        break;
                    case SDLK_t:
                        // clears all tiles and redraws onto selected buffer
                        clearTilesFromTexture(disp.renderer, disp.backTexture, &search1, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);
                        continue;
                    case SDLK_r:
                        // much faster way of clearing the screen
                        clearTilesBulk(disp.renderer, disp.currTexture, &search1, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);

                        continue;
                    case SDLK_c:
                        // clear the screen
                        // draws grid, resets states array to be all empty, and draw state buttons
                        reset(disp.renderer, disp.currTexture, states);
                        continue;
                    case SDLK_s:
                        // color the button so the user knows

                        drawStartButton(disp.renderer, disp.statusTexture, textColor, color{100, 200, 100});
                        selectStartState(disp.renderer, disp.currTexture, &search1);

                        drawStartButton(disp.renderer, disp.statusTexture, textColor, backgroundColor);
                        continue;
                    case SDLK_g:
                        // color the button so the user knows

                        drawGoalButton(disp.renderer, disp.statusTexture, textColor, color{200, 100, 100});
                        selectGoalState(disp.renderer, disp.currTexture, &search1);

                        drawGoalButton(disp.renderer, disp.statusTexture, textColor, backgroundColor);
                        continue;
                }

                if (event.key.keysym.sym == SDLK_b || event.key.keysym.sym == SDLK_u || event.key.keysym.sym == SDLK_j || event.key.keysym.sym == SDLK_k) {
                    char text[] = "Searching!";

                    draw_text(disp.renderer, disp.statusTexture, text, 0, 0, 100, setting.menuHeight, textColor, backgroundColor);

                    isSearching = 1;
                    doneSearching = 0;

                    // this thread will set isSearching to 0 (false) if a key is pressed
                    // this stops the search from continuing
                    std::thread t1(waitForSearch);
                    // std::thread t2(clearTilesFromTexture, disp.renderer, disp.backTexture, &search1, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);

                    // do the search
                    if (event.key.keysym.sym == SDLK_b) {
                        bfs(disp.renderer, disp.currTexture, &search1);
                    } else if (event.key.keysym.sym == SDLK_u) {
                        dfs(disp.renderer, disp.currTexture, &search1);
                    } else if (event.key.keysym.sym == SDLK_k) {
                        greedy(disp.renderer, disp.currTexture, &search1);
                    } else {
                        astar(disp.renderer, disp.currTexture, &search1);
                    }

                    isSearching = 0;
                    hasRan = 1;

                    // make sure that the thread finished
                    t1.join();
                    // t2.join();

                    char doneText[] = "Done!";
                    draw_text(disp.renderer, disp.statusTexture, doneText, 0, 0, 100, setting.menuHeight, textColor, backgroundColor);
                }

                if (event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_d) {
                    int key = event.key.keysym.sym;

                    // do nothing if we are going to have less than a 2x2 grid
                    // not sure what you would do with that
                    if (key == SDLK_d && setting.numTiles == 2) {
                        continue;
                    }

                    if (key == SDLK_d) {
                        setting.numTiles -= 1;
                    } else {
                        setting.numTiles += 1;
                    }

                    setDisplaySettings(setting);
                    destroy_window(disp.renderer, disp.window);
                    disp = init_display();
                    states = (int*) realloc(states, (setting.numTiles * setting.numTiles) * sizeof(int));

                    search1 = getDefaultSearch();
                    search1.states = states;
                    search1.stateSize = (setting.numTiles * setting.numTiles);
                    search1.numTiles = setting.numTiles;
                    disp.currTexture = disp.texture1;

                    usleep(5000);
                    reset(disp.renderer, disp.currTexture, states);
                }
            case SDL_MOUSEBUTTONDOWN:
                // generate a wall
                int mouse_x, mouse_y;
                SDL_GetMouseState(&mouse_x, &mouse_y);

                // don't do anything if it is in the menu bar at the moment
                // adjust here for menu selections
                if (isInMenu(mouse_y)) {
                    continue;
                }

                // color the tile if it is the left mouse button
                if( event.button.button == SDL_BUTTON_LEFT ) {
                    manyWalls(disp.renderer, disp.currTexture, &search1, 0);
                }

                // make the space a wall
                if( event.button.button == SDL_BUTTON_RIGHT ) {
                    manyWalls(disp.renderer, disp.currTexture, &search1, 1);
                }
        }
    }
    printf("Ouch\n");
    destroy_window(disp.renderer, disp.window);

    return 0;
}
