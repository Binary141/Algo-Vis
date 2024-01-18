#include <stdio.h>
#include <stdbool.h>
#include <unistd.h> // usleep()
#include <iostream>
#include "display.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

struct search {
    int start;
    int startx;
    int starty;
    int goal;
    int goalx;
    int goaly;
};



int main() {
    screen disp = init_display();

    SDL_Event event;
    TTF_Init();

    int should_quit = 0;
    int red_iter = 0;
    int blue_iter = 255;

    // draw_grid(disp.renderer);

    search search1;
    search1.start = -1;
    search1.startx = -1;
    search1.starty = -1;
    search1.goal = -1;
    search1.goalx = -1;
    search1.goaly = -1;

    tile closest;
    while (SDL_WaitEvent(&event) && !should_quit) {
        //this opens a font style and sets a size
        TTF_Font* Sans = TTF_OpenFont("OpenSans-Regular.ttf", 24);
        if (!Sans) {
            std::cout << "Error opening font!" << std::endl;
            exit(1);
        }

        // this is the color in rgb format,
        // maxing out all would give you the color white,
        // and it will be your text's color
        SDL_Color White = {255, 255, 255};

        // as TTF_RenderText_Solid could only be used on
        // SDL_Surface then you have to create the surface first
        SDL_Surface* surfaceMessage =
            TTF_RenderText_Solid(Sans, "put your text here", White);

        // now you can convert it into a texture
        SDL_Texture* Message = SDL_CreateTextureFromSurface(disp.renderer, surfaceMessage);

        SDL_Rect Message_rect; //create a rect
        Message_rect.x = 0;  //controls the rect's x coordinate
        Message_rect.y = 0; // controls the rect's y coordinte
        Message_rect.w = 200; // controls the width of the rect
        Message_rect.h = 100; // controls the height of the rect

        // (0,0) is on the top left of the window/screen,
        // think a rect as the text's box,
        // that way it would be very simple to understand

        // Now since it's a texture, you have to put RenderCopy
        // in your game loop area, the area where the whole code executes

        // you put the renderer's name first, the Message,
        // the crop size (you can ignore this if you don't want
        // to dabble with cropping), and the rect which is the size
        // and coordinate of your texture
        SDL_RenderCopy(disp.renderer, Message, NULL, &Message_rect);

        // Update screen
        SDL_RenderPresent(disp.renderer);

        // SDL_BlitSurface(image, NULL, disp.surface, NULL);
        // SDL_UpdateWindowSurface(disp.window);
        // switch (event.type) {
        //     case SDL_QUIT:
        //         should_quit = 1;
        //         break;
        //     case SDL_KEYDOWN:
        //         if (event.key.keysym.sym == SDLK_q) {
        //             // If the 'q' button is pressed, quit the application
        //             should_quit = 1;
        //             break;
        //         }
        //         if (event.key.keysym.sym == SDLK_c) {
        //             // clear the screen
        //             draw_grid(disp.renderer);
        //             continue;
        //         }
        //         if (event.key.keysym.sym == SDLK_s) {
        //             // select the start state
        //             int done = 0;
        //             while (SDL_WaitEvent(&event) && done == 0) {
        //                 switch (event.type) {
        //                     case SDL_MOUSEMOTION:
        //                         continue;
        //                     case SDL_MOUSEBUTTONDOWN:
        //                         int mouse_x, mouse_y;
        //                         SDL_GetMouseState(&mouse_x, &mouse_y);
        //                         if (mouse_y < MENU_HEIGHT) {
        //                             done = 1;
        //                             break;
        //                         }
        //                         closest = getClosestTile(mouse_x, mouse_y);

        //                         if (search1.start == -1) {
        //                             search1.start = (closest.xIndex + (closest.yIndex * NUM_SQUARES));
        //                             search1.startx = closest.x;
        //                             search1.starty = closest.y;
        //                             colorTile(disp.renderer, search1.startx, search1.starty, 0, 255, 0);
        //                         } else {
        //                             colorTile(disp.renderer, search1.startx, search1.starty, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);

        //                             search1.start = (closest.xIndex + (closest.yIndex * NUM_SQUARES));
        //                             search1.startx = closest.x;
        //                             search1.starty = closest.y;

        //                             // TODO look into why display blanks without this. Fast enough it doesn't really matter?
        //                             usleep(5000);
        //                             colorTileByIndex(disp.renderer, search1.start, 0, 255, 0);
        //                         }

        //                         done = 1;
        //                         break;
        //                     case SDL_KEYDOWN:
        //                         done = 1;
        //                         break;
        //                 }
        //             }
        //             continue;
        //         }

        //         if (event.key.keysym.sym == SDLK_g) {
        //             // select the goal state
        //             int done = 0;
        //             while (SDL_WaitEvent(&event) && done == 0) {
        //                 switch (event.type) {
        //                     case SDL_MOUSEMOTION:
        //                         continue;
        //                     case SDL_MOUSEBUTTONDOWN:
        //                         int mouse_x, mouse_y;
        //                         SDL_GetMouseState(&mouse_x, &mouse_y);

        //                         if (mouse_y < MENU_HEIGHT) {
        //                             done = 1;
        //                             break;
        //                         }

        //                         closest = getClosestTile(mouse_x, mouse_y);

        //                         if (search1.goal == -1) {
        //                             // set new goal if it hasn't been set already
        //                             search1.goal = (closest.xIndex + (closest.yIndex * NUM_SQUARES));
        //                             search1.goalx = closest.x;
        //                             search1.goaly = closest.y;
        //                             colorTile(disp.renderer, search1.goalx, search1.goaly, 255, 0, 0);
        //                         } else {
        //                             // a goal has been set before. Clear out the old goal and create the new one
        //                             colorTile(disp.renderer, search1.goalx, search1.goaly, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);

        //                             search1.goal = (closest.xIndex + (closest.yIndex * NUM_SQUARES));
        //                             search1.goalx = closest.x;
        //                             search1.goaly = closest.y;

        //                             // TODO look into why display blanks without this. Fast enough it doesn't really matter?
        //                             usleep(5000);
        //                             colorTileByIndex(disp.renderer, search1.goal, 255, 0, 0);
        //                         }

        //                         done = 1;
        //                         break;
        //                     case SDL_KEYDOWN:
        //                         done = 1;
        //                         break;
        //                 }
        //             }
        //             continue;
        //         }
        //     case SDL_MOUSEBUTTONDOWN:
        //         // generate a wall
        //         int mouse_x, mouse_y;
        //         SDL_GetMouseState(&mouse_x, &mouse_y);
        //         closest = getClosestTile(mouse_x, mouse_y);

        //         // color the tile if it is the left mouse button
        //         if( event.button.button == SDL_BUTTON_LEFT ) {
        //             // don't try to color if it is in the menu bar area
        //             if (mouse_y < MENU_HEIGHT + TILE_BORDER_WIDTH + 1) {
        //                 // +1 comes from adding a little padding just in case
        //                 continue;
        //             }
        //             closest = getClosestTile(mouse_x, mouse_y);

        //             colorTile(disp.renderer, closest.x, closest.y, 0, 0, 255);
        //             continue;
        //         }
        //         if( event.button.button == SDL_BUTTON_RIGHT ) {
        //             // clear the tile if it is the right mouse button
        //             colorTile(disp.renderer, closest.x, closest.y, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B);
        //             continue;
        //         }
        // }
    }
    destroy_window(disp.renderer, disp.window);

    return 0;
}
