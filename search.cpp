#include "search.h"
#include "display.h"
#include <SDL2/SDL.h>
#include <vector>
#include <queue>
#include <iostream>
#include <unistd.h> // usleep()

std::vector<int> getNeighborIndexes(int current, const search* s, int lastRowIndex) {
    std::vector<int> res;

    if (current < 0) {
        printf("Current was zero while getting neighbors!\n");
        exit(1);
    }

    // get index above current, if applicable
    if (current > s->numTiles) {
        res.push_back(current - s->numTiles);
    }

    // get index to the right of current, if applicable
    if ((current + 1) % s->numTiles != 0) {
        res.push_back(current + 1);
    }

    // TODO look into this comparison. Might be pretty slow
    // get index below current, if applicable
    // if (current < ((s->numTiles * s->numTiles) - s->numTiles)) {
    if (current <  lastRowIndex) {
        res.push_back(current + s->numTiles);
    }


    // get index to the left of current, if applicable
    if (current % s->numTiles != 0) {
        res.push_back(current - 1);
    }


    return res;
}

void clearVisitedTiles(SDL_Renderer* renderer, search* s, int r, int g, int b) {
    for (int i = 0; i < (s->numTiles * s->numTiles); i++) {
        if (s->states[i] == VISITED) {
            s->states[i] = EMPTY_SPACE;
            colorTileByIndex(renderer, i, r, g, b);
        }
    }
}

void bfs(SDL_Renderer* r, search* s) {
    // bail out if either start or goal isn't defined
    if (s->goal == EMPTY_SPACE || s->start == EMPTY_SPACE) {
        return;
    }

    // get the start index for the last row. Used for a quick lookup in the generate neighbors function
    int lastRowIndex = ((s->numTiles * s->numTiles) - s->numTiles);

    // create a list of visited states. Used to not have repeats
    int* visited = (int*) malloc((s->numTiles * s->numTiles) * sizeof(int));
    for (int i = 0; i < (s->numTiles * s->numTiles); i++) {
        // set all to a defined val. Doesn't matter as long as it isn't equal to VISITED
        visited[i] = 0;
    }

    // vec that get neighbors will return
    std::vector<int> tmpStates;

    std::queue<int> q;

    // FIFO queue we will use
    q.push(s->start);

    while (!q.empty()) {
        int current = q.front();
        q.pop();
        if (s->states[current] == GOAL) {
            printf("Found the goal at index %d!\n", current);
            return;
        }
        if (s->states[current] == WALL) {
            continue;
        }
        visited[current] = VISITED;

        // don't mark or color the start state if we come to it
        if (s->states[current] != START) {
            colorTileByIndex(r, current, 125, 125, 0);
            s->states[current] = VISITED;
        }
        // sleep for microseconds. 100_000 us is 0.1 seconds
        usleep(100000);

        // if we are on a wall, just remove from the list and skip
        tmpStates = getNeighborIndexes(current, s, lastRowIndex);
        for(int i=0; i < tmpStates.size(); i++){
            // the (|| visited[tmpStates[i]] == visited) is to not
            // go indefinitely if there is no path to the goal
            // don't add to states if it is a wall, or we have been there
            if (s->states[tmpStates[i]] == WALL || visited[tmpStates[i]] == VISITED) {
                continue;
            }

            q.push(tmpStates[i]);
            visited[tmpStates[i]] = VISITED;
        }
    }
    printf("Could not get to the goal!\n");
}
