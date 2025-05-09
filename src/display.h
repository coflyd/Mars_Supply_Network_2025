#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <math.h>
#include <stdlib.h>

#include "structures.h"

void drawSolidSphereGLU(float radius, int slices, int stacks) ;
void drawSphere(float radius, int lats, int longs) ;
void drawNode(Node *node) ;
void drawEdge(Edge *edge, int segments) ;
void handleEvents(SDL_Event *event, Display *display) ;


#endif
