#include <stdio.h>
#include <stdbool.h>
#include <unistd.h> // usleep()
#include <iostream>

#include <SDL2/SDL.h>

// Define MAX and MIN macros
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

// Define screen dimensions
#define SCREEN_WIDTH    1280
#define SCREEN_HEIGHT   720

// #define TILE_WIDTH   10

#define TILE_BORDER_WIDTH 3

#define NUM_SQUARES 10

int TILE_WIDTH = SCREEN_WIDTH / NUM_SQUARES;
int TILE_HEIGHT = SCREEN_HEIGHT / NUM_SQUARES;

SDL_Window* window;

SDL_Renderer* renderer;

void init_display() {
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
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Clear screen
    SDL_RenderClear(renderer);

    // Actually draw the desired color
    SDL_SetRenderDrawColor(renderer, 125, 125, 125, 255);

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
};

tile getClosestTile(int x, int y) {
    tile closest;
    closest.x = TILE_BORDER_WIDTH;
    closest.y = TILE_BORDER_WIDTH;
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
