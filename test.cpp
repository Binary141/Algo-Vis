#include <SDL2/SDL.h>
#include <iostream>

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
const int TEXTURE_WIDTH = 200;
const int TEXTURE_HEIGHT = 200;

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Texture Drawing Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Create a texture to draw on
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, TEXTURE_WIDTH, TEXTURE_HEIGHT);

    // Set the texture as the rendering target
    SDL_SetRenderTarget(renderer, texture);

    // Clear the texture with a white background
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // Draw colored boxes on the texture
    SDL_Rect box1 = {10, 10, 50, 50};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
    SDL_RenderFillRect(renderer, &box1);

    SDL_Rect box2 = {100, 50, 80, 120};
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue
    SDL_RenderFillRect(renderer, &box2);

    // Reset the rendering target to the default (the window)
    SDL_SetRenderTarget(renderer, nullptr);

    // Clear the renderer
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // Render the texture to the screen
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);

    // Present the renderer
    SDL_RenderPresent(renderer);

    // Main loop
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
    }

    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
