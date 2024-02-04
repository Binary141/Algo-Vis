#include "search.h"
#include "display.h"
#include <SDL2/SDL.h>
#include <vector>
#include <queue>
#include <iostream>
#include <unistd.h> // usleep()

std::vector<int> getNeighborIndexes(int current, search s) {
    std::vector<int> res;
    if (current < 0) {
        printf("Current was zero while getting neighbors!\n");
        exit(1);
    }

    // get index above current, if applicable
    if (current > s.numTiles) {
        // push only if we are not on the first row
        // printf("Pushing above!\n");
        res.push_back(current - s.numTiles);
    }

    // int thing = current % s.numTiles - 1;
    // printf("current \% numTiles: %d\n", thing);
    // std::cout << "index: " << current << " res: " << thing << std::endl;

    // get index to the right of current, if applicable
    if ((current + 1) % s.numTiles != 0) {
        // push only if we are not on the first row
        // printf("pushing right!\n");
        res.push_back(current + 1);
    }

    // TODO look into this comparison. Might be pretty slow
    // get index below current, if applicable
    if (current <  ((s.numTiles * s.numTiles) - s.numTiles)) {
        // push only if we are not on the first row
        // printf("Pushing above!\n");
        res.push_back(current + s.numTiles);
    }


    // get index to the left of current, if applicable
    if (current % s.numTiles != 0) {
        res.push_back(current - 1);
    }


    return res;
}

void bfs(SDL_Renderer* r, search s) {
    if (s.goal == EMPTY_SPACE || s.start == EMPTY_SPACE) {
        return;
    }
    int VISITED = 1;
    int* visited = (int*) malloc((s.numTiles * s.numTiles) * sizeof(int));
    for (int i = 0; i < (s.numTiles * s.numTiles); i++) {
        visited[i] = 0;
    }


    std::vector<int> tmpStates;
    std::queue<int> q;

    q.push(s.start);

    while (!q.empty()) {
        int current = q.front();
        if (s.states[current] == GOAL) {
            printf("Found the goal at index %d!\n", current);
            return;
        }
        visited[current] = VISITED;

        if (s.states[current] != START) {
            colorTileByIndex(r, current, 125, 125, 0);
        }
        // usleep(100000);

        // if we are on a wall, just remove from the list and skip
        tmpStates = getNeighborIndexes(current, s);
        for(int i=0; i < tmpStates.size(); i++){
            printf("index: %d\n", tmpStates[i]);
            if (s.states[tmpStates[i]] == WALL) {
                continue;
            }

            // uncomment to allow repeat nodes to be explored
            if (visited[tmpStates[i]] == VISITED) {
                continue;
            }

            q.push(tmpStates[i]);
        }

        q.pop();
    }
}
