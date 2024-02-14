#include <iostream>
#include "display.h"
#include <SDL2/SDL.h>
#include "search.h"
#include <unistd.h> // usleep()
#include <thread>

extern int doneSearching;
int isSearching = 0;

// Function to be launched in another thread
// Only used to listed for a key and stop the searches mid way through
void waitForSearch() {
    SDL_Event event2;

    // the searches will set the doneSearching to be 1
    // so we don't need to worry about breaking or anything
    while (isSearching && !doneSearching) {
        while (SDL_PollEvent(&event2)) {
            switch (event2.type) {
                case SDL_KEYDOWN:
                    if (event2.key.keysym.sym == SDLK_h) {
                        // If the 'h' button is pressed, stop the search
                        isSearching = 0;
                    }
            }
        }
    }
    return;
}

void manyWalls(SDL_Renderer* r, search* search1, int shouldDelete) {
    int mouse_x, mouse_y;
    int index;
    SDL_Event event2;
    tile closest;

    while (SDL_WaitEvent(&event2)) {
        switch (event2.type) {
        case SDL_MOUSEBUTTONUP:
            // duplicated code so if a user clicks without moving the cursor, we will still do the action
            SDL_GetMouseState(&mouse_x, &mouse_y);

            if (isInMenu(mouse_y)) {
                continue;
            }

            closest = getClosestTile(mouse_x, mouse_y);

            index = closest.xIndex + (closest.yIndex * setting.numTiles);

            if (shouldDelete) {
                // make it an empty space
                colorTile(r, closest.x, closest.y, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);

                search1->states[index] = EMPTY_SPACE;
            } else {
                // make it a wall
                search1->states[index] = WALL;

                colorTile(r, closest.x, closest.y, 0, 0, 255);
            }

            if (search1->goal == index) {
                search1->goal = EMPTY_SPACE;
            }

            if (search1->start == index) {
                search1->start = EMPTY_SPACE;
            }

            return;
        case SDL_MOUSEMOTION:
            SDL_GetMouseState(&mouse_x, &mouse_y);

            if (isInMenu(mouse_y)) {
                continue;
            }

            closest = getClosestTile(mouse_x, mouse_y);

            index = closest.xIndex + (closest.yIndex * setting.numTiles);

            if (index < 0) {
                return;
            }

            if (shouldDelete) {
                // make it an empty space
                colorTile(r, closest.x, closest.y, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);

                search1->states[index] = EMPTY_SPACE;
            } else {
                // make it a wall
                search1->states[index] = WALL;

                colorTile(r, closest.x, closest.y, 0, 0, 255);
                // printf("heuristic: %d\n", 1);
            }

            if (search1->goal == index) {
                search1->goal = EMPTY_SPACE;
            }

            if (search1->start == index) {
                search1->start = EMPTY_SPACE;
            }

            continue;
        }
    }
}

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
    reset(disp.renderer, states);

    search search1 = getDefaultSearch();
    search1.stateSize = (setting.numTiles * setting.numTiles);
    search1.numTiles = setting.numTiles;

    search1.states = states;

    color textColor{0, 0, 0};

    color backgroundColor{125, 125, 125};

    tile closest;
    while (isSearching || (SDL_WaitEvent(&event) && !should_quit)) {
        switch (event.type) {
            case SDL_QUIT:
                should_quit = 1;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        should_quit = 1;
                        break;
                    case SDLK_r:
                        clearTiles(disp.renderer, &search1, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);
                    case SDLK_c:
                        // clear the screen
                        // draws grid, resets states array to be all empty, and draw state buttons
                        reset(disp.renderer, states);
                        continue;
                    case SDLK_s:
                        // color the button so the user knows
                        backgroundColor = {100, 200, 100};

                        drawStartButton(disp.renderer, textColor, backgroundColor);
                        selectStartState(disp.renderer, &search1);

                        backgroundColor = {125, 125, 125};

                        drawStartButton(disp.renderer, textColor, backgroundColor);
                        continue;
                    case SDLK_g:
                        // color the button so the user knows
                        backgroundColor = {200, 100, 100};

                        drawGoalButton(disp.renderer, textColor, backgroundColor);
                        selectGoalState(disp.renderer, &search1);

                        backgroundColor = {125, 125, 125};

                        drawGoalButton(disp.renderer, textColor, backgroundColor);
                        continue;
                }

                if (event.key.keysym.sym == SDLK_b || event.key.keysym.sym == SDLK_u || event.key.keysym.sym == SDLK_j || event.key.keysym.sym == SDLK_k) {
                    char text[] = "Searching!";

                    draw_text(disp.renderer, text, 0, 0, 100, setting.menuHeight, textColor, backgroundColor);

                    isSearching = 1;
                    doneSearching = 0;

                    // this thread will set isSearching to 0 (false) if a key is pressed
                    // this stops the search from continuing
                    std::thread t1(waitForSearch);

                    // do the search
                    if (event.key.keysym.sym == SDLK_b) {
                        bfs(disp.renderer, &search1);
                    } else if (event.key.keysym.sym == SDLK_u) {
                        dfs(disp.renderer, &search1);
                    } else if (event.key.keysym.sym == SDLK_k) {
                        greedy(disp.renderer, &search1);
                    } else {
                        astar(disp.renderer, &search1);
                    }

                    isSearching = 0;

                    // make sure that the thread finished
                    t1.join();

                    char doneText[] = "Done!";
                    draw_text(disp.renderer, doneText, 0, 0, 100, setting.menuHeight, textColor, backgroundColor);
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

                    usleep(5000);
                    reset(disp.renderer, states);
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
                    manyWalls(disp.renderer, &search1, 0);
                }

                // make the space a wall
                if( event.button.button == SDL_BUTTON_RIGHT ) {
                    manyWalls(disp.renderer, &search1, 1);
                }
        }
    }
    printf("Ouch\n");
    destroy_window(disp.renderer, disp.window);

    return 0;
}
