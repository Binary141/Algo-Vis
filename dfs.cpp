#include "search.h"
#include "display.h"
#include <SDL2/SDL.h>
#include <vector>
#include <iostream>
#include <unistd.h> // usleep()
#include <stack>

void dfs(SDL_Renderer* r, SDL_Texture* t, search* s) {
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

    color bg{0,0,0};
    color text{255,255,255};

    int visitedCount = 0;

    while (!q.empty()) {
        if (!isSearching) {
            doneSearching = 1;
            clearStates(s);
            free(visited);
            return;
        }

        int current = q.top();
        q.pop();

        visitedCount += 1;
        drawStatesCount(r, t, bg, text, visitedCount);

        if (s->states[current] == GOAL) {
            printf("Found the goal at index %d!\n", current);
            doneSearching = 1;
            clearStates(s);
            free(visited);
            return;
        }
        if (s->states[current] == WALL || s->states[current] == VISITED) {
            continue;
        }

        // don't mark or color the start state if we come to it
        if (s->states[current] != START) {
            colorTileByIndex(r, t, current, 125, 0, 125);
            s->states[current] = VISITED;
        }
        // sleep for microseconds. 100_000 us is 0.1 seconds
        usleep(SLEEPTIME);

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
    free(visited);
    return;
}
