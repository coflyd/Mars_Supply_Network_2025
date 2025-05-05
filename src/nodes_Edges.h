
#pragma once

int initialisationNodesEdges(Node **nodes, int numNodes[4], Edge **edges,
		int numEdges);
void buildAdjacencyMatrix(int **adjMatrix, int numNodes, Edge *edges,
		int numEdges);
void dijkstra(int **adjMatrix, int numNodes, int *dist, int *prev);
void printShortestPaths(int* dist, int* prev, int numNodes, Node* nodes) ;
