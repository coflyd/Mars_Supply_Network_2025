#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "display.h"
#include "models.h"
#include "structures.h"

int main(int argc, char *argv[]) {
	Display display = { 5, 0, 0, 0, 0, 0 };

	int numDomes = 5;
	Node nodes[100];
	generateNodes(nodes, numDomes);

	Edge edges[200];
	int numEdges = 0;
	connectNodes(nodes, numDomes, edges, &numEdges);

	int totalNodes = numDomes + (3 * numDomes) / 2 + (3 * numDomes) / 2;

	int **adjMatrix = (int**) malloc(totalNodes * sizeof(int*));
	for (int i = 0; i < totalNodes; i++) {
		adjMatrix[i] = (int*) malloc(totalNodes * sizeof(int));
	}

	buildAdjacencyMatrix(adjMatrix, totalNodes, edges, numEdges);

	float *dist = (float*) malloc(totalNodes * sizeof(float));
	int *prev = (int*) malloc(totalNodes * sizeof(int));

	dijkstra(adjMatrix, totalNodes, dist, prev, edges, numEdges);
	printShortestPaths(dist, prev, totalNodes, nodes);

	for (int i = 0; i < totalNodes; i++) {
		free(adjMatrix[i]);
	}
	free(adjMatrix);
	free(dist);
	free(prev);

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("PlanÃ¨te 3D", SDL_WINDOWPOS_CENTERED,
	SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext glContext = SDL_GL_CreateContext(window);

	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	gluPerspective(45.0, 800.0 / 600.0, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);

	int running = 1;
	SDL_Event event;
	while (running) {
		handleEvents(&event, &display);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -display.zoom);
		glRotatef(display.rotX, 1.0f, 0.0f, 0.0f);
		glRotatef(display.rotY, 0.0f, 1.0f, 0.0f);

		drawSphere(1.0f, 30, 30);
		for (int i = 0; i < totalNodes; i++) {
			drawNode(&nodes[i]);
		}
		for (int i = 0; i < numEdges; i++) {
			drawEdge(&edges[i], 100);
		}

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
