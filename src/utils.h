#ifndef __UTILSH__
#define __UTILSH__

void printWeight(int weight);
void printAdjMatrix(int size, int ** matrix);

void fillMatrix(int size, int *** matrix, int filler);
void copyMatrix(int size, int ** toCopy, int *** toHold);

void allocMatrix(int size, int *** matrix);
void freeMatrix(int size, int *** matrix);

int DFS(int ** matrix, int size, int root, int ** usedNodesIndex);

#endif