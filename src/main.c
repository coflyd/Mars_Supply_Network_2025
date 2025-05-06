#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
// #include <GL/glut.h> // Pour glutSolidSphere


//#include <OpenGL/gl.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>  // Pour gluPerspective
#endif

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "display.h"
#include "models.h"
#include "structures.h"

int main(int argc, char *argv[]) {

	Display display = { 5, 0, 0, 0, 0, 0 };

	int numDomes = 5; // Exemple de nombre de dômes
	Node nodes[100]; // Tableau pour stocker tous les nœuds
	generateNodes(nodes, numDomes);

	Edge edges[200]; // Tableau pour stocker toutes les arêtes
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

	dijkstra(adjMatrix, totalNodes, dist, prev);

	printShortestPaths(dist, prev, totalNodes, nodes);
	fflush(stdout);
	for (int i = 0; i < totalNodes; i++) {
		free(adjMatrix[i]);
	}
	free(adjMatrix);
	free(dist);
	free(prev);

	// Initialiser GLUT
	int glut_argc = 1;
	char *glut_argv[] = { (char*) "GLUT", NULL };
	glutInit(&glut_argc, glut_argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// Initialiser SDL
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("Planète 3D", SDL_WINDOWPOS_CENTERED,
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

		// Dessiner les routes après la planète
		for (int i = 0; i < numEdges; i++) {
			drawEdge(&edges[i], 100); // Utiliser 100 segments pour une courbe lisse
		}

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

