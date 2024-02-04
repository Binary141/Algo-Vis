#ifndef SEARCH_H
#define SEARCH_H

#include <SDL2/SDL.h>

#define WALL 0
#define EMPTY_SPACE -1
#define GOAL 100
#define START 500

struct search {
    int start;
    int startx;
    int starty;
    int goal;
    int goalx;
    int goaly;
    int *states;
    int stateSize;
    int numTiles;
};

void bfs(SDL_Renderer*, search);

#endif
