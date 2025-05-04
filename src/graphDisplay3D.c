#include "graphDisplay.h"
#include <math.h>

Vec3D project3DTo2D(Vec3D point, int screenWidth, int screenHeight, float zoom) {
    float scale = zoom / (zoom + point.z);
    Vec3D projected;
    projected.x = screenWidth / 2 + point.x * scale;
    projected.y = screenHeight / 2 - point.y * scale;
    projected.z = 0;
    return projected;
}

void calculate3DPositions(Graph *graph, Vec3D *positions3D, float radius) {
    int n = graph->numNodes;
    float golden_angle = M_PI * (3 - sqrt(5));

    for (int i = 0; i < n; i++) {
        float y = 1 - (i / (float)(n - 1)) * 2;
        float radius_xy = sqrt(1 - y * y);
        float theta = golden_angle * i;

        positions3D[i].x = radius * cos(theta) * radius_xy;
        positions3D[i].y = radius * y;
        positions3D[i].z = radius * sin(theta) * radius_xy;
    }
}

Vec3D rotateY(Vec3D p, float angle) {
    Vec3D r;
    r.x = cos(angle) * p.x - sin(angle) * p.z;
    r.z = sin(angle) * p.x + cos(angle) * p.z;
    r.y = p.y;
    return r;
}

Vec3D rotateX(Vec3D p, float angle) {
    Vec3D r;
    r.y = cos(angle) * p.y - sin(angle) * p.z;
    r.z = sin(angle) * p.y + cos(angle) * p.z;
    r.x = p.x;
    return r;
}

void drawGraph3D(SDL_Renderer *renderer, Graph *graph, Vec3D *positions3D, RotationState rotation) {
    int screenW, screenH;
    SDL_GetRendererOutputSize(renderer, &screenW, &screenH);

    Vec3D rotated[graph->numNodes];

    for (int i = 0; i < graph->numNodes; i++) {
        Vec3D p = positions3D[i];
        p = rotateX(p, rotation.phi);
        p = rotateY(p, rotation.theta);
        rotated[i] = p;
    }

    for (int i = 0; i < graph->numNodes; i++) {
        for (int j = 0; j < graph->numNodes; j++) {
            if (graph->adjMatrix[i][j]) {
                Vec3D p1 = project3DTo2D(rotated[i], screenW, screenH, 400);
                Vec3D p2 = project3DTo2D(rotated[j], screenW, screenH, 400);
                SDL_RenderDrawLine(renderer, (int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
            }
        }
    }

    for (int i = 0; i < graph->numNodes; i++) {
        Vec3D p = project3DTo2D(rotated[i], screenW, screenH, 400);
        SDL_Rect rect = {(int)p.x - 3, (int)p.y - 3, 6, 6};
        SDL_RenderFillRect(renderer, &rect);
    }
}
