#ifndef SEARCH_H
#define SEARCH_H

#include <SDL2/SDL.h>
#include <vector>

#define WALL 0
#define EMPTY_SPACE -1
#define VISITED 2
#define GOAL 100
#define START 500

const int SLEEPTIME = 10000;
extern int isSearching;

// defined in search.cpp
extern int doneSearching;

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

void bfs(SDL_Renderer*, SDL_Texture*, search*);
void dfs(SDL_Renderer*, SDL_Texture*, search*);
void clearTiles(SDL_Renderer*, SDL_Texture*, search*, int, int, int);
void clearStates(search*);
void astar(SDL_Renderer*, SDL_Texture*, search*);
void greedy(SDL_Renderer*, SDL_Texture*, search*);
std::vector<int> getNeighborIndexes(int, const search*, int);

double manhattanHeuristic(int, int, int, int);
double euclideanHeuristic(int, int, int, int);
double hammingHeuristic(int, int, int, int);
double zeroHeuristic(int, int, int, int);
double getCost(int, int, int, int);

#endif
