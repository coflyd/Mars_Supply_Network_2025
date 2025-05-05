
#include <limits.h>

#include "structure.h"

#define INF INT_MAX

int initialisationNodesEdges(Node **nodes, int numNodes[4], Edge **edges,
		int numEdges) {
	// tabNumNodes[4] = {numDomeNodes, numResearchStationNodes, numDepotNodes, numNodes}
	int totalNumNodes = numNodes[3];
	*nodes = (Node*) malloc(totalNumNodes * sizeof(Node));
	*edges = (Edge*) malloc(numEdges * sizeof(Edge));
	if (*nodes == NULL || *edges == NULL) {
		return (-1);
	}

	// Initialiser les nœuds avec des positions aléatoires
	for (int i = 0; i < totalNumNodes; i++) {
		(*nodes)[i].index = i;
		(*nodes)[i].x = rand() % WINDOW_WIDTH;
		(*nodes)[i].y = rand() % WINDOW_HEIGHT;
		(*nodes)[i].vx = 0;
		(*nodes)[i].vy = 0;
		(*nodes)[i].isDragging = false;
		if (i < numNodes[0]) { // Dome
			(*nodes)[i].infrastructure = 0;
			(*nodes)[i].radius = 40;
			if (i == 0) {
				(*nodes)[0].radius = 60;
			}
			sprintf((*nodes)[i].label, "D-%d", i);
		} else if (i <numNodes[0] + numNodes[1]) { // Research Station
			(*nodes)[i].infrastructure = 1;
			(*nodes)[i].radius = 20;
			sprintf((*nodes)[i].label, "RS-%d", i - numNodes[0]);
		} else {
			(*nodes)[i].infrastructure = 2;
			(*nodes)[i].radius = 15;
			sprintf((*nodes)[i].label, "d-%d", i - (numNodes[0] + numNodes[1]));
		}
	}
	int condition;
	int edgeCount = 0;

	// joindre les Domes entre eux
	for (int i = 0; i < totalNumNodes; i++) {
		int toIndex = rand() % numNodes[0];
		while (toIndex == i) {
			toIndex = rand() % numNodes[0];
		}
		(*edges)[edgeCount].from = &(*nodes)[i];
		(*edges)[edgeCount].to = &(*nodes)[toIndex];
		(*edges)[edgeCount].distance = 50 + rand() % 50;
		// (*edges)[edgeCount].condition = rand() % 3;
		(*edges)[edgeCount].capacity = rand() % 10 + 1;

		condition = rand() % 10;
		if (condition < 7) {
			condition = 0;
		} else if (condition < 8) {
			condition = 1;
		} else {
			condition = 2;
		}
		(*edges)[edgeCount].condition = condition;

		edgeCount++;
	}

	for (int i = edgeCount; i < numEdges; i++) {
		int fromIndex = numNodes[0] + rand() % (totalNumNodes - numNodes[0]);
		int toIndex = numNodes[0] + rand() % (totalNumNodes - numNodes[0]);

		while (fromIndex == toIndex) {
			toIndex = numNodes[0] + rand() % (totalNumNodes - numNodes[0]);
		}

		(*edges)[i].from = &(*nodes)[fromIndex];
		(*edges)[i].to = &(*nodes)[toIndex];
		(*edges)[i].distance = 20 + rand() % 20;

		(*edges)[i].capacity = rand() % 10 + 1;

		condition = rand() % 10;
		if (condition < 7) {
			condition = 0;
		} else if (condition < 8) {
			condition = 1;
		} else {
			condition = 2;
		}
		(*edges)[edgeCount].condition = condition;
	}
	/*
	 // Initialiser les arêtes de manière aléatoire
	 for (int i = 0; i < numEdges; i++) {
	 int fromIndex = rand() % numNodes;
	 int toIndex = rand() % numNodes;
	 while (fromIndex == toIndex) {
	 toIndex = rand() % numNodes;
	 }

	 (*edges)[i].from = &(*nodes)[fromIndex];
	 (*edges)[i].to = &(*nodes)[toIndex];

	 condition = rand() % 10;
	 if (condition < 7) {
	 condition = 0;
	 } else if (condition < 8) {
	 condition = 1;
	 } else {
	 condition = 2;
	 }
	 (*edges)[i].condition = condition;
	 }
	 */
	return (0);
}


// Fonction pour construire la matrice d'adjacence
void buildAdjacencyMatrix(int** adjMatrix, int numNodes, Edge* edges, int numEdges) {
    // Initialiser la matrice d'adjacence avec des valeurs infinies
    for (int i = 0; i < numNodes; i++) {
        for (int j = 0; j < numNodes; j++) {
            adjMatrix[i][j] = INF;
        }
    }

    // Remplir la matrice d'adjacence avec les poids des arêtes
    for (int i = 0; i < numEdges; i++) {
        int fromIndex = edges[i].from->index;
        int toIndex = edges[i].to->index;
        if (edges[i].condition == FUNCTIONAL) {
            adjMatrix[fromIndex][toIndex] = 1;
            adjMatrix[toIndex][fromIndex] = 1;
        } else if (edges[i].condition == DAMAGE) {
            adjMatrix[fromIndex][toIndex] = 2;
            adjMatrix[toIndex][fromIndex] = 2;
        }
    }
}

// Fonction pour appliquer l'algorithme de Dijkstra
void dijkstra(int** adjMatrix, int numNodes, int* dist, int* prev) {
    int* visited = (int*)malloc(numNodes * sizeof(int));
    for (int i = 0; i < numNodes; i++) {
        dist[i] = INF;
        prev[i] = -1;
        visited[i] = 0;
    }
    dist[0] = 0; // Distance du sommet d'indice 0 à lui-même est 0

    for (int count = 0; count < numNodes - 1; count++) {
        int u = -1;
        // Trouver le sommet non visité avec la distance minimale
        for (int i = 0; i < numNodes; i++) {
            if (!visited[i] && (u == -1 || dist[i] < dist[u])) {
                u = i;
            }
        }

        visited[u] = 1;

        // Mettre à jour les distances des sommets adjacents
        for (int v = 0; v < numNodes; v++) {
            if (adjMatrix[u][v] != INF && !visited[v]) {
                int newDist = dist[u] + adjMatrix[u][v];
                if (newDist < dist[v]) {
                    dist[v] = newDist;
                    prev[v] = u;
                }
            }
        }
    }

    free(visited);
}

// Fonction pour afficher les plus courts chemins
void printShortestPaths(int* dist, int* prev, int numNodes, Node* nodes) {
    printf("Plus courts chemins depuis le sommet 0:\n");
    for (int i = 0; i < numNodes; i++) {
        if (dist[i] == INF) {
            printf("Sommet %d (%s): Non atteignable\n", nodes[i].index, nodes[i].label);
        } else {
            printf("Sommet %d (%s): Distance = %d, Chemin = ", nodes[i].index, nodes[i].label, dist[i]);
            int j = i;
            while (j != -1) {
                printf("%d (%s) ", nodes[j].index, nodes[j].label);
                j = prev[j];
            }
            printf("\n");
        }
    }
}

