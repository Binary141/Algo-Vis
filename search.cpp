#include "search.h"
#include "display.h"
#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <unistd.h> // usleep()

int doneSearching = 0;

std::vector<int> getNeighborIndexes(int current, const search* s, int lastRowIndex) {
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

void clearTiles(SDL_Renderer* renderer, SDL_Texture* texture, search* s, int r, int g, int b) {
    // make sure if we visited a tile, or if it is marked blank, then just clear it out
    int tmp;
    draw_grid(renderer, texture);

    for (int i = 0; i < (s->numTiles * s->numTiles); i++) {
        tmp = s->states[i];
        if (tmp == VISITED) {
            continue;
        }

        if (tmp == WALL) { // || tmp == EMPTY_SPACE) {
            // s->states[i] = EMPTY_SPACE;
            colorTileByIndex(renderer, texture, i, 0, 0, 255);
        }

        if (tmp == GOAL) { // || tmp == empty_space) {
            // s->states[i] = empty_space;
            colorTileByIndex(renderer, texture, i, 255, 0, 0);
        }

        if (tmp == START) { // || tmp == empty_space) {
            // s->states[i] = empty_space;
            colorTileByIndex(renderer, texture, i, 0, 255, 0);
        }
    }
}

void clearStates(search* s) {
    for (int i = 0; i < (s->numTiles * s->numTiles); i++) {
        // set the tile to be empty if it has been visited
        // note this will not update the display, just used so
        // if a search was stopped, it can search properly
        if (s->states[i] == VISITED) {
            s->states[i] = EMPTY_SPACE;
        }
    }
}

