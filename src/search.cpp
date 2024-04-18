#include "search.h"
#include "display.h"
#include <SDL2/SDL.h>
#include <vector>
#include <unistd.h> // usleep()

int doneSearching = 0;

std::vector<int>
getNeighborIndexes(int current, const search* s, int lastRowIndex)
{
    std::vector<int> res;

    if (current < 0) {
        printf("Current was zero while getting neighbors!\n");
        exit(1);
    }

    // get index to the left of current, if applicable
    if (current % s->numTiles != 0) {
        res.push_back(current - 1);
    }

    // get index above current, if applicable
    if (current > (s->numTiles - 1)) {
        res.push_back(current - s->numTiles);
    }

    int rightMostIndex = ((current / s->numTiles) * s->numTiles) - 1 + s->numTiles;

    // get index to the right of current, if applicable
    if (current % rightMostIndex != 0 || current == 0) {
        res.push_back(current + 1);
    }

    // TODO look into this comparison. Might be pretty slow
    // get index below current, if applicable
    // if (current < ((s->numTiles * s->numTiles) - s->numTiles)) {
    if (current < lastRowIndex) {
        res.push_back(current + s->numTiles);
    }


    return res;
}

void
clearTilesBulk(SDL_Renderer* renderer, SDL_Texture* texture, search* s)
{
    // make sure if we visited a tile, or if it is marked blank, then just clear it out
    int tmp;
    int shouldRender = 0;
    draw_grid(renderer, texture, shouldRender);

    for (int i = 0; i < s->stateSize; i++) {
        tmp = s->states[i];
        if (tmp == VISITED) {
            continue;
        }

        if (tmp == WALL) {
            colorTileByIndex(renderer, texture, i, color{0,0,255}, shouldRender);
            continue;
        }

        if (tmp == GOAL) {
            colorTileByIndex(renderer, texture, i, color{255,0,0}, shouldRender);
            continue;
        }

        if (tmp == START) {
            colorTileByIndex(renderer, texture, i, color{0,255,0}, shouldRender);
            continue;
        }
    }

    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void
clearTilesFromTexture(SDL_Renderer* renderer, SDL_Texture* texture, search* s)
{
    // make sure if we visited a tile, or if it is marked blank, then just clear it out
    int tmp;
    int shouldRender = 0;
    draw_grid(renderer, texture, shouldRender);

    for (int i = 0; i < s->stateSize; i++) {
        tmp = s->states[i];
        if (tmp == VISITED) {
            continue;
        }

        if (tmp == WALL) { // || tmp == EMPTY_SPACE) {
            // s->states[i] = EMPTY_SPACE;
            colorTileByIndex(renderer, texture, i, color{0,0,255}, shouldRender);
        }

        if (tmp == GOAL) { // || tmp == empty_space) {
            // s->states[i] = empty_space;
            colorTileByIndex(renderer, texture, i, color{255,0,0}, shouldRender);
        }

        if (tmp == START) { // || tmp == empty_space) {
            // s->states[i] = empty_space;
            colorTileByIndex(renderer, texture, i, color{0,255,0}, shouldRender);
        }
    }
}

void
clearTiles(SDL_Renderer* renderer, SDL_Texture* texture, search* s)
{
    // make sure if we visited a tile, or if it is marked blank, then just clear it out
    int tmp;
    int shouldRender = 1;
    draw_grid(renderer, texture, shouldRender);

    for (int i = 0; i < s->stateSize; i++) {
        tmp = s->states[i];
        if (tmp == VISITED) {
            continue;
        }

        if (tmp == WALL) { // || tmp == EMPTY_SPACE) {
            // s->states[i] = EMPTY_SPACE;
            colorTileByIndex(renderer, texture, i, color{0,0,255}, shouldRender);
        }

        if (tmp == GOAL) { // || tmp == empty_space) {
            // s->states[i] = empty_space;
            colorTileByIndex(renderer, texture, i, color{255,0,0}, shouldRender);
        }

        if (tmp == START) { // || tmp == empty_space) {
            // s->states[i] = empty_space;
            colorTileByIndex(renderer, texture, i, color{0,255,0}, shouldRender);
        }
    }
    drawStartButton(renderer, texture, textColor, bg, 1);
    drawGoalButton(renderer, texture, textColor, bg, 1);
}

void
clearStates(search* s)
{
    for (int i = 0; i < s->stateSize; i++) {
        // set the tile to be empty if it has been visited
        // note this will not update the display, just used so
        // if a search was stopped, it can search properly
        if (s->states[i] == VISITED) {
            s->states[i] = EMPTY_SPACE;
        }
    }
}

// manhattan distance
double
manhattanHeuristic(int currX, int currY, int goalX, int goalY)
{
    int changeX = abs((currX - goalX));
    int changeY = abs((currY - goalY));

    double dist = changeX + changeY + 0.0;
    return dist;
}

// euclidean distance
// straight line distance
double
euclideanHeuristic(int currX, int currY, int goalX, int goalY)
{
    int changeX = ((currX - goalX) * (currX - goalX));
    int changeY = ((currY - goalY) * (currY - goalY));

    return sqrt(changeX + changeY);
}

double
zeroHeuristic(int currX, int currY, int goalX, int goalY)
{
    (void) currX;
    (void) currY;
    (void) goalX;
    (void) goalY;

    return 0.0;
}

double
getCost(int currX, int currY, int goalX, int goalY, int heuristic)
{
    if (heuristic == NONEHEURISTIC)
        return zeroHeuristic(currX, currY, goalX, goalY);

    if (heuristic == SLDHEURISTIC)
        return euclideanHeuristic(currX, currY, goalX, goalY);

    if (heuristic == MANHATTANHEURISTIC)
        return manhattanHeuristic(currX, currY, goalX, goalY);

    return zeroHeuristic(currX, currY, goalX, goalY);
}

// Function to be launched in another thread
// Only used to listen for a key and stop the searches mid way through
void
waitForSearch()
{
    SDL_Event event2;

    // the searches will set the doneSearching to be 1
    // so we don't need to worry about breaking or anything
    while (isSearching && !doneSearching) {
        while (SDL_PollEvent(&event2)) {
            switch (event2.type) {
                case SDL_KEYDOWN:
                    if (event2.key.keysym.sym == SDLK_h) {
                        // If the 'h' button is pressed, tell the search to stop
                        isSearching = 0;
                    }
            }
        }
    }
    return;
}
