#include <SDL2/SDL.h>

#define TILE_BORDER_WIDTH 3
#define NUM_SQUARES 10
#define MENU_HEIGHT 50

// These may change in init_display
// Maybe look into seeing if these could just be left alone
#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 720

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

#define START_COLOR color{0, 255, 0}
#define GOAL_COLOR color{255, 0, 0}

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
};

struct color {
    int r;
    int g;
    int b;
};

settings getDisplaySettings();
screen init_display();
void draw_grid(SDL_Renderer*);
void draw_text(SDL_Renderer*, char*, int, int, int, int, unsigned char, unsigned char, unsigned char);
void destroy_window(SDL_Renderer*, SDL_Window*);
tile getClosestTile(int, int);
void colorTile(SDL_Renderer* , int, int, int, int, int);
void colorTileByIndex(SDL_Renderer* , int, int, int, int);
