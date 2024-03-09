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

int main(int argc, char* argv[]) {
    screen disp = init_display();

    SDL_Event event;

    int should_quit = 0;
    settings setting = getDisplaySettings();

    // create the list of all the tiles on the grid
    int* states;
    states = (int*) malloc((setting.numTiles * setting.numTiles) * sizeof(int));

    // draws grid, resets states array to be all empty, and draw state buttons
    reset(disp.renderer, disp.texture1, states);

    // draw the start and goal buttons
    drawStatusBar(disp.renderer, disp.statusTexture, textColor, backgroundColor);

    usleep(5000);

    testTexture(disp.renderer, disp.texture2, states);

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
                        {
                        printf("Saving?\n");
                          // Read the pixels from the current render target and save them onto the surface
                          SDL_RenderReadPixels(disp.renderer, NULL, SDL_GetWindowPixelFormat(disp.window), disp.surface->pixels, disp.surface->pitch);

                          char str[3];
                          char name[12] = "-result.bmp";
                          snprintf( str, 3, "%d", setting.numTiles );

                          char *result = (char*) malloc(strlen(str) + strlen(name) + 1);
                          if (!result) {
                              printf("Couldn't Malloc!\n");
                              continue;
                          }

                          strcpy(result, str);
                          strcat(result, name);

                          SDL_SaveBMP(disp.surface, result);
                        continue;
                        }
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
                    case SDLK_z:
                        {
                            if (argc <= 1) {
                                printf("No filename passed in\n");
                                continue;
                            }
                            int size = 2;
                            int iterator = 0;

                            char* name = argv[1];

                            char* tiles = (char*) malloc(6 * sizeof(char));
                            for (int i = 0; i < 6; i++) {
                                tiles[i] = '\0';
                            }

                            while (name[iterator] != '\0' && isdigit(name[iterator])) {
                                tiles[iterator] = name[iterator];
                                iterator++;
                            }

                            int number = atoi(tiles);

                        SDL_Surface* abc = SDL_LoadBMP(name);

                        if ( !abc ) {
                            // load failed
                            printf("Couldn't open image!\n");
                            printf("%s\n", SDL_GetError());
                            continue;
                        }

                        setting.numTiles = number;
                        setDisplaySettings(setting);

                        resizeGridLayout();

                        states = (int*) realloc(states, (setting.numTiles * setting.numTiles) * sizeof(int));

                        search1 = getDefaultSearch();
                        search1.states = states;
                        search1.stateSize = (setting.numTiles * setting.numTiles);
                        search1.numTiles = setting.numTiles;
                        disp.currTexture = disp.texture1;

                        usleep(5000);
                        reset(disp.renderer, disp.currTexture, states);

                        // now you can convert it into a texture
                        SDL_Texture* tmp = SDL_CreateTextureFromSurface(disp.renderer, abc);

                        SDL_SetRenderTarget(disp.renderer, disp.currTexture);

                        // Render the full original texture onto the new one
                        SDL_RenderCopy(disp.renderer, tmp, NULL, NULL);

                        // Reset the rendering target to the default (the window)
                        SDL_SetRenderTarget(disp.renderer, NULL);

                        usleep(5000);
                        SDL_RenderPresent(disp.renderer);
                        ColorBlankTile(disp.renderer, disp.currTexture);
                        continue;
                        }
                    case SDLK_t:
                        // clears all tiles and redraws onto selected buffer
                        clearTilesFromTexture(disp.renderer, disp.backTexture, &search1, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);
                        continue;
                    case SDLK_r:
                        // much faster way of clearing the screen
                        clearTilesBulk(disp.renderer, disp.currTexture, &search1, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);

                        drawGoalButton(disp.renderer, disp.statusTexture, textColor, backgroundColor);
                        drawStartButton(disp.renderer, disp.statusTexture, textColor, backgroundColor);

                        continue;
                    case SDLK_c:
                        // clear the screen
                        // draws grid, resets states array to be all empty, and draw state buttons
                        drawStatusBar(disp.renderer, disp.statusTexture, textColor, backgroundColor);
                        reset(disp.renderer, disp.currTexture, states);
                        search1.goal = EMPTY_SPACE;
                        search1.start = EMPTY_SPACE;

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
                        bfs(disp.renderer, disp.currTexture, disp.statusTexture, &search1);
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
                    // destroy_window(disp.renderer, disp.window);

                    // disp = init_display();
                    resizeGridLayout();

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
                if (isInMenu(mouse_y) || isOutOfGrid(mouse_x, mouse_y)) {
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
