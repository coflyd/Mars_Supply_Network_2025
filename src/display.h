#ifndef DISPLAY_H_
#define DISPLAY_H_
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "structures.h"


void drawSphere(float radius, int lats, int longs);
void drawNode(Node *node);
void drawEdge(Edge *edge, int segments);
void handleEvents(SDL_Event *event, Display *display, UIState *ui);
void drawNodeLabel(Node *node);
GLuint renderTextToTexture(const char *text, TTF_Font *font, SDL_Color color, int *w, int *h);
void drawHighlightedEdge(Edge *edge, int segments) ;
void drawText2D(TTF_Font *font, const char *text, float x, float y, SDL_Color color) ;
void drawUIOverlay(TTF_Font *font, Node *nodes, int totalNodes, UIState *ui) ;


#endif
