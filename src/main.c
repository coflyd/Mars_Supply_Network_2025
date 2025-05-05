#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


#include "structure.h"
#include "graphDisplay.h"
#include "nodes_Edges.h"

#define bool int
#define true 1
#define false 0

int main(int argc, char *argv[]) {
	srand(time(NULL));

	SDL_Window *win = NULL;
	SDL_Renderer *renderer = NULL;
	// TTF_Font *font[3];
	TTF_Font *font_1;
	TTF_Font *font_2;

	if (initialisation_SDL2(&win, &renderer, &font_1, &font_2) == -1) {
		printf("Erreur d'initialisation\n");
	}


	int numDomeNodes = 10;
	int numResearchStationNodes = 3*numDomeNodes;
	int numDepotNodes = 2*numDomeNodes;
	int numNodes = numDomeNodes + numResearchStationNodes + numDepotNodes;
	int tabNumNodes[4] = {numDomeNodes, numResearchStationNodes, numDepotNodes, numNodes};
	int numEdges = numNodes*3/2;

	Node *nodes = NULL;
	Edge *edges = NULL;

	/*
	if (initialisationNodesEdges(&nodes, numNodes, &edges, numEdges)== -1){
		fprintf(stderr,"Erreur d'initialisation\n");
	}
*/
	initialisationNodesEdges(&nodes, tabNumNodes, &edges, numEdges);

	int **adjMatrix = (int**) malloc(numNodes * sizeof(int*));
	for (int i = 0; i < numNodes; i++) {
		adjMatrix[i] = (int*) malloc(numNodes * sizeof(int));
	}

    // Construire la matrice d'adjacence
    buildAdjacencyMatrix(adjMatrix, numNodes, edges, numEdges);

    // Tableaux pour stocker les distances et les prédécesseurs
    int* dist = (int*)malloc(numNodes * sizeof(int));
    int* prev = (int*)malloc(numNodes * sizeof(int));

    // Appliquer l'algorithme de Dijkstra
    dijkstra(adjMatrix, numNodes, dist, prev);

    // Afficher les plus courts chemins
    printShortestPaths(dist, prev, numNodes, nodes);


	int quit = 0;
	SDL_Event e;
	bool dragging = false;
	Node *draggedNode = NULL;
	int mouseX;
	int mouseY;

	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = 1;
			} else if (e.type == SDL_MOUSEBUTTONDOWN) {
				if (e.button.button == SDL_BUTTON_LEFT) {
					mouseX = e.button.x;
					mouseY = e.button.y;
					for (int i = 0; i < numNodes; i++) {
						if (isPointInNode(mouseX, mouseY, &nodes[i])) {
							nodes[i].isDragging = true;
							draggedNode = &nodes[i];
							dragging = true;
							break;
						}
					}
				}
			} else if (e.type == SDL_MOUSEBUTTONUP) {
				if (e.button.button == SDL_BUTTON_LEFT && dragging) {
					if (draggedNode != NULL) {
						draggedNode->isDragging = false;
						draggedNode = NULL;
					}
					dragging = false;
				}
			} else if (e.type == SDL_MOUSEMOTION && dragging) {
				if (draggedNode != NULL) {
					draggedNode->x = e.motion.x;
					draggedNode->y = e.motion.y;
				}
			}
		}

		// Appliquer l'algorithme de Force-Directed
		applyForceDirectedLayout(nodes, numNodes, numDomeNodes, edges, numEdges);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		// Dessiner les arêtes
		for (int i = 0; i < numEdges; i++) {
			drawEdge(renderer, &edges[i]);
		}

		// Dessiner les nœuds
		for (int i = 0; i < numNodes; i++) {
			drawNode(renderer, &nodes[i], font_1,font_2);
		}

		SDL_RenderPresent(renderer);
		SDL_Delay(16); // Limiter la fréquence de rafraîchissement
	}

	free(nodes);
	free(edges);
	TTF_CloseFont(font_1);
	TTF_CloseFont(font_2);
	TTF_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}
