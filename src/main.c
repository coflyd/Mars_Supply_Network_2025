#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "graphDisplay.h"

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Graphe 3D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Graph *graph = createSampleGraph(); // À implémenter ou charger
    Vec3D *positions3D = malloc(sizeof(Vec3D) * graph->numNodes);
    calculate3DPositions(graph, positions3D, 150);

    RotationState rotation = {0, 0, 0, 0, 0};
    int quit = 0;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = 1;

            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                rotation.rotating = 1;
                rotation.lastX = e.button.x;
                rotation.lastY = e.button.y;
            }

            if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
                rotation.rotating = 0;
            }

            if (e.type == SDL_MOUSEMOTION && rotation.rotating) {
                int dx = e.motion.x - rotation.lastX;
                int dy = e.motion.y - rotation.lastY;
                rotation.theta += dx * 0.01;
                rotation.phi += dy * 0.01;
                rotation.lastX = e.motion.x;
                rotation.lastY = e.motion.y;
            }
        }

        SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 200, 200, 255, 255);
        drawGraph3D(renderer, graph, positions3D, rotation);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
