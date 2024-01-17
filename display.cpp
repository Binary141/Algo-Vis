#include "display.h"
#include <SDL2/SDL.h>

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
    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    ret.window = window;
    ret.renderer = renderer;
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

    squareRect.y = y; // closestTile.y;
    squareRect.x = x; // closestTile.x;

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
