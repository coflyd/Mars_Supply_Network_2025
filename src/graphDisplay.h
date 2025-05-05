
#pragma once

#define bool int
#define true 1
#define false 0

int initialisation_SDL2(SDL_Window **win, SDL_Renderer **renderer,
		TTF_Font **font_1, TTF_Font **font_2);

// Fonction pour dessiner un nœud
void drawNode(SDL_Renderer *renderer, Node *node, TTF_Font *font_1, TTF_Font *font_2);
// Fonction pour dessiner une arête
void drawEdge(SDL_Renderer *renderer, Edge *edge);
// Fonction pour vérifier si un point est dans un nœud
bool isPointInNode(int x, int y, Node *node);
// Fonction pour appliquer l'algorithme de Force-Directed
void applyForceDirectedLayout(Node *nodes, int numNodes,int numDomeNodes,  Edge *edges,
		int numEdges);
void drawCircle(SDL_Renderer* renderer, int x, int y, int radius) ;
