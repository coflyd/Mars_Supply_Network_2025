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


void connectNodes(Node nodes[], int numDomes, Edge edges[], int *numEdges) {
	int numResearchStations = (3 * numDomes) / 2;
	int numSupplyDepots = (3 * numDomes) / 2;

	// Relier chaque dôme aux dômes les plus proches sans créer de routes qui se coupent
	for (int i = 1; i < numDomes; i++) {
		float minDist = INFINITY;
		int closestDomeIndex = -1;
		for (int j = 0; j < numDomes; j++) {
			if (i == j)
				continue; // Ne pas relier un dôme à lui-même
			float dx = nodes[i].x - nodes[j].x;
			float dy = nodes[i].y - nodes[j].y;
			float dz = nodes[i].z - nodes[j].z;
			float dist = sqrt(dx * dx + dy * dy + dz * dz);
			if (dist < minDist) {
				minDist = dist;
				closestDomeIndex = j;
			}
		}
		if (closestDomeIndex != -1) {
			edges[*numEdges] = (Edge ) { &nodes[i], &nodes[closestDomeIndex],
							1.0f, FUNCTIONAL, 1.0f };
			(*numEdges)++;
		}
	}

	// Relier les stations de recherche et les dépôts aux dômes les plus proches
	for (int i = numDomes; i < numDomes + numResearchStations + numSupplyDepots;
			i++) {
		float minDist = INFINITY;
		int closestDomeIndex = -1;
		for (int j = 0; j < numDomes; j++) {
			float dx = nodes[i].x - nodes[j].x;
			float dy = nodes[i].y - nodes[j].y;
			float dz = nodes[i].z - nodes[j].z;
			float dist = sqrt(dx * dx + dy * dy + dz * dz);
			if (dist < minDist) {
				minDist = dist;
				closestDomeIndex = j;
			}
		}
		if (closestDomeIndex != -1) {
			edges[*numEdges] = (Edge ) { &nodes[i], &nodes[closestDomeIndex],
							1.0f, FUNCTIONAL, 1.0f };
			(*numEdges)++;
		}
	}

	// Vérifier la connexité du graphe uniquement entre les dômes
	bool *visited = (bool*) malloc(numDomes * sizeof(bool));
	for (int i = 0; i < numDomes; i++) {
		visited[i] = false;
	}

	// Utiliser un parcours en largeur (BFS) pour vérifier la connexité
	int queue[numDomes];
	int front = 0, rear = 0;
	queue[rear++] = 0; // Commencer par le dôme principal
	visited[0] = true;

	while (front < rear) {
		int current = queue[front++];
		for (int i = 0; i < *numEdges; i++) {
			if ((edges[i].start == &nodes[current]
					|| edges[i].end == &nodes[current])
					&& edges[i].start->infrastructure == DOME
					&& edges[i].end->infrastructure == DOME) {
				Node *neighbor =
						(edges[i].start == &nodes[current]) ?
								edges[i].end : edges[i].start;
				int neighborIndex = neighbor - nodes;
				if (!visited[neighborIndex]) {
					visited[neighborIndex] = true;
					queue[rear++] = neighborIndex;
				}
			}
		}
	}

	// Si des dômes ne sont pas visités, les relier au dôme principal
	for (int i = 0; i < numDomes; i++) {
		if (!visited[i]) {
			edges[*numEdges] = (Edge ) { &nodes[i], &nodes[0], 1.0f, FUNCTIONAL,
							1.0f };
			(*numEdges)++;
		}
	}

	free(visited);

	// Attribuer des états aux routes selon la répartition souhaitée
	int totalEdges = *numEdges;
	int functionalCount = (int) (0.5 * totalEdges);
	int damageCount = (int) (0.3 * totalEdges);
	int  destroyedCount = totalEdges - functionalCount - damageCount;

	for (int i = 0; i < totalEdges; i++) {
		edges[i].condition = FUNCTIONAL;
	}
	int i =0;
	while(i<damageCount){
		int r = rand()%totalEdges;
		if (edges[r].condition == FUNCTIONAL){
			edges[r].condition = DAMAGE;
			i +=1;
		}
	}

	i =0;
	while(i<destroyedCount){
		int r = rand()%totalEdges;
		if (edges[r].condition == FUNCTIONAL){
			edges[r].condition = DESTROYED;
			i +=1;
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
			adjMatrix[i][j] = 0;
		}
	}
	// Remplir la matrice d'adjacence
	for (int i = 0; i < numEdges; i++) {
		int fromIndex = edges[i].start->index;
		int toIndex = edges[i].end->index;
		if (edges[i].condition != DESTROYED) {
			adjMatrix[fromIndex][toIndex] = 1;
			adjMatrix[toIndex][fromIndex] = 1;
		}
	}
}

int getEdgeIndexbetween2Nodes(int nodeIndexStart, int nodeIndexEnd, Edge *edge,
		int numEdges) {
	for (int i = 0; i < numEdges; i++) {
		while (!((edge[i].start->index == nodeIndexStart
				&& edge[i].end->index == nodeIndexEnd)
				|| (edge[i].start->index == nodeIndexEnd
						&& edge[i].end->index == nodeIndexStart))) {
			i += 1;
		}
		if (i < numEdges) {
			return (i);
		}
	}
	return (-1);
}

// void dijkstra(int **adjMatrix, int numNodes, float *dist, int *prev, Edge *edges, int numEdges)
void dijkstra(int **adjMatrix, int numNodes, float *dist, int *prev, Edge *edges, int numEdges, int start)

{
    int *visited = (int *)malloc(numNodes * sizeof(int));
    for (int i = 0; i < numNodes; i++) {
        dist[i] = INF;
        prev[i] = -1;
        visited[i] = 0;
    }
    dist[start] = 0; // Distance from node 0 to itself is 0

    for (int count = 0; count < numNodes - 1; count++) {
        int u = -1;
        // Find the unvisited node with the smallest distance
        for (int i = 0; i < numNodes; i++) {
            if (!visited[i] && (u == -1 || dist[i] < dist[u])) {
                u = i;
            }
        }
        visited[u] = 1;
        // Update the distances of the adjacent nodes
        for (int v = 0; v < numNodes; v++) {
        	// printf("adjMatrix[%d][%d] = %d\n", u, v, adjMatrix[u][v]);
            if (adjMatrix[u][v] != 0 && !visited[v]) {
                float newDist = dist[u];
                for (int i = 0; i < numEdges; i++) {
                    if (  (edges[i].start->index == u && edges[i].end->index == v) ||
                        (edges[i].start->index == v && edges[i].end->index == u)) {
                    	// printf("dist géo edge %d : %f\n",i, geodesicDistance(edges[i].start, edges[i].end));
                        if (edges[i].condition == FUNCTIONAL) {
                            newDist +=geodesicDistance(edges[i].start, edges[i].end);
                        } else if (edges[i].condition == DAMAGE) {
                            newDist += 2 * geodesicDistance(edges[i].start, edges[i].end);
                        }else{
                        	newDist = INF;
                        }
                        // printf("New dist : %f <?  dist[v] : %f\n", newDist, dist[v]);
                        break;
                    }
                }

                if (newDist < dist[v]) {
                	// printf("New dist node %d :: %f <!  dist[v] : %f\n",v,  newDist, dist[v]);
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
			printf("Sommet %d (%s): Distance = %.2f, Chemin = ", nodes[i].index,
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

int buildShortestPathEdges(int *prev, int target, Edge *edges, int numEdges, Edge **shortestPath) {
    int count = 0;
    int current = target;

    while (prev[current] != -1) {
        int idx = getEdgeIndexbetween2Nodes(current, prev[current], edges, numEdges);
        if (idx != -1) {
            shortestPath[count++] = &edges[idx];
        }
        current = prev[current];
    }

    return count; // nombre d'arêtes dans le chemin
}
