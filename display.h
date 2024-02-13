#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include "search.h"

#define TILE_BORDER_WIDTH 3
#define NUM_SQUARES 25
#define MENU_HEIGHT 50

// These may change in init_display
// Maybe look into seeing if these could just be left alone
#define SCREEN_WIDTH 1080 // 2560
#define SCREEN_HEIGHT 720 // 1080

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
};

struct settings {
    int width;
    int height;
    int tileWidth;
    int tileHeight;
    int numTiles;
    int menuHeight;
    int startButtonX;
    int startButtonX2;
    int startButtonY;
    int goalButtonX;
    int goalButtonX2;
    int goalButtonY;
};

struct color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

settings getDisplaySettings();
void setDisplaySettings(settings);
screen init_display();
void draw_grid(SDL_Renderer*);
void draw_text(SDL_Renderer*, char*, int, int, int, int, color, color);
void reset(SDL_Renderer*, int*);
void destroy_window(SDL_Renderer*, SDL_Window*);
tile getClosestTile(int, int);
void colorTile(SDL_Renderer* , int, int, int, int, int);
void colorTileByIndex(SDL_Renderer* , int, int, int, int);
void selectStartState(SDL_Renderer*, search*);
void selectGoalState(SDL_Renderer*, search*);
void drawStartButton(SDL_Renderer*, color, color);
void drawGoalButton(SDL_Renderer*, color, color);

extern settings setting;

#endif
