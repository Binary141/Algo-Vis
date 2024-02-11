#include "search.h"
#include "display.h"
#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <unistd.h> // usleep()
#include <queue>
#include <stack>

int doneSearching = 0;
extern int isSearching;

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

void clearTiles(SDL_Renderer* renderer, search* s, int r, int g, int b) {
    // make sure if we visited a tile, or if it is marked blank, then just clear it out
    for (int i = 0; i < (s->numTiles * s->numTiles); i++) {
        if (s->states[i] == VISITED || s->states[i] == EMPTY_SPACE) {
            s->states[i] = EMPTY_SPACE;
            colorTileByIndex(renderer, i, r, g, b);
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

// caller needs to set doneSearching to 0 if they need
void bfs(SDL_Renderer* r, search* s) {
    color textColor{0, 0, 0};

    color backgroundColor{125, 125, 125};
    // bail out if either start or goal isn't defined
    if (s->goal == EMPTY_SPACE || s->start == EMPTY_SPACE) {
        doneSearching = 1;
        clearStates(s);
        return;
    }

    // get the start index for the last row. Used for a quick lookup in the generate neighbors function
    int lastRowIndex = ((s->numTiles * s->numTiles) - s->numTiles);

    // create a list of visited states. Used to not have repeats
    int* visited = (int*) malloc((s->numTiles * s->numTiles) * sizeof(int));
    for (int i = 0; i < (s->numTiles * s->numTiles); i++) {
        // set all to a defined val. Doesn't matter as long as it isn't equal to VISITED
        visited[i] = 0;

        // If we have visited it before, then just clear it so we can visit it again
        if (s->states[i] == VISITED) {
            s->states[i] = EMPTY_SPACE;
        }
    }

    // vec that get neighbors will return
    std::vector<int> tmpStates;

    std::queue<int> q;

    // FIFO queue we will use
    q.push(s->start);

    while (!q.empty()) {
        if (!isSearching) {
            doneSearching = 1;
            clearStates(s);
            return;
        }

        int current = q.front();
        q.pop();
        if (s->states[current] == GOAL) {
            printf("Found the goal at index %d!\n", current);
            doneSearching = 1;
            clearStates(s);
            return;
        }
        if (s->states[current] == WALL) {
            continue;
        }
        visited[current] = VISITED;

        // don't mark or color the start state if we come to it
        if (s->states[current] != START) {
            colorTileByIndex(r, current, 255, 125, 0);
            s->states[current] = VISITED;
        }
        // sleep for microseconds. 100_000 us is 0.1 seconds
        usleep(100000);

        // if we are on a wall, just remove from the list and skip
        tmpStates = getNeighborIndexes(current, s, lastRowIndex);
        for(int i = 0; i < tmpStates.size(); i++){
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
    doneSearching = 1;
    clearStates(s);
    printf("Could not get to the goal!\n");
    return;
}

void dfs(SDL_Renderer* r, search* s) {
    // bail out if either start or goal isn't defined
    if (s->goal == EMPTY_SPACE || s->start == EMPTY_SPACE) {
        doneSearching = 1;
        return;
    }

    // get the start index for the last row. Used for a quick lookup in the generate neighbors function
    int lastRowIndex = ((s->numTiles * s->numTiles) - s->numTiles);

    // create a list of visited states. Used to not have repeats
    int* visited = (int*) malloc((s->numTiles * s->numTiles) * sizeof(int));
    for (int i = 0; i < (s->numTiles * s->numTiles); i++) {
        // set all to a defined val. Doesn't matter as long as it isn't equal to VISITED
        visited[i] = 0;

        // If we have visited it before, then just clear it so we can visit it again
        if (s->states[i] == VISITED) {
            s->states[i] = EMPTY_SPACE;
        }
    }

    // vec that get neighbors will return
    std::vector<int> tmpStates;

    std::stack<int> q;

    // LIFO queue we will use
    q.push(s->start);

    while (!q.empty()) {
        if (!isSearching) {
            doneSearching = 1;
            clearStates(s);
            return;
        }

        int current = q.top();
        q.pop();
        if (s->states[current] == GOAL) {
            printf("Found the goal at index %d!\n", current);
            doneSearching = 1;
            clearStates(s);
            return;
        }
        if (s->states[current] == WALL || s->states[current] == VISITED) {
            continue;
        }

        // don't mark or color the start state if we come to it
        if (s->states[current] != START) {
            colorTileByIndex(r, current, 125, 0, 125);
            s->states[current] = VISITED;
        }
        // sleep for microseconds. 100_000 us is 0.1 seconds
        usleep(100000);

        visited[current] = VISITED;
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
            // visited[tmpStates[i]] = VISITED;
        }
    }
    doneSearching = 1;
    clearStates(s);
    printf("Could not get to the goal!\n");
    return;
}
