/*
 * models.c
 *
 *  Created on: 5 mai 2025
 *      Author: yves
 */

#include "structures.h"

void generateNodes(Node nodes[], int numDomes) {
	int numResearchStations = (3 * numDomes) / 2;
	int numSupplyDepots = (3 * numDomes) / 2;
	int totalNodes = numDomes + numResearchStations + numSupplyDepots;

	for (int i = 0; i < totalNodes; i++) {
		nodes[i].name = malloc(20 * sizeof(char));
		float theta = (rand() / (float) RAND_MAX) * 2 * M_PI;
		float phi = acos(2 * (rand() / (float) RAND_MAX) - 1);
		nodes[i].x = sin(phi) * cos(theta);
		nodes[i].y = sin(phi) * sin(theta);
		nodes[i].z = cos(phi);
		// nodes[i].name = "City"; // Vous pouvez personnaliser les noms des cités
		nodes[i].index = i; // Ajout de l'index pour identifier les nœuds

		if (i == 0) {
			nodes[i].infrastructure = DOME;
			nodes[i].radius = 0.1f; // Plus grand pour le dôme principal
			sprintf(nodes[i].name, "Dôme_%d", nodes[i].index);

		} else if (i < numDomes) {
			nodes[i].infrastructure = DOME;
			nodes[i].radius = 0.07f; // Plus grand pour les dômes
			sprintf(nodes[i].name, "Dôme_%d", nodes[i].index);
		} else if (i < numDomes + numResearchStations) {
			nodes[i].infrastructure = RESEARCH_STATION;
			nodes[i].radius = 0.05f; // Taille moyenne pour les stations de recherche
			sprintf(nodes[i].name, "Station_%d", nodes[i].index);
		} else {
			nodes[i].infrastructure = SUPPLY_DEPOT;
			nodes[i].radius = 0.03f; // Plus petit pour les dépôts
			sprintf(nodes[i].name, "Dépôt_%d", nodes[i].index);
		}
	}
}

// Fonction pour calculer la distance géodésique entre deux points sur la sphère
float geodesicDistance(Node *a, Node *b) {
	// Calculer le produit scalaire des vecteurs a et b
	float dotProduct = a->x * b->x + a->y * b->y + a->z * b->z;

	// Limiter le produit scalaire entre -1 et 1 pour éviter des erreurs d'arrondi
	dotProduct = fmax(-1.0f, fmin(1.0f, dotProduct));

	// Calculer l'angle entre les vecteurs a et b
	float angle = acos(dotProduct);

	// Retourner la distance géodésique (distance du grand cercle)
	return angle * SPHERE_RADIUS;

}
// Fonction pour construire la matrice d'adjacence
void buildAdjacencyMatrix(int **adjMatrix, int numNodes, Edge *edges,
		int numEdges) {
	// Initialiser la matrice d'adjacence avec des valeurs infinies
	for (int i = 0; i < numNodes; i++) {
		for (int j = 0; j < numNodes; j++) {
			adjMatrix[i][j] = INF;
		}
	}

	// Remplir la matrice d'adjacence avec les poids des arêtes
	for (int i = 0; i < numEdges; i++) {
		int fromIndex = edges[i].start->index;
		int toIndex = edges[i].end->index;
		float distance = geodesicDistance(edges[i].start, edges[i].end);
		if (edges[i].condition == FUNCTIONAL) {
			adjMatrix[fromIndex][toIndex] = distance;
			adjMatrix[toIndex][fromIndex] = distance;
		} else if (edges[i].condition == DAMAGE) {
			adjMatrix[fromIndex][toIndex] = 2 * distance;
			adjMatrix[toIndex][fromIndex] = 2 * distance;
		}
	}
}

// Fonction pour appliquer l'algorithme de Dijkstra
void dijkstra(int **adjMatrix, int numNodes, float *dist, int *prev) {
	int *visited = (int*) malloc(numNodes * sizeof(int));
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
void printShortestPaths(float *dist, int *prev, int numNodes, Node *nodes) {
	printf("Plus courts chemins depuis le sommet 0:\n");
	for (int i = 0; i < numNodes; i++) {
		if (dist[i] == INF) {
			printf("Sommet %d (%s): Non atteignable\n", nodes[i].index,
					nodes[i].name);
		} else {
			printf("Sommet %d (%s): Distance = %f, Chemin = ", nodes[i].index,
					nodes[i].name, dist[i]);
			int j = i;
			while (j != -1) {
				printf("%d (%s) ", nodes[j].index, nodes[j].name);
				j = prev[j];
			}
			printf("\n");
		}
	}
}

