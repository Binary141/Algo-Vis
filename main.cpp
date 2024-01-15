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

#define NUM_SQUARES 3

int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 720;

const int BACKGROUND_R = 255;
const int BACKGROUND_G = 255;
const int BACKGROUND_B = 255;

const int BORDER_R = 0;
const int BORDER_G = 0;
const int BORDER_B = 0;

int TILE_WIDTH = SCREEN_WIDTH / NUM_SQUARES;
int TILE_HEIGHT = SCREEN_HEIGHT / NUM_SQUARES;

SDL_Window* window;

SDL_Renderer* renderer;

void init_display() {
    int calculated_width = (NUM_SQUARES * TILE_WIDTH) + TILE_BORDER_WIDTH;

    if (calculated_width != SCREEN_WIDTH) {
        SCREEN_WIDTH = calculated_width;
    }

    int calculated_height = (NUM_SQUARES * TILE_HEIGHT) + TILE_BORDER_WIDTH;

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

void draw(int r, int g, int b) {
    // Declare rect of square
    SDL_Rect squareRect;

    // Square dimensions: Half of the min(SCREEN_WIDTH, SCREEN_HEIGHT)
    squareRect.w = SCREEN_WIDTH; // MIN(SCREEN_WIDTH, SCREEN_HEIGHT) / 2;
    squareRect.h = SCREEN_HEIGHT; // MIN(SCREEN_WIDTH, SCREEN_HEIGHT) / 2;

    // Square position: In the middle of the screen
    squareRect.x = SCREEN_WIDTH / 2 - squareRect.w / 2;
    squareRect.y = SCREEN_HEIGHT / 2 - squareRect.h / 2;

    // Initialize renderer color white for the background
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Clear screen
    SDL_RenderClear(renderer);

    // Actually draw the desired color
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);

    // Draw filled square
    SDL_RenderFillRect(renderer, &squareRect);

    // Update screen
    SDL_RenderPresent(renderer);
}

void init() {
    // Declare rect of square
    SDL_Rect squareRect;

    // Initialize renderer color white for the background
    SDL_SetRenderDrawColor(renderer, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 255);

    // Clear screen
    SDL_RenderClear(renderer);

    // Actually draw the desired color
    SDL_SetRenderDrawColor(renderer, BORDER_R, BORDER_G, BORDER_B, 255);

    // Square dimensions: Half of the min(SCREEN_WIDTH, SCREEN_HEIGHT)
    squareRect.w = TILE_BORDER_WIDTH; // MIN(SCREEN_WIDTH, SCREEN_HEIGHT) / 2;
    squareRect.h = SCREEN_HEIGHT; // MIN(SCREEN_WIDTH, SCREEN_HEIGHT) / 2;

    // Square position: In the middle of the screen
    // squareRect.x = 0; // SCREEN_WIDTH / 2 - squareRect.w / 2;
    squareRect.y = SCREEN_HEIGHT / 2 - squareRect.h / 2;

    // draw vertical lines
    for (int i = 0; i < SCREEN_WIDTH; i += TILE_WIDTH) {
        squareRect.x = i;

        // Draw it
        SDL_RenderFillRect(renderer, &squareRect);
    }

    // Square dimensions: Half of the min(SCREEN_WIDTH, SCREEN_HEIGHT)
    squareRect.w = SCREEN_WIDTH; // MIN(SCREEN_WIDTH, SCREEN_HEIGHT) / 2;
    squareRect.h = TILE_BORDER_WIDTH; // MIN(SCREEN_WIDTH, SCREEN_HEIGHT) / 2;
                                  //
    squareRect.x = SCREEN_WIDTH / 2 - squareRect.w / 2;
    // draw horizontal lines
    for (int i = 0; i < SCREEN_HEIGHT; i += TILE_HEIGHT) {
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

tile getClosestTile(int x, int y) {
    tile closest;

    int closest_x = TILE_BORDER_WIDTH;
    while ( closest_x < x ) {
        closest_x += TILE_WIDTH;
    }
    // subtract the TILE_WIDTH since we over shot it
    closest_x -= TILE_WIDTH;

    int closest_y = TILE_BORDER_WIDTH;
    while ( closest_y < y ) {
        closest_y += TILE_HEIGHT;
    }
    // subtract the TILE_WIDTH since we over shot it
    closest_y -= TILE_HEIGHT;


    closest.x = closest_x;
    closest.y = closest_y;
    closest.xIndex = closest_x / TILE_WIDTH;
    closest.yIndex = closest_y / TILE_HEIGHT;
    // printf("Closest x index: %d\n\n", closest.xIndex);

    // printf("Closest y index: %d\n\n", closest.yIndex);

    return closest;
}

void colorTile(int x, int y, int r, int g, int b) {
    tile closestTile = getClosestTile(x,y);

    // Declare rect of square
    SDL_Rect squareRect;

    // Initialize renderer color white for the background
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Actually draw the desired color
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);

    // Square dimensions: Half of the min(SCREEN_WIDTH, SCREEN_HEIGHT)
    squareRect.w = TILE_WIDTH - TILE_BORDER_WIDTH; // MIN(SCREEN_WIDTH, SCREEN_HEIGHT) / 2;
    squareRect.h = TILE_HEIGHT - TILE_BORDER_WIDTH; // MIN(SCREEN_WIDTH, SCREEN_HEIGHT) / 2;

    squareRect.y = closestTile.y; // SCREEN_HEIGHT / 2 - squareRect.h / 2;
    squareRect.x = closestTile.x; // SCREEN_WIDTH / 2 - squareRect.w / 2;

    // Draw it
    SDL_RenderFillRect(renderer, &squareRect);

    // Update screen
    SDL_RenderPresent(renderer);
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

    init();

    while(!should_quit) {
        SDL_PollEvent(&event);

        switch (event.type) {
            case SDL_QUIT:
                std::cout << "quit: " << event.type << std::endl;
                should_quit = 1;
                break;
            case SDL_KEYDOWN:
                std::cout << "keydown: " << event.type << std::endl;
                if (event.key.keysym.sym == SDLK_q) {
                    // If the 'q' button is pressed, quit the application
                    should_quit = 1;
                    break;
                }
                if (event.key.keysym.sym == SDLK_c) {
                    // clear the screen
                    init();
                    continue;
                }
            case SDL_MOUSEBUTTONDOWN:
                int mouse_x, mouse_y;
                SDL_GetMouseState(&mouse_x, &mouse_y);
                colorTile(mouse_x, mouse_y, 0, 0, 255);
        }

        // Change the color from blue to red over time
        // draw(red_iter, 0, blue_iter);
        // red_iter++;
        // blue_iter--;
        // red_iter %= 255;
        // if (blue_iter == 0) {
        //     blue_iter = 255;
        // }
        // init();

        // Sleep for half a second
        // usleep(0.5 * 100000);
    }
    destroy_window();

    return 0;
}
