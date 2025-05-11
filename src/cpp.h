/*#ifndef CPP_SOLVER_H
#define CPP_SOLVER_H

#include "structures.h"

typedef struct {
    int from;
    int to;
    float cost;
} Pairing;

int buildCPPPath(Node *nodes, int totalNodes, Edge *edges, int numEdges, Edge **cppPath, int *cppLength);
// void afficherCheminInspection(Node *nodes, int totalNodes, Edge *edges, int numEdges) ;
void afficherCheminInspectionAvecSens(Edge **cppPath, int pathLength, Node *start) ;
#endif
*/
