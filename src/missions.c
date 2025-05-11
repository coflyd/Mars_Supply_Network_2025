#include "structures.h"
#include "utils.h"

#define UNDEF -1


//--------------------MISSION 1------------------------------//
void mission1Aux(int** adjMatrix, int numNodes, int* usedNodes, int actualNode)
{
	usedNodes[actualNode] = true;
	for(int i = 0; i<numNodes; i++)
	{
		if((adjMatrix[actualNode][i] == FUNCTIONAL || adjMatrix[actualNode][i] == DAMAGE) && usedNodes[i] == false)
		{
			mission1Aux(adjMatrix, numNodes, usedNodes, i);
		}
	}
}

void mission1(int** adjMatrix, int numNodes, Node* nodes)
{
    int usedNodes[numNodes];
	for(int i = 0; i<numNodes; i++)
	{
		usedNodes[i] = false;
	}
	mission1Aux(adjMatrix, numNodes, usedNodes, 0);
	for(int i = 1; i<numNodes; i++)
	{
		if(usedNodes[i] == true)
		{
			printf("vertex (%s) is connected to Dome-0\n", nodes[i].name);
		}
	}
	printf("\n");
	for(int i = 1; i<numNodes; i++)
	{
		if(usedNodes[i] == false)
		{
			printf("we have lost connection with node (%s)\n", nodes[i].name);
		}
	}
	printf("\n");
}
//-------------------END MISSION 1---------------------------//


//--------------------MISSION 2------------------------------//
void mission2Aux(int** adjMatrix, int numNodes, int actualNode, int* usedNodes, bool first, Node* nodes)
{
    usedNodes[actualNode] = true;
	for(int i = 0; i<numNodes; i++)
	{
		if((adjMatrix[actualNode][i] == FUNCTIONAL || adjMatrix[actualNode][i] == DAMAGE) && usedNodes[i] == false)
		{
            printf("(%s) ", nodes[i].name);
            first = false;
			mission2Aux(adjMatrix, numNodes, i, usedNodes, first, nodes);
		}
	}
    if(first == true)
    {
        printf("can't connect to any vertex ... process failed\n");
    }
}

void mission2(int numDomeNodes, int** adjMatrix, int numNodes, Node* nodes)
{
    bool allDomesConnected = true;
    int usedNodes[numNodes];
	for(int i = 0; i<numNodes; i++)
	{
		usedNodes[i] = false;
	}
    mission1Aux(adjMatrix, numNodes, usedNodes, 0);
    for(int i = 1; i<numDomeNodes; i++)
    {
        if(usedNodes[i] == false)
        {
            allDomesConnected = false;
        }
    }
    if(allDomesConnected == true)
    {
        printf("all domes connected to main dome (%s), no rerouting needed.\n\n",  nodes[0].name);
    }
    else
    {
        printf("not all domes are connected to main dome...\ninitializing rerouting of ressources....\n\n");
        for(int i = 0; i<numDomeNodes; i++)
        {
            if(usedNodes[i] == false)
            {
                printf("dome (%s) is not connected to main dome, after rerouting procedure the dome has access to vertex: \n", nodes[i].name);
                mission2Aux(adjMatrix, numNodes, i, usedNodes, true, nodes);
                printf("\n");
            }
        }
        printf("\n");
    }
}
//-------------------END MISSION 2---------------------------//


/**
 * A brief description. A more elaborate class description
 * @param totalNumNodes S-E.
 * @param adjacencyMatrix S-E.
 * @return Prints something about the minimal weight spanning tree / Destructive kruskal
 */
void mission4(int totalNumNodes, int ** adjacencyMatrix, Node* nodes, Edge * edges){

    int ** adjMatrix2 = NULL;
    allocMatrix(totalNumNodes, &adjMatrix2);
    copyMatrix(totalNumNodes, adjacencyMatrix, &adjMatrix2);

    // Remove unconnected Nodes from the adjacency matrix ----------------------

    int * linkedNodes = malloc(totalNumNodes * sizeof(int));
    int numNodes = DFS(adjMatrix2, totalNumNodes, 0, &linkedNodes);

    for (int i = 0; i < totalNumNodes; i++){
        if (linkedNodes[i] == false){
            for (int j = 0; j < totalNumNodes; j++){
                adjMatrix2[i][j] = UNDEF;
                adjMatrix2[j][i] = UNDEF;
            }
        }
    }


    // Using the new adjacency matrix with just the important edges, select them

    int numEdge = 0;

    for (int i = 0; i < totalNumNodes; i++){
        for (int j = 0; j <= i; j++){
            if (adjMatrix2[i][j] != UNDEF){
                numEdge = numEdge + 1;
            }
        }
    }

    Edge remainingEdges[numEdge];
    int index = 0;
    for (int i = 0; i < numEdge; i++){
        while (adjMatrix2[edges[index].start->index][edges[index].end->index] == UNDEF){
            index = index + 1;
        }
        remainingEdges[i] = edges[index];
        index = index + 1;
    }
    
    // Sort from lowest to highest capacity ------------------------------------

    index = 0;
    Edge holder;
    for (int i = 0; i < numEdge - 1; i++) {
        index = i;
        for (int j = i; j < numEdge; j++){
            if (remainingEdges[j].capacity < remainingEdges[index].capacity){
                index = j;
            }
        }
        holder = remainingEdges[i];
        remainingEdges[i] = remainingEdges[index];
        remainingEdges[index] = holder;
    }

    // Now deconstructive kruskal
    
    int holder1 = UNDEF;
    int holder2 = UNDEF;
    index = 0;
    while(numEdge > numNodes - 1){
        holder1 = adjMatrix2[edges[index].start->index][edges[index].end->index];
        holder2 = adjMatrix2[edges[index].end->index][edges[index].start->index];
        adjMatrix2[edges[index].start->index][edges[index].end->index] = UNDEF;
        adjMatrix2[edges[index].end->index][edges[index].start->index] = UNDEF;
        numEdge = numEdge - 1;
        if (numNodes != DFS(adjMatrix2, totalNumNodes, 0, NULL)){
            adjMatrix2[edges[index].start->index][edges[index].end->index] = holder1;
            adjMatrix2[edges[index].end->index][edges[index].start->index] = holder2;
            numEdge = numEdge + 1;
        }
        index = index + 1;
    } 

    printf("Assuming the network at full health, the most cost effective way to protect edges while ensuring full connectivity is to protect the following %d edges :\n", numEdge);

    for (int i = 0; i < totalNumNodes; i++){
        for (int j = 0; j <= i; j++){
            if (adjMatrix2[i][j] != UNDEF){
                printf("    - %s to %s\n", nodes[i].name, nodes[j].name);
            }
        }
    }
    printf("\n");

    freeMatrix(totalNumNodes, &adjMatrix2);
    free(linkedNodes);
}
