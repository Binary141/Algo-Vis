#include "display.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "search.h"
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <unistd.h> // usleep()
#include <thread>

extern
int doneSearching;
int isSearching = 0;

search
getDefaultSearch()
{
    search search1;
    search1.start = EMPTY_SPACE;
    search1.startx = EMPTY_SPACE;
    search1.starty = EMPTY_SPACE;
    search1.goal = EMPTY_SPACE;
    search1.goalx = EMPTY_SPACE;
    search1.goaly = EMPTY_SPACE;
    search1.heuristic = SLDHEURISTIC;
    search1.selectedHeuristic = SLDHEURISTIC;
    return search1;
}

int
main(int argc, char* argv[]) 
{
    screen disp = init_display();

    search search1 = getDefaultSearch();
    search1.stateSize = (setting.numTiles * setting.numTiles);
    search1.numTiles = setting.numTiles;

    SDL_Event event;

    int should_quit = 0;
    int should_change = 0;
    settings setting = getDisplaySettings();

    // create the list of all the tiles on the grid
    int* states;
    states = (int*) malloc((setting.numTiles * setting.numTiles) * sizeof(int));

    drawMenu(disp.renderer, disp.menuTexture, setting.width, setting.height, search1.heuristic);

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

    search1.states = states;

    tile closest;
    int indicator = 0;

    SDL_Rect texr;
    texr.x = 0;
    texr.y = 0;
    texr.w = 100;
    texr.h = 200;

    SDL_Surface* textureSurface;
    int hasRan = 0;
    int menuDisplayed = 0;

    while (isSearching || (SDL_WaitEvent(&event) && !should_quit)) {
        switch (event.type) {
            case SDL_QUIT:
                should_quit = 1;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_m:
                        {
                        if (menuDisplayed) {
                            // don't do anything if the menu is being displayed
                            continue;
                        }
                        printf("Saving!\n");
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
                    case SDLK_o:
                        if (menuDisplayed) {
                            // don't do anything if the menu is being displayed
                            continue;
                        }
                        // demo to swap the textures

                        disp.backTexture = disp.currTexture;

                        // alternate the current
                        if (!indicator) {
                            disp.currTexture = disp.texture2;
                            indicator = 1;
                        } else {
                            disp.currTexture = disp.texture1;
                            indicator = 0;
                        }

                        // clear out the menu
                        SDL_RenderCopy(disp.renderer,
                                       disp.currTexture,
                                       NULL,
                                       NULL);

                        SDL_RenderPresent(disp.renderer);
                        ColorBlankTile(disp.renderer, disp.currTexture);

                        continue;
                    case SDLK_MINUS:
                        if (!menuDisplayed) {
                            continue;
                        }

                        setting.selectedResolution = setting.selectedResolution - 1;
                        if (setting.selectedResolution < 0)
                            setting.selectedResolution = 4;


                        drawMenu(disp.renderer, 
                                disp.menuTexture, 
                                supportedWidths[setting.selectedResolution], 
                                supportedHeights[setting.selectedResolution], 
                                search1.heuristic);
                        showMenu(disp.renderer, disp.menuTexture, setting.width, setting.height);
                        continue;
                    case SDLK_EQUALS:
                        if (!menuDisplayed) {
                            continue;
                        }

                        setting.selectedResolution = (setting.selectedResolution + 1) % 5;

                        drawMenu(disp.renderer, 
                                disp.menuTexture, 
                                supportedWidths[setting.selectedResolution], 
                                supportedHeights[setting.selectedResolution], 
                                search1.heuristic);
                        showMenu(disp.renderer, disp.menuTexture, setting.width, setting.height);
                        continue;
                    case SDLK_RIGHTBRACKET:
                        if (!menuDisplayed) {
                            continue;
                        }

                        search1.selectedHeuristic = (search1.selectedHeuristic + 1) % 3;

                        drawMenu(disp.renderer, 
                                disp.menuTexture,
                                supportedWidths[setting.selectedResolution],
                                supportedHeights[setting.selectedResolution],
                                search1.selectedHeuristic);
                        showMenu(disp.renderer, disp.menuTexture, setting.width, setting.height);
                        continue;
                    case SDLK_LEFTBRACKET:
                        if (!menuDisplayed) {
                            continue;
                        }

                        search1.selectedHeuristic = (search1.selectedHeuristic - 1);
                        if (search1.selectedHeuristic < 0)
                            search1.selectedHeuristic = 2;

                        drawMenu(disp.renderer, 
                                disp.menuTexture,
                                supportedWidths[setting.selectedResolution],
                                supportedHeights[setting.selectedResolution],
                                search1.selectedHeuristic);
                        showMenu(disp.renderer, disp.menuTexture, setting.width, setting.height);
                        continue;
                    case SDLK_ESCAPE:
                        if (!menuDisplayed) {
                            continue;
                        }
                        should_change = 0;

                        // reset the choice
                        setting.selectedResolution = setting.currentResolution;
                    case SDLK_p:

                        drawMenu(disp.renderer, disp.menuTexture, setting.width, setting.height, search1.heuristic);
                        // drawMenu(disp.renderer, disp.currTexture, setting.width, setting.height);
                        usleep(5000);

                        // Declare rect of square
                        SDL_Rect squareRect;
                        // Square dimensions
                        // squareRect.w = setting.width / 2;
                        // squareRect.h = setting.height / 1.2;

                        // squareRect.y = (setting.height / 2) - (squareRect.h / 2);
                        // squareRect.x = (setting.width / 2) - (squareRect.w / 2);

                        // if the menu is not showing, show it
                        if (!menuDisplayed) {
                            menuDisplayed = 1;

                            // SDL_RenderCopy(disp.renderer,
                            //                disp.statusTexture,
                            //                NULL,
                            //                NULL);

                            // SDL_RenderCopy(disp.renderer,
                            //                disp.currTexture,
                            //                NULL,
                            //                NULL);

                            // SDL_RenderCopy(disp.renderer,
                                           // disp.menuTexture,
                                           // &squareRect,
                                           // &squareRect);

                            // SDL_RenderPresent(disp.renderer);
                            showMenu(disp.renderer, disp.menuTexture, setting.width, setting.height);

                            continue;
                        }
                        if (search1.selectedHeuristic != search1.heuristic) {
                            search1.heuristic = search1.selectedHeuristic;
                        }

                        // don't update anything if nothing changed
                        if (setting.selectedResolution == setting.currentResolution) {
                            squareRect.w = setting.width;
                            squareRect.h = setting.statusHeight + TILE_BORDER_WIDTH;

                            squareRect.y = 0;
                            squareRect.x = 0;

                            menuDisplayed = 0;

                            drawStatusBar(disp.renderer, disp.statusTexture, textColor, backgroundColor);

                            SDL_SetRenderTarget(disp.renderer, disp.currTexture);

                            // clear out the menu
                            SDL_RenderCopy(disp.renderer,
                                           disp.currTexture,
                                           NULL,
                                           NULL);

                            // SDL_RenderCopy(disp.renderer,
                            //                disp.statusTexture,
                            //                NULL,
                            //                NULL);

                            SDL_RenderPresent(disp.renderer);

                            // drawStatusBar(disp.renderer, disp.statusTexture, backgroundColor, textColor);

                            ColorBlankTile(disp.renderer, disp.currTexture);
                            continue;
                        }

                        setting.width = supportedWidths[setting.selectedResolution];
                        setting.height = supportedHeights[setting.selectedResolution];

                        // setting.selectedResolution = (setting.selectedResolution + 1) % 5; // cycle through the list
                        setting.currentResolution = setting.selectedResolution; // cycle through the list

                        // drawMenu(disp.renderer, disp.menuTexture, setting.width, setting.height);

                        menuDisplayed = 0;

                        should_change = 0;

                        setDisplaySettings(setting);

                        // destroy everything for good measure
                        destroy_window(disp.renderer, disp.window);
                        SDL_DestroyRenderer(disp.renderer);
                        SDL_DestroyTexture(disp.currTexture);
                        SDL_DestroyTexture(disp.backTexture);
                        SDL_DestroyTexture(disp.menuTexture);
                        SDL_DestroyTexture(disp.statusTexture);

                        disp = init_display();

                        states = (int*) realloc(states, (setting.numTiles * setting.numTiles) * sizeof(int));

                        search1.states = states;

                        usleep(5000);

                        disp.currTexture = disp.texture1;

                        reset(disp.renderer, disp.currTexture, states);

                        usleep(5000);

                        continue;
                    case SDLK_q:
                        should_quit = 1;
                        break;
                    case SDLK_z:
                        if (menuDisplayed) {
                            // don't do anything if the menu is being displayed
                            continue;
                        }
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
                        // get the display to render. Doesn't load anything without this
                        ColorBlankTile(disp.renderer, disp.currTexture);
                        continue;
                        }
                    case SDLK_t:
                        if (menuDisplayed) {
                            // don't do anything if the menu is being displayed
                            continue;
                        }
                        // clears all tiles and redraws onto selected buffer
                        clearTilesFromTexture(disp.renderer, disp.backTexture, &search1);
                        continue;
                    case SDLK_r:
                        if (menuDisplayed) {
                            // don't do anything if the menu is being displayed
                            continue;
                        }
                        // much faster way of clearing the screen
                        clearTilesBulk(disp.renderer, disp.currTexture, &search1);

                        drawGoalButton(disp.renderer, disp.statusTexture, textColor, backgroundColor, 1);
                        drawStartButton(disp.renderer, disp.statusTexture, textColor, backgroundColor, 1);

                        continue;
                    case SDLK_c:
                        if (menuDisplayed) {
                            // don't do anything if the menu is being displayed
                            continue;
                        }
                        // clear the screen
                        // draws grid, resets states array to be all empty, and draw state buttons
                        drawStatusBar(disp.renderer, disp.statusTexture, textColor, backgroundColor);
                        reset(disp.renderer, disp.currTexture, states);
                        search1.goal = EMPTY_SPACE;
                        search1.start = EMPTY_SPACE;

                        continue;
                    case SDLK_s:
                        if (menuDisplayed) {
                            // don't do anything if the menu is being displayed
                            continue;
                        }
                        // color the button so the user knows

                        drawStartButton(disp.renderer, disp.statusTexture, textColor, color{100, 200, 100}, 1);
                        selectStartState(disp.renderer, disp.currTexture, &search1);

                        drawStartButton(disp.renderer, disp.statusTexture, textColor, backgroundColor, 1);

                        continue;
                    case SDLK_g:
                        if (menuDisplayed) {
                            // don't do anything if the menu is being displayed
                            continue;
                        }
                        // color the button so the user knows

                        drawGoalButton(disp.renderer, disp.statusTexture, textColor, color{200, 100, 100}, 1);
                        selectGoalState(disp.renderer, disp.currTexture, &search1);

                        drawGoalButton(disp.renderer, disp.statusTexture, textColor, backgroundColor, 1);
                        continue;
                }

                if (event.key.keysym.sym == SDLK_b || event.key.keysym.sym == SDLK_u || event.key.keysym.sym == SDLK_j || event.key.keysym.sym == SDLK_k) {
                    if (menuDisplayed) {
                        // don't do anything if the menu is being displayed
                        continue;
                    }
                    char text[] = "Searching!";

                    draw_text(disp.renderer, 
                              disp.statusTexture, 
                              text, 
                              0, // x
                              0, // y
                              100, // width
                              setting.statusHeight, 
                              textColor, 
                              backgroundColor,
                              1,
                              1
                              );

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
                    draw_text(disp.renderer, 
                              disp.statusTexture, 
                              doneText,
                              0, 
                              0, 
                              100, 
                              setting.statusHeight, 
                              textColor, 
                              backgroundColor,
                              1,
                              1
                              );
                }

                if (event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_d) {
                    if (menuDisplayed) {
                        // don't do anything if the menu is being displayed
                        continue;
                    }
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
                if (menuDisplayed) {
                    // don't do anything if the menu is being displayed
                    continue;
                }
                // generate a wall, or clear out the space

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

    destroy_window(disp.renderer, disp.window);

    return 0;
}
