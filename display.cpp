#include "display.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <SDL2/SDL_ttf.h>
#include <unistd.h> // usleep()

settings setting;

settings getDisplaySettings() {
    return setting;
}


screen init_display() {
    // change the display dimensions if there is any extra space on the
    // bottom or right of the window
    screen ret;

    int calculated_width = (NUM_SQUARES * TILE_WIDTH) + TILE_BORDER_WIDTH;

    if (calculated_width != SCREEN_WIDTH) {
        setting.width = calculated_width;
    }

    int calculated_height = (NUM_SQUARES * TILE_HEIGHT) + TILE_BORDER_WIDTH + MENU_HEIGHT;

    if (calculated_height != SCREEN_HEIGHT) {
        setting.height = calculated_height;
    }

    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not be initialized!: %s\n", SDL_GetError());
        return ret;
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

    // position buttons on the far right of display
    int offset = setting.width * 0.05;
    setting.startButtonX = setting.width - offset; // TILE_WIDTH * (NUM_SQUARES - 1);
    setting.startButtonX2 = setting.width; // TILE_WIDTH * (NUM_SQUARES - 1);

    setting.startButtonY = 0;

    setting.goalButtonX = setting.width - (2 * offset); // TILE_WIDTH * (NUM_SQUARES - 2);
    setting.goalButtonX2 = setting.width - offset; // TILE_WIDTH * (NUM_SQUARES - 2);
    setting.goalButtonY = 0;

    TTF_Init();

    return ret;
}

void draw_grid(SDL_Renderer* renderer) {
    // Declare rect of square
    SDL_Rect squareRect;

    // Initialize renderer color white for the background
    SDL_SetRenderDrawColor(renderer, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 255);

    // Clear screen
    SDL_RenderClear(renderer);

    // Actually draw the desired color
    SDL_SetRenderDrawColor(renderer, BORDER_R, BORDER_G, BORDER_B, 255);

    squareRect.w = TILE_BORDER_WIDTH;
    squareRect.h = setting.height;

    squareRect.y = MENU_HEIGHT; // SCREEN_HEIGHT / 2 - squareRect.h / 2;

    // draw vertical lines
    for (int i = 0; i < setting.width; i += TILE_WIDTH) {
        squareRect.x = i;

        // Draw it
        SDL_RenderFillRect(renderer, &squareRect);
    }

    squareRect.w = setting.width;
    squareRect.h = TILE_BORDER_WIDTH;

    squareRect.x = setting.width / 2 - squareRect.w / 2;
    // draw horizontal lines
    for (int i = MENU_HEIGHT; i < setting.height; i += TILE_HEIGHT) {
        squareRect.y = i;

        // Draw it
        SDL_RenderFillRect(renderer, &squareRect);
    }

    // Update screen
    SDL_RenderPresent(renderer);
}

void reset(SDL_Renderer* renderer, int* states) {
    draw_grid(renderer);

    color textColor;
    textColor.r = 0;
    textColor.g = 0;
    textColor.b = 0;

    color backgroundColor;
    backgroundColor.r = 125;
    backgroundColor.g = 125;
    backgroundColor.b = 125;

    drawStartButton(renderer, textColor, backgroundColor);
    drawGoalButton(renderer, textColor, backgroundColor);

    // set all states to be open
    for(int i = 0; i < NUM_SQUARES * NUM_SQUARES; i++) {
        states[i] = WALL;
    }
}

void draw_text(SDL_Renderer* renderer, char* text, int x, int y, int width, int height, color textColor, color backgroundColor) {
    TTF_Font* Sans = TTF_OpenFont("OpenSans-Regular.ttf", FONT_SIZE);
    if (!Sans) {
        printf("Error opening font!");
        exit(1);
    }

    SDL_Color textColorSDL = {textColor.r, textColor.g, textColor.b};

    SDL_Surface* surfaceMessage =
        TTF_RenderText_Solid(Sans, text, textColorSDL);

    // now you can convert it into a texture
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect Message_rect; //create a rect
    Message_rect.x = x;  //controls the rect's x coordinate
    Message_rect.y = y; // controls the rect's y coordinate
    Message_rect.w = width; // controls the width of the rect
    Message_rect.h = height; // controls the height of the rect


    // draw the background of the start button
    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, 255);

    // Draw it
    SDL_RenderFillRect(renderer, &Message_rect);

    // Actually draw the desired color
    SDL_SetRenderDrawColor(renderer, BORDER_R, BORDER_G, BORDER_B, 255);

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
        closest_x += TILE_WIDTH;
    }
    // subtract the TILE_WIDTH since we over shot it
    closest_x -= TILE_WIDTH;

    int closest_y = TILE_BORDER_WIDTH + MENU_HEIGHT;
    while ( closest_y < y ) {
        closest_y += TILE_HEIGHT;
    }
    // subtract the TILE_HEIGHT since we over shot it
    closest_y -= TILE_HEIGHT;


    closest.x = closest_x;
    closest.y = closest_y;
    closest.xIndex = closest_x / TILE_WIDTH;
    closest.yIndex = closest_y / TILE_HEIGHT;

    return closest;
}

void colorTile(SDL_Renderer* renderer, int x, int y, int r, int g, int b) {
    // Declare rect of square
    SDL_Rect squareRect;

    // Initialize renderer color white for the background
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Actually draw the desired color
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);

    // Square dimensions: Half of the min(SCREEN_WIDTH, SCREEN_HEIGHT)
    squareRect.w = TILE_WIDTH - TILE_BORDER_WIDTH;
    squareRect.h = TILE_HEIGHT - TILE_BORDER_WIDTH;

    squareRect.y = y;
    squareRect.x = x;

    // Draw it
    SDL_RenderFillRect(renderer, &squareRect);

    // Update screen
    SDL_RenderPresent(renderer);
}

void colorTileByIndex(SDL_Renderer* renderer, int index, int r, int g, int b) {
    int y = index / NUM_SQUARES;
    int x = index % NUM_SQUARES;
    colorTile(renderer, ((x * TILE_WIDTH) + TILE_BORDER_WIDTH), (((y * TILE_HEIGHT) + MENU_HEIGHT + TILE_BORDER_WIDTH)), r, g, b);
}

void selectGoalState(SDL_Renderer* renderer, search* s) {
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
                if (mouse_y < MENU_HEIGHT) {
                    // don't do anything if it is in the menu bar. That will be handled elsewhere
                    return;
                }

                closest = getClosestTile(mouse_x, mouse_y);
                new_goal = (closest.xIndex + (closest.yIndex * NUM_SQUARES));

                if (s->goal == WALL) {
                    s->goalx = closest.x;
                    s->goaly = closest.y;
                    s->goal = new_goal;

                    // marks the new goal in the array
                    s->states[new_goal] = GOAL;
                    colorTile(renderer, s->goalx, s->goaly, GOAL_COLOR.r, GOAL_COLOR.g, GOAL_COLOR.b);
                } else {
                    // reset the original goal state tile to be the background color
                    colorTile(renderer, s->goalx, s->goaly, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);

                    s->goalx = closest.x;
                    s->goaly = closest.y;

                    // reset the old goal to be a blank space
                    s->states[s->goal] = EMPTY_SPACE;

                    // make the new index a goal
                    s->states[new_goal] = GOAL;
                    s->goal = new_goal;

                    // TODO look into why display blanks without this. Fast enough it doesn't really matter?
                    usleep(5000);
                    // Color the new goal state
                    colorTileByIndex(renderer, s->goal, GOAL_COLOR.r, GOAL_COLOR.g, GOAL_COLOR.b);
                }

                return;
            case SDL_KEYDOWN:
                // if the user clicks away or anything, don't do anything
                return;
        }
    }
    return;
}

void selectStartState(SDL_Renderer* renderer, search* s) {
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
                if (mouse_y < MENU_HEIGHT) {
                    // don't do anything if it is in the menu bar. That will be handled elsewhere
                    return;
                }

                closest = getClosestTile(mouse_x, mouse_y);
                new_start = (closest.xIndex + (closest.yIndex * NUM_SQUARES));

                if (s->start == WALL) {
                    s->startx = closest.x;
                    s->starty = closest.y;
                    s->start = new_start;

                    // marks the new start in the array
                    s->states[new_start] = START;
                    colorTile(renderer, s->startx, s->starty, GOAL_COLOR.r, GOAL_COLOR.g, GOAL_COLOR.b);
                    colorTile(renderer, s->startx, s->starty, START_COLOR.r, START_COLOR.g, START_COLOR.b);
                } else {
                    // reset the original start state tile to be the background color
                    colorTile(renderer, s->startx, s->starty, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);

                    s->startx = closest.x;
                    s->starty = closest.y;

                    // reset the old start to be a blank space
                    s->states[s->start] = EMPTY_SPACE;

                    // make the new index a start
                    s->states[new_start] = START;
                    s->start = new_start;

                    // TODO look into why display blanks without this. Fast enough it doesn't really matter?
                    usleep(5000);
                    // Color the new start state
                    colorTileByIndex(renderer, s->start, START_COLOR.r, START_COLOR.g, START_COLOR.b);
                }

                return;
            case SDL_KEYDOWN:
                // if the user clicks away or anything, don't do anything
                return;
        }
    }
    return;
}

void drawStartButton(SDL_Renderer* renderer, color textColor, color backgroundColor) {
    char start[] = "START";
    int width = setting.startButtonX2 - setting.startButtonX;

    draw_text(renderer, start, setting.startButtonX, setting.startButtonY, width, MENU_HEIGHT, textColor, backgroundColor);
}

void drawGoalButton(SDL_Renderer* renderer, color textColor, color backgroundColor) {
    char goal[] = "GOAL";
    int width = setting.goalButtonX2 - setting.goalButtonX;

    draw_text(renderer, goal, setting.goalButtonX, setting.goalButtonY, width, MENU_HEIGHT, textColor, backgroundColor);
}
