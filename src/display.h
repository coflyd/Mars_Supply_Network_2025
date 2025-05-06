/*
 * display.h
 *
 *  Created on: 5 mai 2025
 *      Author: yves
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "structures.h"

void drawSphere(float radius, int lats, int longs);
void drawNode(Node *node);
void drawEdge(Edge *edge, int segments);
void handleEvents(SDL_Event *event, Display *display);
void connectNodes(Node nodes[], int numDomes, Edge edges[], int *numEdges);

#endif /* DISPLAY_H_ */
