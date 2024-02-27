#include "search.h"
#include "display.h"
#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <unistd.h> // usleep()
#include <queue>
#include <math.h> // sqrt()
#include <float.h>

struct astarStruct {
    int Index;
    int x;
    int y;
    int Cost;
    double heuristic;
    bool operator<(const astarStruct& rhs) const {
        return heuristic > rhs.heuristic;
    }
};

// caller needs to set doneSearching to 0 if they need
void astar(SDL_Renderer* r, SDL_Texture* t, search* s) {
    // bail out if either start or goal isn't defined
    if (s->goal == EMPTY_SPACE || s->start == EMPTY_SPACE) {
        doneSearching = 1;
        clearStates(s);
        return;
    }

    // get the start index for the last row. Used for a quick lookup in the generate neighbors function
    int lastRowIndex = ((s->numTiles * s->numTiles) - s->numTiles);

    // create a list of visited states. Used to not have repeats
    double* visited = (double*) malloc((s->numTiles * s->numTiles) * sizeof(double));
    for (int i = 0; i < (s->numTiles * s->numTiles); i++) {
        // set all to a defined val. Doesn't matter as long as it isn't equal to VISITED
        visited[i] = DBL_MAX;

        // If we have visited it before, then just clear it so we can visit it again
        if (s->states[i] == VISITED) {
            s->states[i] = EMPTY_SPACE;
        }
    }

    // vec that get neighbors will return
    std::vector<int> tmpStates;

    // prio queue we will use
    std::priority_queue<astarStruct> pq;

    astarStruct as;

    as.Index = s->start;

    as.x = s->startx;
    as.y = s->starty;
    as.Cost = 0;

    // the goal doesn't move while we search
    int goalX = s->goal % setting.numTiles;
    int goalY = s->goal / setting.numTiles;

    as.heuristic = getCost(s->startx, s->starty, goalX, goalY);
    pq.push(as);

    int visitedCount = 0;

    while (!pq.empty()) {
        if (!isSearching) {
            doneSearching = 1;
            clearStates(s);
            free(visited);
            return;
        }

        astarStruct curr = pq.top();
        pq.pop();
        int current = curr.Index;

        if (s->states[current] == GOAL) {
            printf("Found the goal at index %d!\n", current);
            doneSearching = 1;
            clearStates(s);
            free(visited);
            return;
        }
        if (s->states[current] == WALL) {
            continue;
        }

        // don't mark or color the start state if we come to it
        if (s->states[current] != START) {
            visitedCount += 1;

            colorTileByIndex(r, t, current, 255, 0, 255, 1);
            drawStatesCount(r, t, textColor, bg, visitedCount);

            s->states[current] = VISITED;
        }

        // sleep for microseconds. 100_000 us is 0.1 seconds
        usleep(SLEEPTIME);

        // if we are on a wall, just remove from the list and skip
        tmpStates = getNeighborIndexes(current, s, lastRowIndex);
        for(int i = 0; i < tmpStates.size(); i++){
            int tmpState = tmpStates[i];

            int x = tmpState % setting.numTiles;
            int y = tmpState / setting.numTiles;

            double heuristic = getCost(x, y, goalX, goalY);

            // the (|| visited[tmpStates[i]] == visited) is to not
            // go indefinitely if there is no path to the goal
            // don't add to states if it is a wall, or we have been there
            if (s->states[tmpState] == WALL || visited[tmpStates[i]] != DBL_MAX) {
                // if (heuristic > visited[tmpStates[i]]) {
                //     continue;
                // }
                continue;
            }

            astarStruct tmp;
            tmp.Index = tmpState;

            // Cost of parent + 1 since our step Cost is 1
            tmp.Cost = curr.Cost + 1;
            tmp.heuristic = heuristic + tmp.Cost;

            pq.push(tmp);
            visited[tmpState] = heuristic;
        }
    }

    doneSearching = 1;
    clearStates(s);
    printf("Could not get to the goal!\n");
    free(visited);
    return;
}
