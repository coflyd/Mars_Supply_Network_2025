#ifndef MODELS_H_
#define MODELS_H_

#include "structures.h"

void generateNodes(Node nodes[], int numDomes);
void connectNodes(Node nodes[], int numDomes, Edge edges[], int *numEdges);
float geodesicDistance(Node *a, Node *b);
void buildAdjacencyMatrix(int **adjMatrix, int numNodes, Edge *edges,int numEdges);
int getEdgeIndexbetween2Nodes(int nodeIndexStart, int nodeIndexEnd, Edge edge[],int numEdges);
void dijkstra(int **adjMatrix, int numNodes, float *dist, int *prev,Edge *edges, int numEdges, int start);
void printShortestPaths(float *dist, int *prev, int numNodes, Node *nodes);
int buildShortestPathEdges(int *prev, int target, Edge *edges, int numEdges,Edge **shortestPath);

#endif
