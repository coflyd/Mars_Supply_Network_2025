
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "structure.h"

#define bool int
#define true 1
#define false 0

#define NODE_RADIUS 20
#define SPRING_LENGTH 300
#define REPULSION_FORCE 3000
#define SPRING_CONSTANT 0.005
#define DAMPING 0.8
#define MAX_ITERATIONS 1000
#define SCALE_FACTOR 0.1


void drawCircle(SDL_Renderer *renderer, int x, int y, int radius) {
	int offsetx, offsety, d;
	int x_center = x;
	int y_center = y;

	offsetx = 0;
	offsety = radius;
	d = 1 - radius;

	while (offsety >= offsetx) {
		SDL_RenderDrawPoint(renderer, x_center + offsetx, y_center + offsety);
		SDL_RenderDrawPoint(renderer, x_center + offsetx, y_center - offsety);
		SDL_RenderDrawPoint(renderer, x_center - offsetx, y_center + offsety);
		SDL_RenderDrawPoint(renderer, x_center - offsetx, y_center - offsety);
		SDL_RenderDrawPoint(renderer, x_center + offsety, y_center + offsetx);
		SDL_RenderDrawPoint(renderer, x_center + offsety, y_center - offsetx);
		SDL_RenderDrawPoint(renderer, x_center - offsety, y_center + offsetx);
		SDL_RenderDrawPoint(renderer, x_center - offsety, y_center - offsetx);

		if (d < 0) {
			d += 2 * offsetx + 3;
		} else {
			d += 2 * (offsetx - offsety) + 5;
			offsety--;
		}
		offsetx++;
	}
}

int initialisation_SDL2(SDL_Window **win, SDL_Renderer **renderer,
		TTF_Font **font_1, TTF_Font **font_2) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("SDL_Init Error: %s\n", SDL_GetError());
		return 1;
	}

	*win = SDL_CreateWindow("Mars colony map", 100, 100, WINDOW_WIDTH,
	WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (*win == NULL) {
		printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	*renderer = SDL_CreateRenderer(*win, -1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (*renderer == NULL) {
		SDL_DestroyWindow(*win);
		printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	// Initialiser TTF (TrueType Font)
	if (TTF_Init() == -1) {
		printf("TTF_Init Error: %s\n", TTF_GetError());
		SDL_DestroyRenderer(*renderer);
		SDL_DestroyWindow(*win);
		SDL_Quit();
		return 1;
	}

	// Charger une police
	*font_1 = TTF_OpenFont("src/data/arial.ttf", 24);
	if (*font_1 == NULL) {
		printf("TTF_OpenFont Error: %s\n", TTF_GetError());
		TTF_Quit();
		SDL_DestroyRenderer(*renderer);
		SDL_DestroyWindow(*win);
		SDL_Quit();
		return 1;
	}

	*font_2 = TTF_OpenFont("src/data/arial.ttf", 14);
	if (*font_2 == NULL) {
		printf("TTF_OpenFont Error: %s\n", TTF_GetError());
		TTF_Quit();
		SDL_DestroyRenderer(*renderer);
		SDL_DestroyWindow(*win);
		SDL_Quit();
		return 1;
	}
	return 0;
}

// Fonction pour dessiner un nœud
void drawNode(SDL_Renderer *renderer, Node *node, TTF_Font *font_1,
		TTF_Font *font_2) {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	drawCircle(renderer, node->x, node->y, node->radius);

	// Dessiner le label du nœud
	SDL_Color color = { 255, 255, 255, 255 };
	SDL_Surface *surface = NULL;
	switch (node->infrastructure) {
	case 0:
		surface = TTF_RenderText_Solid(font_1, node->label, color);
		break;
	case 1:
	case 2:
		surface = TTF_RenderText_Solid(font_2, node->label, color);
		break;
	}

	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	int textWidth = surface->w;
	int textHeight = surface->h;
	SDL_Rect textRect = { node->x - (textWidth / 2), node->y - (textHeight / 2),
			textWidth, textHeight };
	SDL_RenderCopy(renderer, texture, NULL, &textRect);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

// Fonction pour dessiner une arête
void drawEdge(SDL_Renderer *renderer, Edge *edge) {
	// SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	if (edge->condition == FUNCTIONAL) {
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Vert
	} else if (edge->condition == DAMAGE) {
		SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255); // Orange
	} else if (edge->condition == DESTROYED) {
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rouge
	}
	SDL_RenderDrawLine(renderer, edge->from->x, edge->from->y, edge->to->x,
			edge->to->y);
}

// Fonction pour vérifier si un point est dans un nœud
bool isPointInNode(int x, int y, Node *node) {
	int dx = x - node->x;
	int dy = y - node->y;
	return (dx * dx + dy * dy) <= (NODE_RADIUS * NODE_RADIUS);
}

// Fonction pour appliquer l'algorithme de Force-Directed
void applyForceDirectedLayout(Node *nodes, int numNodes, int numDomeNodes,
		Edge *edges, int numEdges) {
	for (int i = 0; i < numNodes; i++) {
		nodes[i].vx = 0;
		nodes[i].vy = 0;
	}

	// Calculer les forces de répulsion
	for (int i = 0; i < numNodes; i++) {
		for (int j = 0; j < numNodes; j++) {
			if (i != j) {
				int dx = nodes[j].x - nodes[i].x;
				int dy = nodes[j].y - nodes[i].y;
				double distance = sqrt(dx * dx + dy * dy);
				if (distance > 0) {
					double force = REPULSION_FORCE / (distance * distance);
					if (i < numDomeNodes && j < numDomeNodes) {
						force *= 20;
					}
					nodes[i].vx += (dx / distance) * force;
					nodes[i].vy += (dy / distance) * force;
				}
			}
		}
	}

	// Calculer les forces d'attraction (ressorts)
	for (int i = 0; i < numEdges; i++) {
		int dx = edges[i].to->x - edges[i].from->x;
		int dy = edges[i].to->y - edges[i].from->y;
		double distance = sqrt(dx * dx + dy * dy);
		double force = SPRING_CONSTANT * (distance - SPRING_LENGTH);
		edges[i].from->vx += (dx / distance) * force;
		edges[i].from->vy += (dy / distance) * force;
		edges[i].to->vx -= (dx / distance) * force;
		edges[i].to->vy -= (dy / distance) * force;
	}

	// Mettre à jour les positions des nœuds
	for (int i = 0; i < numNodes; i++) {
		nodes[i].x += nodes[i].vx * DAMPING;
		nodes[i].y += nodes[i].vy * DAMPING;
		// Limiter les nœuds à l'intérieur de la fenêtre
		if (nodes[i].x < NODE_RADIUS)
			nodes[i].x = NODE_RADIUS;
		if (nodes[i].x > WINDOW_WIDTH - NODE_RADIUS)
			nodes[i].x = WINDOW_WIDTH - NODE_RADIUS;
		if (nodes[i].y < NODE_RADIUS)
			nodes[i].y = NODE_RADIUS;
		if (nodes[i].y > WINDOW_HEIGHT - NODE_RADIUS)
			nodes[i].y = WINDOW_HEIGHT - NODE_RADIUS;
	}
}

/*
 // Fonction pour appliquer l'algorithme de Force-Directed
 void applyForceDirectedLayout(Node* nodes, int numNodes, Edge* edges, int numEdges) {
 int iterations = 0;
 while (iterations < MAX_ITERATIONS) {
 for (int i = 0; i < numNodes; i++) {
 nodes[i].vx = 0;
 nodes[i].vy = 0;
 }

 // Calculer les forces de répulsion
 for (int i = 0; i < numNodes; i++) {
 for (int j = 0; j < numNodes; j++) {
 if (i != j) {
 int dx = nodes[j].x - nodes[i].x;
 int dy = nodes[j].y - nodes[i].y;
 double distance = sqrt(dx * dx + dy * dy);
 if (distance > 0) {
 double force = REPULSION_FORCE * (1 + SCALE_FACTOR * (nodes[i].radius + nodes[j].radius)) / (distance * distance);
 nodes[i].vx += (dx / distance) * force;
 nodes[i].vy += (dy / distance) * force;
 }
 }
 }
 }

 // Calculer les forces d'attraction (ressorts)
 for (int i = 0; i < numEdges; i++) {
 int dx = edges[i].to->x - edges[i].from->x;
 int dy = edges[i].to->y - edges[i].from->y;
 double distance = sqrt(dx * dx + dy * dy);
 double force = SPRING_CONSTANT * (distance - SPRING_LENGTH) * (1 + SCALE_FACTOR * (edges[i].from->radius + edges[i].to->radius));
 edges[i].from->vx += (dx / distance) * force;
 edges[i].from->vy += (dy / distance) * force;
 edges[i].to->vx -= (dx / distance) * force;
 edges[i].to->vy -= (dy / distance) * force;
 }

 // Mettre à jour les positions des nœuds
 for (int i = 0; i < numNodes; i++) {
 nodes[i].x += nodes[i].vx * DAMPING;
 nodes[i].y += nodes[i].vy * DAMPING;
 // Limiter les nœuds à l'intérieur de la fenêtre
 if (nodes[i].x < nodes[i].radius) nodes[i].x = nodes[i].radius;
 if (nodes[i].x > WINDOW_WIDTH - nodes[i].radius) nodes[i].x = WINDOW_WIDTH - nodes[i].radius;
 if (nodes[i].y < nodes[i].radius) nodes[i].y = nodes[i].radius;
 if (nodes[i].y > WINDOW_HEIGHT - nodes[i].radius) nodes[i].y = WINDOW_HEIGHT - nodes[i].radius;
 }

 iterations++;
 }
 }
 */
