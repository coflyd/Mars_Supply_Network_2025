#ifndef GRAPH_DISPLAY_H
#define GRAPH_DISPLAY_H

#include "graph.h"
#include <SDL2/SDL.h>

typedef struct {
    float x, y, z;
} Vec3D;

typedef struct {
    float theta, phi;
    int lastX, lastY;
    int rotating;
} RotationState;

void drawGraph3D(SDL_Renderer *renderer, Graph *graph, Vec3D *positions3D, RotationState rotation);
Vec3D project3DTo2D(Vec3D point, int screenWidth, int screenHeight, float zoom);
void calculate3DPositions(Graph *graph, Vec3D *positions3D, float radius);

#endif
