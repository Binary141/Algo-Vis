#include "display.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <unistd.h> // usleep()
#include <stdlib.h>
#include "font.h"

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
    0,
    0,
    0,
    0,
    2,
    2
};

settings 
getDisplaySettings() 
{
    return setting;
}

void 
setDisplaySettings(settings set) 
{
    setting = set;
}

void 
resizeGridLayout() 
{
    // change the display dimensions if there is any extra space on the
    // bottom or right of the window
    screen ret;

    int tile_width = (setting.width / setting.numTiles);
    setting.tileWidth = tile_width;
    setting.gridStartX = ((setting.width - (setting.tileWidth * setting.numTiles)) / 2);

    int calculated_height = (setting.height - MENU_HEIGHT - TILE_BORDER_WIDTH) / setting.numTiles;
    setting.tileHeight = calculated_height;

    setting.gridHeight = setting.height - setting.statusHeight;

    // position buttons on the far right of display
    int offset = setting.width * 0.05;
    setting.startButtonX = setting.width - offset;
    setting.startButtonX2 = setting.width;

    setting.startButtonY = 0;

    setting.goalButtonX = setting.width - (2.4 * offset);
    setting.goalButtonX2 = setting.width - (1.4 * offset);

    setting.goalButtonY = 0;

    return;
}

screen 
init_display() 
{
    // change the display dimensions if there is any extra space on the
    // bottom or right of the window
    screen ret;

    int calculated_height = (setting.height - MENU_HEIGHT - TILE_BORDER_WIDTH) / setting.numTiles;
    setting.tileHeight = calculated_height;

    setting.statusHeight = setting.height - (calculated_height * setting.numTiles) - TILE_BORDER_WIDTH;
    setting.gridHeight = setting.height - setting.statusHeight;

    resizeGridLayout();

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

    // create all the textures we need
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
                                    setting.statusHeight);
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

    ret.menuTexture = SDL_CreateTexture(ret.renderer,
                                    SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_TARGET,
                                    setting.width,
                                    setting.height);
    if(!ret.menuTexture) {
        printf("Failed to create menuTexture!\n");
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

    ret.scratchTexture = SDL_CreateTexture(ret.renderer,
                                    SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_TARGET,
                                    setting.width,
                                    setting.height);
    if(!ret.scratchTexture) {
        printf("Failed to create scratchTexture!\n");
        printf("SDL2 Error: %s\n", SDL_GetError());
        exit(1);
    }

    // need to call this so we can load fonts
    TTF_Init();

    return ret;
}

void 
draw_grid(SDL_Renderer* r, SDL_Texture* t, int shouldRender) 
{

    // anything drawn to renderer will be drawn to the texture
    SDL_SetRenderTarget(r, t);

    // Initialize renderer color white for the background
    SDL_SetRenderDrawColor(r, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 255);

    // Clear screen
    SDL_RenderClear(r);

    // Actually draw the desired color
    SDL_SetRenderDrawColor(r, BORDER_R, BORDER_G, BORDER_B, 255);

    // Declare rect of square
    SDL_Rect squareRect;

    squareRect.w = TILE_BORDER_WIDTH;
    squareRect.h = (setting.numTiles * setting.tileHeight) + TILE_BORDER_WIDTH;

    squareRect.x = 0;
    squareRect.y = setting.statusHeight;

    // draw vertical lines
    for (int i = 0; i < setting.numTiles + 1; i++) {
        squareRect.x = (i * setting.tileWidth) + setting.gridStartX;

        // Draw it
        SDL_RenderFillRect(r, &squareRect);
    }

    squareRect.w = setting.width;
    squareRect.h = TILE_BORDER_WIDTH;

    // draw in the middle of the screen
    squareRect.x = (setting.width / 2) - (squareRect.w / 2);

    // draw the first line all the way across the screen
    squareRect.y = (0 * setting.tileHeight) + setting.statusHeight;

    // Draw it
    SDL_RenderFillRect(r, &squareRect);

    // draw all the rest in the grid
    squareRect.w = setting.numTiles * setting.tileWidth;
    squareRect.h = TILE_BORDER_WIDTH;
    squareRect.x = (setting.width / 2) - (squareRect.w / 2);
    squareRect.y = 0;

    // draw horizontal lines
    for (int i = 0; i < setting.numTiles + 1; i++) {
        squareRect.y = (i * setting.tileHeight) + setting.statusHeight;

        // Draw it
        SDL_RenderFillRect(r, &squareRect);
    }

    // Reset the rendering target to the default (the window)
    SDL_SetRenderTarget(r, nullptr);

    // Clear the renderer
    SDL_SetRenderDrawColor(r, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, 255);
    SDL_RenderClear(r);

    SDL_RenderCopy(r, t, NULL, NULL);

    // Update screen
    if (shouldRender) {
        SDL_RenderPresent(r);
        usleep(SLEEPTIME2);
    }

    drawStartButton(r, t, textColor, backgroundColor, shouldRender);
    drawGoalButton(r, t, textColor, backgroundColor, shouldRender);
}

void
showMenu(SDL_Renderer* r, SDL_Texture* t, int width, int height) 
{
    // Declare rect of square
    SDL_Rect squareRect;
    // Square dimensions
    squareRect.w = setting.width / 2;
    squareRect.h = setting.height / 1.2;

    squareRect.y = (setting.height / 2) - (squareRect.h / 2);
    squareRect.x = (setting.width / 2) - (squareRect.w / 2);

    SDL_RenderCopy(r, t, &squareRect, &squareRect);

    SDL_RenderPresent(r);
}

void
drawHelp(SDL_Renderer* r, SDL_Texture* t, int width, int height)
{
    // anything drawn to renderer will be drawn to the texture
    SDL_SetRenderTarget(r, t);

    // Actually draw the desired color
    SDL_SetRenderDrawColor(r, 125, 125, 125, 255);

    // Declare rect of square
    SDL_Rect squareRect;

    // Square dimensions
    squareRect.w = setting.width / 2;
    squareRect.h = setting.height / 1.2;

    squareRect.y = (setting.height / 2) - (squareRect.h / 2);
    squareRect.x = (setting.width / 2) - (squareRect.w / 2);

    int y = (setting.height / 2) - (squareRect.h / 2);
    int x = (setting.width / 2) - (squareRect.w / 4);

    // Draw it
    SDL_RenderFillRect(r, &squareRect);

    // Reset the rendering target to the default (the window)
    SDL_SetRenderTarget(r, NULL);

    SDL_RenderCopy(r, t, &squareRect, &squareRect);

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wwrite-strings"

    const int helpLen = 13;
    char* helpTexts[helpLen] = {
        "a - Add Tiles",
        "d - Delete Tiles",

        "b - Breadth First Search",
        "u - Depth First Search",
        "j - Astar Search",
        "k - Greedy Search",

        "c - Clear Entire Screen",
        "r - Clear Visited States",

        "h - Halt a Search",

        "left click - Add a wall",
        "right click - Delete a wall",

        "/ - Help Menu",
        "m - Setting Menu"
    };
    #pragma GCC diagnostic pop

    int tempHeight = 40;

    for (int i = 0; i < helpLen; i++) {
        draw_text(r, t, helpTexts[i], x, y, 400, tempHeight, color{0, 0, 0}, color{125, 125, 125}, 0, 0);
        y += 40;

    }
}

void
showHelp(SDL_Renderer* r, SDL_Texture* t, int width, int height)
{
    // Declare rect of square
    SDL_Rect squareRect;
    // Square dimensions
    squareRect.w = setting.width / 2;
    squareRect.h = setting.height / 1.2;

    squareRect.y = (setting.height / 2) - (squareRect.h / 2);
    squareRect.x = (setting.width / 2) - (squareRect.w / 2);

    SDL_RenderCopy(r, t, &squareRect, &squareRect);

    SDL_RenderPresent(r);
}

void 
drawMenu(SDL_Renderer* r, SDL_Texture* t, int width, int height, int heuristic) 
{
    // anything drawn to renderer will be drawn to the texture
    SDL_SetRenderTarget(r, t);

    // Actually draw the desired color
    SDL_SetRenderDrawColor(r, 125, 125, 125, 255);

    // Declare rect of square
    SDL_Rect squareRect;
    // Square dimensions
    squareRect.w = setting.width / 2;
    squareRect.h = setting.height / 1.2;

    squareRect.y = (setting.height / 2) - (squareRect.h / 2);
    squareRect.x = (setting.width / 2) - (squareRect.w / 2);

    int y = (setting.height / 2) - (squareRect.h / 2);
    int x = (setting.width / 2) - (squareRect.w / 4);

    // Draw it
    SDL_RenderFillRect(r, &squareRect);

    // Reset the rendering target to the default (the window)
    SDL_SetRenderTarget(r, NULL);

    SDL_RenderCopy(r, t, &squareRect, &squareRect);

    char w[5];
    snprintf(w, 5, "%d", width);

    char h[5];
    snprintf(h, 5, "%d", height);

    char *hStr = (char*) malloc(strlen(h) + 1);
    if (!hStr) {
        printf("Couldn't Malloc for height string!\n");
        return;
    }

    char *wStr = (char*) malloc(strlen(w) + 1);
    if (!wStr) {
        printf("Couldn't Malloc for width string!\n");
        return;
    }

    strcpy(hStr, h);
    strcpy(wStr, w);

    char help[] = "To cycle between display sizes, press '-' or '='";
    char help2[] = "To leave the menu, press escape";
    char help3[] = "To cycle between heuristics, press '[' or ']'";
    char help4[] = "To save, press enter or 'm'";
    char text[] = "Display Size:";
    char by[] = "x";
    int tempHeight = 50;

    draw_text(r, t, help, x, y, 450, tempHeight, color{0, 0, 0}, color{125, 125, 125}, 0, 0);
    y += 50;

    draw_text(r, t, help2, x, y, 400, tempHeight, color{0, 0, 0}, color{125, 125, 125}, 0, 0);
    y += 50;

    draw_text(r, t, help3, x, y, 450, tempHeight, color{0, 0, 0}, color{125, 125, 125}, 0, 0);
    y += 50;

    draw_text(r, t, help4, x, y, 400, tempHeight, color{0, 0, 0}, color{125, 125, 125}, 0, 0);
    y += 100;

    draw_text(r, t, text, x, y, 150, tempHeight, color{0, 0, 0}, color{125, 125, 125}, 0, 0);
    y += 50;

    draw_text(r, t, wStr, x, y, 100, tempHeight, color{0, 0, 0}, color{125, 125, 125}, 0, 0);
    draw_text(r, t, by, x + 110, y, 20, tempHeight, color{0, 0, 0}, color{125, 125, 125}, 0, 0);
    draw_text(r, t, hStr, x + 140, y, 100, tempHeight, color{0, 0, 0}, color{125, 125, 125}, 0, 0);

    char text1[] = "Current Heuristic:";

    y += 100;
    draw_text(r, t, text1, x, y, 200, tempHeight, color{0, 0, 0}, color{125, 125, 125}, 0, 0);
    y += tempHeight;

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wwrite-strings"
    char* supportedHeuristics[] = {"None", "SLD", "Manhattan"};
    #pragma GCC diagnostic pop

    int heuristicWidths[] = {100, 100, 150};

    draw_text(r, t, supportedHeuristics[heuristic], x, y, heuristicWidths[heuristic], tempHeight, color{0, 0, 0}, color{125, 125, 125}, 0, 0);
}

void
testTexture(SDL_Renderer* renderer, SDL_Texture* texture, int* states)
{
    int shouldRender = 1;
    draw_grid(renderer, texture, shouldRender);

    usleep(SLEEPTIME2);
}

void
drawStatusBar(SDL_Renderer* r, SDL_Texture* t, color txtColor, color bgColor)
{

    // anything drawn to renderer will be drawn to the texture
    SDL_SetRenderTarget(r, t);

    // Clear screen
    SDL_RenderClear(r);

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

    drawStartButton(r, t, txtColor, bgColor, 1);
    drawGoalButton(r, t, txtColor, bgColor, 1);
}

void
reset(SDL_Renderer* r, SDL_Texture* t, int* states)
{
    int shouldRender = 1;

    draw_grid(r, t, shouldRender);
    usleep(SLEEPTIME2);

    // set all states to be open
    for(int i = 0; i < setting.numTiles * setting.numTiles; i++) {
        states[i] = EMPTY_SPACE;
    }

    usleep(SLEEPTIME2);
}

void
draw_text(SDL_Renderer* r, SDL_Texture* t, char* text, int x, int y, int width, int height, color txtColor, color bgColor, int shouldRender, int drawOverExistingTexture)
{

    // anything drawn to renderer will be drawn to the texture
    SDL_SetRenderTarget(r, t);

    // draw the background of the text box
    SDL_SetRenderDrawColor(r, bgColor.r, bgColor.g, bgColor.b, 255);

    // Create a read-only memory stream
    SDL_RWops* fontRWops = SDL_RWFromConstMem(OpenSans_Regular_ttf, sizeof(OpenSans_Regular_ttf));
    if (!fontRWops) {
        printf("%s", SDL_GetError());
        exit(1);
    }

    // Load the font from the memory stream
    TTF_Font* font = TTF_OpenFontRW(fontRWops, 1, 75);
    if (!font) {
        printf("%s", SDL_GetError());
        exit(1);
    }

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, SDL_Color{txtColor.r, txtColor.g, txtColor.b});

    // now you can convert it into a texture
    SDL_Texture* Message = SDL_CreateTextureFromSurface(r, textSurface);

    SDL_Rect Message_rect{x, y, width, height}; //create a rect
                                                //
    // Draw it
    SDL_RenderFillRect(r, &Message_rect);

    SDL_Rect srcrect{x, y, width, setting.statusHeight};

    if (drawOverExistingTexture) {
        // reset the render target to the default
        SDL_SetRenderTarget(r, NULL);
    }

    SDL_RenderCopy(r, t, &srcrect, &srcrect);
    usleep(SLEEPTIME2);

    SDL_RenderCopy(r, Message, NULL, &Message_rect);

    if (!drawOverExistingTexture) {
        // reset the render target to the default
        SDL_SetRenderTarget(r, NULL);
    }

    if (shouldRender) {
        // Update screen if needed
        SDL_RenderPresent(r);
        usleep(SLEEPTIME2);
    }
}

void
destroy_window(SDL_Renderer* r, SDL_Window* window)
{
    // Destroy renderer
    SDL_DestroyRenderer(r);

    // Destroy window
    SDL_DestroyWindow(window);

    // Quit SDL
    SDL_Quit();
}

tile
getClosestTile(int x, int y)
{
    tile closest;

    int closest_x = TILE_BORDER_WIDTH + setting.gridStartX;
    while ( closest_x < x ) {
        closest_x += setting.tileWidth;
    }

    // subtract the setting.tileWidth since we over shot it
    closest_x -= setting.tileWidth;

    // find out how many tiles it takes to subtract from the y position before it is negative
    int closest_y = y - TILE_BORDER_WIDTH - setting.statusHeight;

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
    closest.y = setting.statusHeight + (numTilesAway * setting.tileHeight) + TILE_BORDER_WIDTH;
    closest.xIndex = (closest_x - TILE_BORDER_WIDTH - setting.gridStartX) / setting.tileWidth;
    closest.yIndex = numTilesAway;

    return closest;
}

void
ColorBlankTile(SDL_Renderer* r, SDL_Texture* texture)
{

    // anything drawn to renderer will be drawn to the texture
    SDL_SetRenderTarget(r, texture);

    // Declare rect of square
    SDL_Rect squareRect;

    // Actually draw the desired color
    SDL_SetRenderDrawColor(r, 0, 0, 0, 0);

    squareRect.w = 1;
    squareRect.h = 1;

    squareRect.y = 1;
    squareRect.x = 1;

    // Draw it
    SDL_RenderFillRect(r, &squareRect);

    // Reset the rendering target to the default (the window)
    SDL_SetRenderTarget(r, NULL);

    // don't try to update the status bar portion of the display
    SDL_Rect srcrect;
    srcrect.w = setting.width;
    srcrect.h = setting.height - setting.statusHeight;
    srcrect.x = 0;
    srcrect.y = setting.statusHeight;

    SDL_RenderCopy(r, texture, &srcrect, &srcrect);

    SDL_RenderPresent(r);
    usleep(SLEEPTIME2);
}

void
colorTile(SDL_Renderer* r, SDL_Texture* t, int x, int y, color Color, int shouldRender)
{

    // anything drawn to renderer will be drawn to the texture
    SDL_SetRenderTarget(r, t);

    // Declare rect of square
    SDL_Rect squareRect;

    // Actually draw the desired color
    SDL_SetRenderDrawColor(r, Color.r, Color.g, Color.b, 255);

    // Square dimensions: Half of the min(SCREEN_WIDTH, SCREEN_HEIGHT)
    squareRect.w = setting.tileWidth - TILE_BORDER_WIDTH;
    squareRect.h = setting.tileHeight - TILE_BORDER_WIDTH;

    squareRect.y = y;
    squareRect.x = x;

    // Draw it
    SDL_RenderFillRect(r, &squareRect);

    // Reset the rendering target to the default (the window)
    SDL_SetRenderTarget(r, NULL);

    // Update screen if needed
    if (shouldRender) {
        // don't try to update the status bar portion of the display
        SDL_Rect srcrect;
        srcrect.w = setting.width;
        srcrect.h = setting.height - setting.statusHeight;
        srcrect.x = 0;
        srcrect.y = setting.statusHeight;

        SDL_RenderCopy(r, t, &srcrect, &srcrect);

        SDL_RenderPresent(r);
        usleep(SLEEPTIME2);
    }
}

void
colorTileByIndex(SDL_Renderer* r, SDL_Texture* t, int index, color Color, int shouldUpdate)
{
    int y = index / setting.numTiles;
    int x = index % setting.numTiles;
    colorTile(r, 
              t, 
              ((x * setting.tileWidth) + TILE_BORDER_WIDTH + setting.gridStartX), 
              (((y * setting.tileHeight) + setting.statusHeight + TILE_BORDER_WIDTH)), 
              Color, 
              shouldUpdate
              );

    usleep(SLEEPTIME2);
}

void
selectGoalState(SDL_Renderer* r, SDL_Texture* t, search* s)
{
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
                if (isInMenu(mouse_y) || isOutOfGrid(mouse_x, mouse_y)) {
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
                    colorTile(r, t, s->goalx, s->goaly, GOAL_COLOR, 1);
                    return;
                }

                // reset the original goal state tile to be the background color
                colorTile(r, t, s->goalx, s->goaly, backgroundColor, 1);

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
                colorTileByIndex(r, t, s->goal, GOAL_COLOR, 1);

                return;
            case SDL_KEYDOWN:
                // if the user clicks away or anything, don't do anything
                return;
        }
    }
    return;
}

void
selectStartState(SDL_Renderer* r, SDL_Texture* t, search* s)
{
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

                if (isInMenu(mouse_y) || isOutOfGrid(mouse_x, mouse_y)) {
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
                    colorTile(r, t, s->startx, s->starty, START_COLOR, 1);
                    return;
                }

                // reset the original start state tile to be the background color
                colorTile(r, t, s->startx, s->starty, backgroundColor, 1);

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
                colorTileByIndex(r, t, s->start, START_COLOR, 1);

                return;
            case SDL_KEYDOWN:
                // if the user clicks away or anything, don't do anything
                return;
        }
    }
    return;
}

void
drawStartButton(SDL_Renderer* r, SDL_Texture* t, color txtColor, color bgColor, int shouldRender)
{
    char start[] = "START";
    int width = setting.startButtonX2 - setting.startButtonX;

    draw_text(r, 
              t, 
              start, 
              setting.startButtonX, 
              setting.startButtonY, 
              width, 
              setting.statusHeight, 
              txtColor, 
              bgColor, 
              shouldRender,
              1
              );
}

void
drawStatesCount(SDL_Renderer* r, SDL_Texture* t, color txtColor, color bgColor, int value)
{
    // I don't think we will have any counts larger than 6 digits
    char str[6];
    sprintf(str, "%d", value);

    draw_text(r, 
              t, 
              str, 
              125,  // x
              0,  // y 
              40, // width
              setting.statusHeight, 
              txtColor, 
              bgColor, 
              1,
              1
              );
}

void
drawGoalButton(SDL_Renderer* r, SDL_Texture* t, color txtColor, color bgColor, int shouldRender)
{
    char goal[] = "GOAL";
    int width = setting.goalButtonX2 - setting.goalButtonX;

    draw_text(r, 
              t, 
              goal, 
              setting.goalButtonX, 
              setting.goalButtonY, 
              width, 
              setting.statusHeight, 
              txtColor, 
              bgColor,
              shouldRender,
              1
              );
    usleep(SLEEPTIME2);
}

int
isInMenu(int y)
{
    if (y <= setting.statusHeight + TILE_BORDER_WIDTH) {
        // if it is within y 0 to the height of the menu, return true
        return 1;
    }
    return 0;
}

void
manyWalls(SDL_Renderer* r, SDL_Texture* t, search* search1, int shouldDelete)
{
    int mouse_x, mouse_y;
    int index;
    SDL_Event event2;
    tile closest;

    while (SDL_WaitEvent(&event2)) {
        switch (event2.type) {
        case SDL_MOUSEMOTION:
            // don't need to do anything specific here. The following case will either return or continue if needed
            // continues if it is a mouse motion, but returns if it was a mouse button up
        case SDL_MOUSEBUTTONUP:
            // duplicated code so if a user clicks without moving the cursor, we will still do the action
            SDL_GetMouseState(&mouse_x, &mouse_y);

            // if we are not in the grid, then bail out
            if (isInMenu(mouse_y) || isOutOfGrid(mouse_x, mouse_y)) {
                return;
            }

            closest = getClosestTile(mouse_x, mouse_y);

            index = closest.xIndex + (closest.yIndex * setting.numTiles);

            if (index < 0) {
                return;
            }

            if (shouldDelete) {
                // make it an empty space
                colorTile(r, t, closest.x, closest.y, backgroundColor, 1);

                search1->states[index] = EMPTY_SPACE;
            } else {
                // make it a wall
                search1->states[index] = WALL;

                colorTile(r, t, closest.x, closest.y, color{0,0,255}, 1);
            }

            if (search1->goal == index) {
                search1->goal = EMPTY_SPACE;
            }

            if (search1->start == index) {
                search1->start = EMPTY_SPACE;
            }

            if (event2.type == SDL_MOUSEBUTTONUP) {
                return;
            }
            continue;
        }
    }
}

int
isOutOfGrid(int x, int y)
{
    if (x <= (setting.gridStartX + TILE_BORDER_WIDTH) || x > ((setting.numTiles * setting.tileWidth) + setting.gridStartX) || y > ((setting.numTiles * setting.tileHeight) + setting.statusHeight)) {
        return 1;
    }

    return 0;
}
