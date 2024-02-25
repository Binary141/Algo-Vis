#include "display.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <SDL2/SDL_ttf.h>
#include <unistd.h> // usleep()
#include <stdlib.h>

settings setting = {
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    TILE_WIDTH,
    TILE_HEIGHT,
    NUM_SQUARES,
    MENU_HEIGHT,
    0,
    0,
    0,
    0,
    0,
    0
};

struct Font {
    TTF_Font* f;
} font;

settings getDisplaySettings() {
    return setting;
}

void setDisplaySettings(settings set) {
    setting = set;
}

screen init_display() {
    // change the display dimensions if there is any extra space on the
    // bottom or right of the window
    screen ret;

    int tile_width = ((SCREEN_WIDTH - TILE_BORDER_WIDTH) / setting.numTiles);
    setting.tileWidth = tile_width;

    int temp_width = 0;

    // get largest width size
    temp_width = setting.numTiles * tile_width;

    // update the display width. Add the tile border width to allow the drawing of the right most border
    setting.width = temp_width + TILE_BORDER_WIDTH;

    int calculated_height = (SCREEN_HEIGHT - setting.menuHeight - TILE_BORDER_WIDTH) / setting.numTiles;
    setting.tileHeight = calculated_height;

    // update the display height. Add in the tile boarder width to allow the drawing of the bottom most border
    setting.height = (calculated_height * setting.numTiles) + setting.menuHeight + TILE_BORDER_WIDTH;

    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not be initialized!: %s\n", SDL_GetError());
        exit(1);
    }

    // Create window
    SDL_Window* window = SDL_CreateWindow("Algo-Visualizer",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          setting.width, setting.height,
                                          SDL_WINDOW_SHOWN);
    ret.window = window;
    if(!window) {
        printf("Failed to create window!\n");
        printf("SDL2 Error: %s\n", SDL_GetError());
        exit(1);
    }

    // Create renderer
    // ret.renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    ret.renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!ret.renderer) {
        printf("Failed to get the renderer from the window!\n");
        printf("SDL2 Error: %s\n", SDL_GetError());
        exit(1);
    }

    ret.surface = SDL_GetWindowSurface(window);
    if(!ret.surface) {
        printf("Failed to get the surface from the window!\n");
        printf("SDL2 Error: %s\n", SDL_GetError());
        exit(1);
    }

    ret.texture1 = SDL_CreateTexture(ret.renderer,
                                    SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_TARGET,
                                    setting.width,
                                    setting.height);
    if(!ret.texture1) {
        printf("Failed to create texture1!\n");
        printf("SDL2 Error: %s\n", SDL_GetError());
        exit(1);
    }

    ret.statusTexture = SDL_CreateTexture(ret.renderer,
                                    SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_TARGET,
                                    setting.width,
                                    setting.menuHeight);
    if(!ret.statusTexture) {
        printf("Failed to create statusTexture!\n");
        printf("SDL2 Error: %s\n", SDL_GetError());
        exit(1);
    }

    ret.texture2 = SDL_CreateTexture(ret.renderer,
                                    SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_TARGET,
                                    setting.width,
                                    setting.height);
    if(!ret.texture2) {
        printf("Failed to create texture2!\n");
        printf("SDL2 Error: %s\n", SDL_GetError());
        exit(1);
    }

    ret.backTexture = SDL_CreateTexture(ret.renderer,
                                    SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_TARGET,
                                    setting.width,
                                    setting.height);
    if(!ret.backTexture) {
        printf("Failed to create backTexture!\n");
        printf("SDL2 Error: %s\n", SDL_GetError());
        exit(1);
    }

    // position buttons on the far right of display
    int offset = setting.width * 0.05;
    setting.startButtonX = setting.width - offset;
    setting.startButtonX2 = setting.width;

    setting.startButtonY = 0;

    setting.goalButtonX = setting.width - (2 * offset);
    setting.goalButtonX2 = setting.width - offset;
    setting.goalButtonY = 0;

    TTF_Init();

    font.f = TTF_OpenFont("./OpenSans-Regular.ttf", FONT_SIZE);
    if (!font.f) {
        printf("Error opening font!\n");
        printf("%s\n", TTF_GetError());
        exit(1);
    }

    return ret;
}

void draw_grid(SDL_Renderer* renderer, SDL_Texture* texture, int shouldRender) {

    // anything drawn to renderer will be drawn to the texture
    SDL_SetRenderTarget(renderer, texture);

    // Initialize renderer color white for the background
    SDL_SetRenderDrawColor(renderer, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 255);

    // Clear screen
    SDL_RenderClear(renderer);

    // Actually draw the desired color
    SDL_SetRenderDrawColor(renderer, BORDER_R, BORDER_G, BORDER_B, 255);

    // Declare rect of square
    SDL_Rect squareRect;

    squareRect.w = TILE_BORDER_WIDTH;
    squareRect.h = setting.height;

    squareRect.y = setting.menuHeight;

    // draw vertical lines
    for (int i = 0; i <= setting.width; i += setting.tileWidth) {
        squareRect.x = i;

        // Draw it
        SDL_RenderFillRect(renderer, &squareRect);
    }

    squareRect.w = setting.width;
    squareRect.h = TILE_BORDER_WIDTH;

    // draw in the middle of the screen
    squareRect.x = (setting.width / 2) - (squareRect.w / 2);

    // draw horizontal lines
    for (int i = setting.menuHeight; i <= setting.height; i += setting.tileHeight) {
        squareRect.y = i;

        // Draw it
        SDL_RenderFillRect(renderer, &squareRect);
    }

    // Reset the rendering target to the default (the window)
    SDL_SetRenderTarget(renderer, nullptr);

    // Clear the renderer
    SDL_SetRenderDrawColor(renderer, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 255);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, texture, NULL, NULL);

    // Update screen
    if (shouldRender) {
        SDL_RenderPresent(renderer);
    }
}

void testTexture(SDL_Renderer* renderer, SDL_Texture* texture, int* states) {
    int shouldRender = 1;
    draw_grid(renderer, texture, shouldRender);

    usleep(SLEEPTIME2);
}

void drawStatusBar(SDL_Renderer* r, SDL_Texture* t, color txtColor, color bgColor) {

    // anything drawn to renderer will be drawn to the texture
    SDL_SetRenderTarget(r, t);

    // Declare rect of square
    SDL_Rect squareRect;

    // Actually draw the desired color
    SDL_SetRenderDrawColor(r, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 255);

    // Square dimensions
    squareRect.w = setting.width;
    squareRect.h = MENU_HEIGHT;

    squareRect.y = 0;
    squareRect.x = 0;

    // Draw it
    SDL_RenderFillRect(r, &squareRect);

    // Reset the rendering target to the default (the window)
    SDL_SetRenderTarget(r, NULL);

    SDL_RenderCopy(r, t, NULL, NULL);

    drawStartButton(r, t, txtColor, bgColor);
    drawGoalButton(r, t, txtColor, bgColor);
}

void reset(SDL_Renderer* r, SDL_Texture* t, int* states) {
    int shouldRender = 1;

    draw_grid(r, t, shouldRender);
    usleep(SLEEPTIME2);

    // set all states to be open
    for(int i = 0; i < setting.numTiles * setting.numTiles; i++) {
        states[i] = EMPTY_SPACE;
    }

    usleep(SLEEPTIME2);
}

void draw_text(SDL_Renderer* renderer, SDL_Texture* texture, char* text, int x, int y, int width, int height, color txtColor, color bgColor) {

    // anything drawn to renderer will be drawn to the texture
    SDL_SetRenderTarget(renderer, texture);

    // Clear the renderer
    SDL_SetRenderDrawColor(renderer, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 255);

    SDL_Color textColorSDL = {txtColor.r, txtColor.g, txtColor.b};

    SDL_Surface* surfaceMessage =
        TTF_RenderText_Solid(font.f, text, textColorSDL);

    // now you can convert it into a texture
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect Message_rect; //create a rect
    Message_rect.x = x;  //controls the rect's x coordinate
    Message_rect.y = y; // controls the rect's y coordinate
    Message_rect.w = width; // controls the width of the rect
    Message_rect.h = height; // controls the height of the rect

    // draw the background of the text box
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, 255);

    // Draw it
    SDL_RenderFillRect(renderer, &Message_rect);

    // Actually draw the desired color
    SDL_SetRenderDrawColor(renderer, BORDER_R, BORDER_G, BORDER_B, 255);

    SDL_SetRenderTarget(renderer, NULL);

    SDL_Rect srcrect;
    srcrect.x = x;
    srcrect.y = y;
    srcrect.w = width;
    srcrect.h = height;

    SDL_RenderCopy(renderer, texture, &srcrect, &srcrect);

    // SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);

    // Update screen
    SDL_RenderPresent(renderer);
}

void destroy_window(SDL_Renderer* renderer, SDL_Window* window) {
    // Destroy renderer
    SDL_DestroyRenderer(renderer);

    // Destroy window
    SDL_DestroyWindow(window);

    // Quit SDL
    SDL_Quit();
}

tile getClosestTile(int x, int y) {
    tile closest;

    int closest_x = TILE_BORDER_WIDTH;
    while ( closest_x < x ) {
        closest_x += setting.tileWidth;
    }
    // subtract the setting.tileWidth since we over shot it
    closest_x -= setting.tileWidth;

    // find out how many tiles it takes to subtract from the y position before it is negative
    int closest_y = y - TILE_BORDER_WIDTH - setting.menuHeight;

    // this shouldn't happen. But if it does we should quit. Undefined behaviour if we were to do something in the menu relating to tiles
    if (closest_y < 0) {
        printf("closest_y was less than 0!\n");
        exit(1);
    }
    int numTilesAway = 0;

    while (closest_y > 0) {
        closest_y -= setting.tileHeight;
        numTilesAway += 1;
    }
    numTilesAway -= 1;

    closest.x = closest_x;
    closest.y = setting.menuHeight + (numTilesAway * setting.tileHeight) + TILE_BORDER_WIDTH;
    closest.xIndex = closest_x / setting.tileWidth;
    closest.yIndex = numTilesAway;

    return closest;
}

void colorTile(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int r, int g, int b, int shouldRender) {

    // anything drawn to renderer will be drawn to the texture
    SDL_SetRenderTarget(renderer, texture);

    // Declare rect of square
    SDL_Rect squareRect;

    // Actually draw the desired color
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);

    // Square dimensions: Half of the min(SCREEN_WIDTH, SCREEN_HEIGHT)
    squareRect.w = setting.tileWidth - TILE_BORDER_WIDTH;
    squareRect.h = setting.tileHeight - TILE_BORDER_WIDTH;

    squareRect.y = y;
    squareRect.x = x;

    // Draw it
    SDL_RenderFillRect(renderer, &squareRect);

    // Reset the rendering target to the default (the window)
    SDL_SetRenderTarget(renderer, nullptr);

    // Update screen if needed
    if (shouldRender) {
        // don't try to update the status bar portion of the display
        SDL_Rect srcrect;
        srcrect.w = setting.width;
        srcrect.h = setting.height - setting.menuHeight - TILE_BORDER_WIDTH;
        srcrect.x = 0;
        srcrect.y = setting.menuHeight + TILE_BORDER_WIDTH;

        SDL_RenderCopy(renderer, texture, &srcrect, &srcrect);

        SDL_RenderPresent(renderer);
    }
}

void colorTileByIndex(SDL_Renderer* renderer, SDL_Texture* texture, int index, int r, int g, int b, int shouldUpdate) {
    int y = index / setting.numTiles;
    int x = index % setting.numTiles;
    colorTile(renderer, texture, ((x * setting.tileWidth) + TILE_BORDER_WIDTH), (((y * setting.tileHeight) + setting.menuHeight + TILE_BORDER_WIDTH)), r, g, b, shouldUpdate);

    usleep(SLEEPTIME2);
}

void selectGoalState(SDL_Renderer* renderer, SDL_Texture* texture, search* s) {
    SDL_Event event;
    tile closest;
    int new_goal;

    while (SDL_WaitEvent(&event)) {
        switch (event.type) {
            case SDL_MOUSEMOTION:
                // used for mouse movement
                continue;
            case SDL_MOUSEBUTTONDOWN:
                int mouse_x, mouse_y;
                SDL_GetMouseState(&mouse_x, &mouse_y);
                if (isInMenu(mouse_y)) {
                    // don't do anything if it is in the menu bar. That will be handled elsewhere
                    return;
                }

                closest = getClosestTile(mouse_x, mouse_y);
                new_goal = (closest.xIndex + (closest.yIndex * setting.numTiles));

                if (s->start == new_goal) {
                    // remove what used to be the start state
                    s->start = EMPTY_SPACE;
                }

                if (s->goal == EMPTY_SPACE) {
                    s->goalx = closest.x;
                    s->goaly = closest.y;
                    s->goal = new_goal;

                    // marks the new goal in the array
                    s->states[new_goal] = GOAL;
                    colorTile(renderer, texture, s->goalx, s->goaly, GOAL_COLOR.r, GOAL_COLOR.g, GOAL_COLOR.b, 1);
                    return;
                }

                // reset the original goal state tile to be the background color
                colorTile(renderer, texture, s->goalx, s->goaly, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 1);

                s->goalx = closest.x;
                s->goaly = closest.y;

                // reset the old goal to be a blank space
                s->states[s->goal] = EMPTY_SPACE;

                // make the new index a goal
                s->states[new_goal] = GOAL;
                s->goal = new_goal;

                // TODO look into why display blanks without this. Fast enough it doesn't really matter?
                usleep(SLEEPTIME2);
                // Color the new goal state
                colorTileByIndex(renderer, texture, s->goal, GOAL_COLOR.r, GOAL_COLOR.g, GOAL_COLOR.b, 1);

                return;
            case SDL_KEYDOWN:
                // if the user clicks away or anything, don't do anything
                return;
        }
    }
    return;
}

void selectStartState(SDL_Renderer* renderer, SDL_Texture* texture, search* s) {
    SDL_Event event;
    tile closest;
    int new_start;

    while (SDL_WaitEvent(&event)) {
        switch (event.type) {
            case SDL_MOUSEMOTION:
                // used for mouse movement
                continue;
            case SDL_MOUSEBUTTONDOWN:
                int mouse_x, mouse_y;
                SDL_GetMouseState(&mouse_x, &mouse_y);

                if (isInMenu(mouse_y)) {
                    // don't do anything if it is in the menu bar. That will be handled elsewhere
                    return;
                }

                closest = getClosestTile(mouse_x, mouse_y);
                new_start = (closest.xIndex + (closest.yIndex * setting.numTiles));

                if (s->goal == new_start) {
                    // remove what used to be the goal state
                    s->goal = EMPTY_SPACE;
                }

                if (s->start == EMPTY_SPACE) {
                    s->startx = closest.x;
                    s->starty = closest.y;
                    s->start = new_start;

                    // marks the new start in the array
                    s->states[new_start] = START;
                    colorTile(renderer, texture, s->startx, s->starty, START_COLOR.r, START_COLOR.g, START_COLOR.b, 1);
                    return;
                }

                // reset the original start state tile to be the background color
                colorTile(renderer, texture, s->startx, s->starty, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 1);

                s->startx = closest.x;
                s->starty = closest.y;

                // reset the old start to be a blank space
                s->states[s->start] = EMPTY_SPACE;

                // make the new index a start
                s->states[new_start] = START;
                s->start = new_start;

                // TODO look into why display blanks without this. Fast enough it doesn't really matter?
                usleep(SLEEPTIME2);
                // Color the new start state
                colorTileByIndex(renderer, texture, s->start, START_COLOR.r, START_COLOR.g, START_COLOR.b, 1);

                return;
            case SDL_KEYDOWN:
                // if the user clicks away or anything, don't do anything
                return;
        }
    }
    return;
}

void drawStartButton(SDL_Renderer* renderer, SDL_Texture* texture, color txtColor, color bgColor) {
    char start[] = "START";
    int width = setting.startButtonX2 - setting.startButtonX;

    draw_text(renderer, texture, start, setting.startButtonX, setting.startButtonY, width, setting.menuHeight, txtColor, bgColor);
}

void drawStatesCount(SDL_Renderer* r, SDL_Texture* t, color txtColor, color bgColor, int value) {
    char str[5];
    sprintf(str, "%d", value);

    draw_text(r, t, str, 125, 0, 40, setting.menuHeight, txtColor, bgColor);
}

void drawGoalButton(SDL_Renderer* renderer, SDL_Texture* texture, color txtColor, color bgColor) {
    char goal[] = "GOAL";
    int width = setting.goalButtonX2 - setting.goalButtonX;

    draw_text(renderer, texture, goal, setting.goalButtonX, setting.goalButtonY, width, setting.menuHeight, txtColor, bgColor);
}

int isInMenu(int y) {
    if (y <= setting.menuHeight + TILE_BORDER_WIDTH) {
        // if it is within y 0 to the height of the menu, return true
        return 1;
    }
    return 0;
}

void manyWalls(SDL_Renderer* r, SDL_Texture* t, search* search1, int shouldDelete) {
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
                colorTile(r, t, closest.x, closest.y, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 1);

                search1->states[index] = EMPTY_SPACE;
            } else {
                // make it a wall
                search1->states[index] = WALL;

                colorTile(r, t, closest.x, closest.y, 0, 0, 255, 1);
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
                colorTile(r, t, closest.x, closest.y, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 1);

                search1->states[index] = EMPTY_SPACE;
            } else {
                // make it a wall
                search1->states[index] = WALL;

                colorTile(r, t, closest.x, closest.y, 0, 0, 255, 1);
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
