#include "structures.h"
#include "stack.h"

#define UNDEF -1

void printWeight(int weight){

    switch (weight){
        case UNDEF:
            printf("\033[45m NDEF \033[m");
            break;
        case FUNCTIONAL:
            printf("\033[42m GOOD \033[m");
            break;
        case DAMAGE:
            printf("\033[43m DMGD \033[m");
            break;
        case DESTROYED:
            printf("\033[41m DSTR \033[m");
            break;
    }

}

void printAdjMatrix(int size, int ** matrix){
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            printWeight(matrix[i][j]);
        }
        printf("\n");
    }
}

void fillMatrix(int size, int *** matrix, int filler){
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            (*matrix)[i][j] = filler;
        }
    }
}

void copyMatrix(int size, int ** toCopy, int *** toHold){
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            (*toHold)[i][j] = toCopy[i][j];
        }
    }
}

void allocMatrix(int size, int *** matrix){
    *matrix = malloc(size * sizeof(int*));
    for (int i = 0; i < size; i++){
        (*matrix)[i] = malloc(size * sizeof(int));
    }
}

void freeMatrix(int size, int *** matrix){
    for (int i = 0; i < size; i++){
        free((*matrix)[i]);
    }
    free(*matrix);
}

// returns the number of nodes
// Explores JUST from the root
// Put NULL as 4th arg if youo don't car about the visited nodes
int DFS(int ** matrix, int size, int root, int ** usedNodesIndex){
    intStack s = CreateStack(size);
    int freeUsed = false;
    if (usedNodesIndex == NULL){
        freeUsed = true;
        usedNodesIndex = malloc(sizeof(int *));
        *usedNodesIndex = malloc(size * sizeof(int));
    }
    for (int i = 0; i < size; i++){
        (*usedNodesIndex)[i] = false;
    }

    int value;
    int nbNodes = 1;


    for (int i = 0; i < size; i++){
        (*usedNodesIndex)[i] = false;
    }

    (*usedNodesIndex)[root] = true;


    for (int i = 0; i < size; i++){
        if (matrix[root][i] != UNDEF){
            Push(&s, i);
            (*usedNodesIndex)[i] = true;
            nbNodes = nbNodes + 1;
        }
    }

    while(Top(s, &value)){
        Pop(&s);

        for (int i = 0; i < size; i++){
            if (matrix[value][i] != UNDEF && (*usedNodesIndex)[i] == false){
                Push(&s, i);
                (*usedNodesIndex)[i] = true;
                nbNodes = nbNodes + 1;
            }
        }
    }
    freeStack(&s);
    if (freeUsed){
        free((*usedNodesIndex));
        free(usedNodesIndex);
    }
    return nbNodes;
}
