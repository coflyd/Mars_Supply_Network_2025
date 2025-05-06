/*
 * models.h
 *
 *  Created on: 5 mai 2025
 *      Author: yves
 */

#ifndef MODELS_H_
#define MODELS_H_

void generateNodes(Node nodes[], int numDomes);
float geodesicDistance(Node *a, Node *b);
void buildAdjacencyMatrix(int **adjMatrix, int numNodes, Edge *edges,
		int numEdges);
void dijkstra(int **adjMatrix, int numNodes, float *dist, int *prev);
void printShortestPaths(float *dist, int *prev, int numNodes, Node *nodes);

#endif /* MODELS_H_ */
