#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "display.h"
#include "models.h"
#include "structures.h"
#include "inspection_solver.h"


int main(int argc, char *argv[]) {
	//Initializes the camera
	Display display = { 5, 0, 0, 0, 0, 0 };

	//Create nodes representing the colony's infrastructure
	int numDomes = 5; // Example of number of domes
	Node nodes[100]; // Array to store all nodes
	generateNodes(nodes, numDomes);

	//Connects nodes with routes
	Edge edges[200]; // Array to store all edges
	int numEdges = 0;
	connectNodes(nodes, numDomes, edges, &numEdges);

	//Calculates the total number of nodes
	int totalNodes = numDomes + (3 * numDomes) / 2 + (3 * numDomes) / 2;

	//Calculating and displaying geodetic distances
	for (int i = 0; i < totalNodes; i++) {
		for (int j = 0; j < totalNodes; j++) {
			if (i != j) {
				printf("Distance %d %d %f\n", i, j,
						geodesicDistance(&nodes[i], &nodes[j]));
			}
		}
	}

	//Dynamically allocate an adjacency matrix
	int **adjMatrix = (int**) malloc(totalNodes * sizeof(int*));
	if (!adjMatrix) {
		fprintf(stderr, "Erreur d'allocation mémoire pour la matrice d'adjacence\n");
		return EXIT_FAILURE;
	}

	for (int i = 0; i < totalNodes; i++) {
		adjMatrix[i] = (int*) malloc(totalNodes * sizeof(int));
		if (!adjMatrix[i]) {
			fprintf(stderr, "Erreur d'allocation mémoire pour la ligne %d\n", i);
			// Libérer les allocations précédentes
			for (int j = 0; j < i; j++) {
				free(adjMatrix[j]);
			}
			free(adjMatrix);
			return EXIT_FAILURE;
		}
	}

	//Fill in this matrix according to the connections between nodes
	buildAdjacencyMatrix(adjMatrix, totalNodes, edges, numEdges);
	int startNode = 0;
	int endNode = 8;
	float *dist = (float*) malloc(totalNodes * sizeof(float));
	if (!dist) {
		fprintf(stderr, "Erreur d'allocation pour le tableau dist\n");
		// Libérer les allocations précédentes
		for (int i = 0; i < totalNodes; i++) {
			free(adjMatrix[i]);
		}
		free(adjMatrix);
		return EXIT_FAILURE;
	}

	int *prev = (int*) malloc(totalNodes * sizeof(int));
	if (!prev) {
		fprintf(stderr, "Erreur d'allocation pour le tableau prev\n");
		// Libérer les allocations précédentes
		for (int i = 0; i < totalNodes; i++) {
			free(adjMatrix[i]);
		}
		free(adjMatrix);
		free(dist);
		return EXIT_FAILURE;
	}


	//Calculates the shortest path between two nodes using Dijkstra, and reconstructs the edges of the path
	dijkstra(adjMatrix, totalNodes, dist, prev, edges, numEdges, startNode);
	Edge *shortestPath[100]; // max size
	int pathLength = buildShortestPathEdges(prev, endNode, edges, numEdges, shortestPath);
	for (int i = 0; i < totalNodes; i++) {
		printf("%d %f\n", i, dist[i]);
	}
	printShortestPaths(dist, prev, totalNodes, nodes);
	fflush(stdout);

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	if (TTF_Init() != 0) {
		fprintf(stderr, "Error init TTF : %s\n", TTF_GetError());
		SDL_Quit();
		return EXIT_FAILURE;
	}

	// Configure OpenGL attributes
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	static TTF_Font *font_1 = NULL;
	if (!font_1) {
		//Load font to display text
		font_1 = TTF_OpenFont("data/arial.ttf", 14);
		if (!font_1) {
			fprintf(stderr, "Font loading error : %s\n", TTF_GetError());
			TTF_Quit();
			SDL_Quit();
			return EXIT_FAILURE;
		}
	}

	//Create an OpenGL (3D) window with SDL
	SDL_Window *window = SDL_CreateWindow("Planet 3D", SDL_WINDOWPOS_CENTERED,
	SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	if (!window) {
		fprintf(stderr, "Window could not be created! SDL Error: %s\n", SDL_GetError());
		TTF_CloseFont(font_1);
		TTF_Quit();
		SDL_Quit();
		return EXIT_FAILURE;
	}

	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	if (!glContext) {
		fprintf(stderr, "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		TTF_CloseFont(font_1);
		TTF_Quit();
		SDL_Quit();
		return EXIT_FAILURE;
	}


	//BONUS
	buildAdjacencyMatrix(adjMatrix, totalNodes, edges, numEdges);
	Edge *chemin[MAX_EDGES];
	Node *noeudsVisites[MAX_NODES];
	Edge *nonAtteignables[MAX_EDGES];

	int nbNoeuds = 0, nbNonAtteignables = 0;
	//Calculates the inspection path
	//type of Chinese postman problem displays the routes visited, those inaccessible
	buildInspectionPath(&nodes[0], edges, numEdges, chemin, noeudsVisites, &nbNoeuds, nonAtteignables, &nbNonAtteignables);
	afficherCheminInspectionAvecSens(noeudsVisites, chemin, nbNoeuds);
	afficherArcsInaccessibles(nonAtteignables, nbNonAtteignables);

	//Enable 3D rendering with depth, configure camera
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	gluPerspective(45.0, (1.0 * WINDOW_WIDTH) / WINDOW_HEIGHT, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	UIState ui = { .selectedStart = 0, .selectedEnd = 1, .buttonHover = 0, .showPath = 0 };

	int running = 1;
	display.rotX = -180.0f;
	SDL_Event event;

	// Variables pour limiter le framerate
	Uint32 frameStart;
	const int FPS = 60;
	const int frameDelay = 1000 / FPS;

	//Render loop with event management, drawing of planet, nodes, routes, selected path and display update
	while (running) {
		frameStart = SDL_GetTicks();

		handleEvents(&event, &display, &ui);
		if (ui.showPath) {
			dijkstra(adjMatrix, totalNodes, dist, prev, edges, numEdges, ui.selectedStart);
			pathLength = buildShortestPathEdges(prev, ui.selectedEnd, edges, numEdges, shortestPath);
			ui.showPath = 0;  // reset flag after display
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -display.zoom);
		glRotatef(display.rotX, 1.0f, 0.0f, 0.0f);
		glRotatef(display.rotY, 0.0f, 1.0f, 0.0f);

		drawSphere(1.0f, 30, 30);
		for (int i = 0; i < totalNodes; i++) {
			drawNode(&nodes[i]);
		}

		// Designing the roads after the planet
		for (int i = 0; i < numEdges; i++) {
			drawEdge(&edges[i], 100); // Use 100 segments for a smooth curve
		}

		// Draw the highlighted path
		for (int i = 0; i < pathLength; i++) {
			drawHighlightedEdge(shortestPath[i], 100);
		}

		drawUIOverlay(font_1, nodes, totalNodes, &ui);

		SDL_GL_SwapWindow(window);

		// Limiter la fréquence d'images
		int frameTime = SDL_GetTicks() - frameStart;
		if (frameDelay > frameTime) {
			SDL_Delay(frameDelay - frameTime);
		}
	}

	//Releases all resources on program closure
	free(dist);
	free(prev);

	for (int i = 0; i < totalNodes; i++) {
		free(adjMatrix[i]);
	}
	free(adjMatrix);

	TTF_CloseFont(font_1);
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();

	return 0;
}
