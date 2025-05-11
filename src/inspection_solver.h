#ifndef INSPECTION_SOLVER_H_
#define INSPECTION_SOLVER_H_

int buildInspectionPath(Node *start, Edge *edges, int numEdges, Edge **path, Node **nodesPath, int *pathLength, Edge **inaccessibleEdges, int *inaccessibleCount) ;
void afficherCheminInspectionAvecSens(Node **nodesPath, Edge **path, int pathLen) ;
void afficherArcsInaccessibles(Edge **inaccessibles, int count) ;
#endif
