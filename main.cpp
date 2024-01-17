#include <stdio.h>
#include <stdbool.h>
#include <unistd.h> // usleep()
#include <iostream>

#include <SDL2/SDL.h>

// Define MAX and MIN macros
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

// Define screen dimensions
// #define SCREEN_WIDTH    1280
// #define SCREEN_HEIGHT   720

// #define TILE_WIDTH   10

#define TILE_BORDER_WIDTH 3

#define NUM_SQUARES 10

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

// (255,255,255) is white
const int BACKGROUND_R = 255;
const int BACKGROUND_G = 255;
const int BACKGROUND_B = 255;

// (0,0,0) is black
const int BORDER_R = 0;
const int BORDER_G = 0;
const int BORDER_B = 0;

int TILE_WIDTH = SCREEN_WIDTH / NUM_SQUARES;
int TILE_HEIGHT = SCREEN_HEIGHT / NUM_SQUARES;

SDL_Window* window;

SDL_Renderer* renderer;

#define MENU_HEIGHT 20

void init_display() {
    // change the display dimensions if there is any extra space on the
    // bottom or right of the window
    int calculated_width = (NUM_SQUARES * TILE_WIDTH) + TILE_BORDER_WIDTH;

    if (calculated_width != SCREEN_WIDTH) {
        SCREEN_WIDTH = calculated_width;
    }

    int calculated_height = (NUM_SQUARES * TILE_HEIGHT) + TILE_BORDER_WIDTH + MENU_HEIGHT;

    if (calculated_height != SCREEN_HEIGHT) {
        SCREEN_HEIGHT = calculated_height;
    }

    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not be initialized!: %s\n", SDL_GetError());
        return;
    }

    // Create window
    window = SDL_CreateWindow("Algo-Visualizer",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void draw_grid() {
    // Declare rect of square
    SDL_Rect squareRect;

    // Initialize renderer color white for the background
    SDL_SetRenderDrawColor(renderer, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 255);

    // Clear screen
    SDL_RenderClear(renderer);

    // Actually draw the desired color
    SDL_SetRenderDrawColor(renderer, BORDER_R, BORDER_G, BORDER_B, 255);

    squareRect.w = TILE_BORDER_WIDTH;
    squareRect.h = SCREEN_HEIGHT;

    squareRect.y = MENU_HEIGHT; // SCREEN_HEIGHT / 2 - squareRect.h / 2;

    // draw vertical lines
    for (int i = 0; i < SCREEN_WIDTH; i += TILE_WIDTH) {
        squareRect.x = i;

        // Draw it
        SDL_RenderFillRect(renderer, &squareRect);
    }

    squareRect.w = SCREEN_WIDTH;
    squareRect.h = TILE_BORDER_WIDTH;

    squareRect.x = SCREEN_WIDTH / 2 - squareRect.w / 2;
    // draw horizontal lines
    for (int i = MENU_HEIGHT; i < SCREEN_HEIGHT; i += TILE_HEIGHT) {
        squareRect.y = i;

        // Draw it
        SDL_RenderFillRect(renderer, &squareRect);
    }

    // Update screen
    SDL_RenderPresent(renderer);
}

void destroy_window() {
    // Destroy renderer
    SDL_DestroyRenderer(renderer);

    // Destroy window
    SDL_DestroyWindow(window);

    // Quit SDL
    SDL_Quit();
}

struct tile {
    int x;
    int y;
    int xIndex;
    int yIndex;
};

struct search {
    int start;
    int startx;
    int starty;
    int goal;
    int goalx;
    int goaly;
};

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

void colorTile(int x, int y, int r, int g, int b) {
    // Declare rect of square
    SDL_Rect squareRect;

    // Initialize renderer color white for the background
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Actually draw the desired color
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);

    // Square dimensions: Half of the min(SCREEN_WIDTH, SCREEN_HEIGHT)
    squareRect.w = TILE_WIDTH - TILE_BORDER_WIDTH;
    squareRect.h = TILE_HEIGHT - TILE_BORDER_WIDTH;

    squareRect.y = y; // closestTile.y;
    squareRect.x = x; // closestTile.x;

    // Draw it
    SDL_RenderFillRect(renderer, &squareRect);

    // Update screen
    SDL_RenderPresent(renderer);
}

void colorTileByIndex(int index, int r, int g, int b) {
    int y = index / NUM_SQUARES;
    int x = index - y;
    // printf("x: %d\n", x);
    // printf("y: %d\n", y);
    colorTile(((x * TILE_WIDTH) + (x * TILE_BORDER_WIDTH)), ((y * TILE_WIDTH) + (y * TILE_BORDER_WIDTH)), r, g, b);
}

int main() {
    init_display();
    if(!window) {
        printf("Window could not be created with error: %s\n", SDL_GetError());
        return 0;
    }
    if(!renderer) {
        printf("Renderer could not be created with error: %s\n", SDL_GetError());
        return 0;
    }

    SDL_Event event;

    int should_quit = 0;
    int red_iter = 0;
    int blue_iter = 255;

    draw_grid();

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
                    draw_grid();
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
                                    colorTile(search1.startx, search1.starty, 0, 255, 0);
                                } else {
                                    // int tmp = search1.start;
                                    // colorTileByIndex(tmp, 125, 125, 125);
                                    colorTile(search1.startx, search1.starty, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);

                                    search1.start = (closest.xIndex + (closest.yIndex * NUM_SQUARES));
                                    search1.startx = closest.x;
                                    search1.starty = closest.y;

                                    // TODO look into why display blanks without this. Fast enough it doesn't really matter?
                                    usleep(5000);
                                    colorTile(search1.startx, search1.starty, 0, 255, 0);
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
                                    colorTile(search1.goalx, search1.goaly, 255, 0, 0);
                                } else {
                                    colorTile(search1.goalx, search1.goaly, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);

                                    search1.goal = (closest.xIndex + (closest.yIndex * NUM_SQUARES));
                                    search1.goalx = closest.x;
                                    search1.goaly = closest.y;

                                    // TODO look into why display blanks without this. Fast enough it doesn't really matter?
                                    usleep(5000);
                                    colorTile(search1.goalx, search1.goaly, 255, 0, 0);
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

                    colorTile(closest.x, closest.y, 0, 0, 255);
                    continue;
                }
                if( event.button.button == SDL_BUTTON_RIGHT ) {
                    // clear the tile if it is the right mouse button
                    colorTile(closest.x, closest.y, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);
                    continue;
                }
        }
    }
    destroy_window();

    return 0;
}
