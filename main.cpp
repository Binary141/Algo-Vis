#include <iostream>
#include "display.h"
#include <SDL2/SDL.h>
#include "search.h"
#include <unistd.h> // usleep()
#include <thread>

extern int doneSearching;
int isSearching = 0;

void waitForSearch() {
    SDL_Event event2;
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

    search search1;
    search1.start = EMPTY_SPACE;
    search1.startx = EMPTY_SPACE;
    search1.starty = EMPTY_SPACE;
    search1.goal = EMPTY_SPACE;
    search1.goalx = EMPTY_SPACE;
    search1.goaly = EMPTY_SPACE;
    search1.stateSize = (setting.numTiles * setting.numTiles);
    search1.numTiles = setting.numTiles;

    search1.states = states;

    color textColor{0, 0, 0};

    color backgroundColor{125, 125, 125};

    tile closest;
    while (isSearching || (SDL_WaitEvent(&event) && !should_quit)) {
        while (isSearching) {
            printf("We searching!\n");
        }

        // SDL_BlitSurface(image, NULL, disp.surface, NULL);
        // SDL_UpdateWindowSurface(disp.window);
        switch (event.type) {
            case SDL_QUIT:
                should_quit = 1;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_r) {
                    clearTiles(disp.renderer, &search1, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);
                }
                if (event.key.keysym.sym == SDLK_b) {
                    char text[] = "Searching!";

                    isSearching = 1;
                    doneSearching = 0;

                    // this thread will set isSearching to 0 (false) if a key is pressed
                    // this stops the search from continuing
                    std::thread t1(waitForSearch);

                    // do the search
                    bfs(disp.renderer, &search1);

                    isSearching = 0;

                    // make sure that the thread finished
                    t1.join();

                    char doneText[] = "Done!";
                    draw_text(disp.renderer, doneText, 0, 0, 100, setting.menuHeight, textColor, backgroundColor);
                }
                if (event.key.keysym.sym == SDLK_u) {
                    char text[] = "Searching!";

                    isSearching = 1;
                    doneSearching = 0;

                    // this thread will set isSearching to 0 (false) if a key is pressed
                    // this stops the search from continuing
                    std::thread t1(waitForSearch);

                    // do the search
                    dfs(disp.renderer, &search1);

                    isSearching = 0;

                    // make sure that the thread finished
                    t1.join();

                    char doneText[] = "Done!";
                    draw_text(disp.renderer, doneText, 0, 0, 100, setting.menuHeight, textColor, backgroundColor);
                }
                if (event.key.keysym.sym == SDLK_a) {
                    setting.numTiles += 1;
                    setDisplaySettings(setting);
                    destroy_window(disp.renderer, disp.window);
                    disp = init_display();
                    states = (int*) realloc(states, (setting.numTiles * setting.numTiles) * sizeof(int));

                    search1.states = states;
                    search1.start = EMPTY_SPACE;
                    search1.startx = EMPTY_SPACE;
                    search1.starty = EMPTY_SPACE;
                    search1.goal = EMPTY_SPACE;
                    search1.goalx = EMPTY_SPACE;
                    search1.goaly = EMPTY_SPACE;
                    search1.stateSize = (setting.numTiles * setting.numTiles);
                    search1.numTiles = setting.numTiles;

                    usleep(5000);
                    reset(disp.renderer, states);
                }
                if (event.key.keysym.sym == SDLK_d) {
                    // do nothing if we are going to have less than a 2x2 grid
                    // not sure what you would do with that
                    if (setting.numTiles == 2) {
                        continue;
                    }
                    setting.numTiles -= 1;
                    setDisplaySettings(setting);
                    destroy_window(disp.renderer, disp.window);
                    disp = init_display();
                    states = (int*) realloc(states, (setting.numTiles * setting.numTiles) * sizeof(int));

                    search1.states = states;
                    search1.start = EMPTY_SPACE;
                    search1.startx = EMPTY_SPACE;
                    search1.starty = EMPTY_SPACE;
                    search1.goal = EMPTY_SPACE;
                    search1.goalx = EMPTY_SPACE;
                    search1.goaly = EMPTY_SPACE;
                    search1.stateSize = (setting.numTiles * setting.numTiles);
                    search1.numTiles = setting.numTiles;

                    usleep(5000);
                    reset(disp.renderer, states);
                }
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

                    // for(int i = 0; i < setting.numTiles * setting.numTiles; i++) {
                    //     printf("%d\n", search1.states[i]);
                    // }

                    // printf("goal: %d\n", search1.goal);
                    // printf("start: %d\n", search1.start);

                    continue;
                }

                if (event.key.keysym.sym == SDLK_g) {
                    // color the button so the user knows
                    backgroundColor = {200, 100, 100};

                    drawGoalButton(disp.renderer, textColor, backgroundColor);

                    selectGoalState(disp.renderer, &search1);

                    backgroundColor = {125, 125, 125};

                    drawGoalButton(disp.renderer, textColor, backgroundColor);

                    // for(int i = 0; i < setting.numTiles * setting.numTiles; i++) {
                    //     printf("%d\n", search1.states[i]);
                    // }

                    // printf("goal: %d\n", search1.goal);
                    // printf("start: %d\n", search1.start);

                    continue;
                }
            case SDL_MOUSEBUTTONDOWN:
                // generate a wall
                int mouse_x, mouse_y;
                SDL_GetMouseState(&mouse_x, &mouse_y);

                // don't do anything if it is in the menu bar at the moment
                // adjust here for menu selections
                if (mouse_y <= setting.menuHeight + TILE_BORDER_WIDTH) { // + TILE_BORDER_WIDTH + 1) {
                    // +1 comes from adding a little padding just in case
                    continue;
                }

                closest = getClosestTile(mouse_x, mouse_y);

                // color the tile if it is the left mouse button
                if( event.button.button == SDL_BUTTON_LEFT ) {

                    colorTile(disp.renderer, closest.x, closest.y, 0, 0, 255);
                    int index = closest.xIndex + (closest.yIndex * setting.numTiles);

                    search1.states[index] = WALL;

                    if (search1.goal == index) {
                        search1.goal = EMPTY_SPACE;
                    }

                    if (search1.start == index) {
                        search1.start = EMPTY_SPACE;
                    }
                    continue;
                }
                if( event.button.button == SDL_BUTTON_RIGHT ) {
                    // clear the tile if it is the right mouse button
                    colorTile(disp.renderer, closest.x, closest.y, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);
                    int index = closest.xIndex + (closest.yIndex * setting.numTiles);
                    search1.states[index] = EMPTY_SPACE;

                    if (search1.goal == index) {
                        search1.goal = EMPTY_SPACE;
                    }

                    if (search1.start == index) {
                        search1.start = EMPTY_SPACE;
                    }

                    continue;
                }
        }
    }
    printf("Ouch\n");
    destroy_window(disp.renderer, disp.window);

    return 0;
}
