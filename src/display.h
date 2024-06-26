#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include "search.h"

#define TILE_BORDER_WIDTH 3
#define NUM_SQUARES 12
#define MENU_HEIGHT 40

// These may change in init_display
// Maybe look into seeing if these could just be left alone
#define SCREEN_WIDTH 1920 // 2560
#define SCREEN_HEIGHT 1080 // 1080

const int supportedHeights[5] = {720, 768, 1080, 1504, 1440};
const int supportedWidths[5] = {1280, 1366, 1920, 2256, 2560};

const int NONEHEURISTIC = 0;
const int SLDHEURISTIC = 1;
const int MANHATTANHEURISTIC = 2;

// (255,255,255) is white
const int BACKGROUND_R = 255;
const int BACKGROUND_G = 255;
const int BACKGROUND_B = 255;

// (0,0,0) is black
const int BORDER_R = 0;
const int BORDER_G = 0;
const int BORDER_B = 0;

const int TILE_WIDTH = SCREEN_WIDTH / NUM_SQUARES;
const int TILE_HEIGHT = SCREEN_HEIGHT / NUM_SQUARES;

#define SLEEPTIME2 1000

#define START_COLOR color{0, 255, 0}
#define GOAL_COLOR color{255, 0, 0}

#define FONT_SIZE 75

struct tile {
    int x;
    int y;
    int xIndex;
    int yIndex;
};

struct screen {
    SDL_Window* window;
    SDL_Surface* surface;
    SDL_Renderer* renderer;
    SDL_Texture* texture1;
    SDL_Texture* texture2;
    SDL_Texture* menuTexture;
    SDL_Texture* statusTexture;
    SDL_Texture* currTexture;
    SDL_Texture* backTexture;
    SDL_Texture* scratchTexture;
};

struct settings {
    int width;
    int height;
    int tileWidth;
    int tileHeight;
    int numTiles;
    int statusHeight;
    int gridStartX;
    int gridEndY;
    int gridHeight;
    int startButtonX;
    int startButtonX2;
    int startButtonY;
    int goalButtonX;
    int goalButtonX2;
    int goalButtonY;
    int currentResolution;
    int selectedResolution;
};

struct color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

settings getDisplaySettings();
void setDisplaySettings(settings);
void resizeGridLayout();
screen init_display();
void draw_grid(SDL_Renderer*, SDL_Texture*, int);
void draw_text(SDL_Renderer*, SDL_Texture*, char*, int, int, int, int, color, color, int, int);
void reset(SDL_Renderer*, SDL_Texture*, int*);
void destroy_window(SDL_Renderer*, SDL_Window*);
tile getClosestTile(int, int);
void ColorBlankTile(SDL_Renderer*, SDL_Texture*);
void colorTile(SDL_Renderer* , SDL_Texture*, int, int, color, int);
void colorTileByIndex(SDL_Renderer*, SDL_Texture*, int, color, int);
void selectStartState(SDL_Renderer*, SDL_Texture*, search*);
void selectGoalState(SDL_Renderer*, SDL_Texture*, search*);
void drawStartButton(SDL_Renderer*, SDL_Texture*, color, color, int);
void drawGoalButton(SDL_Renderer*, SDL_Texture*, color, color, int);
void drawStatesCount(SDL_Renderer*, SDL_Texture*, color, color, int);
void testTexture(SDL_Renderer*, SDL_Texture*, int*);
int isInMenu(int);
int isOutOfGrid(int, int);
void manyWalls(SDL_Renderer*, SDL_Texture*, search*, int);
void drawStatusBar(SDL_Renderer*, SDL_Texture*, color, color);
void showMenu(SDL_Renderer*, SDL_Texture*, int, int);
void drawMenu(SDL_Renderer*, SDL_Texture*, int, int, int);
void drawHelp(SDL_Renderer*, SDL_Texture*, int, int);
void showHelp(SDL_Renderer*, SDL_Texture*, int, int);

extern settings setting;

// white
const color bg{255, 255, 255};
// color text{255,255,255};

// black
const color textColor{0, 0, 0};
const color backgroundColor{255, 255, 255};

#endif
