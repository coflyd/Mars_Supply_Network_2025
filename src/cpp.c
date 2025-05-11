/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>

#include "cpp.h"
#include "structures.h"
#include "models.h"

float geodesicDistance(Node *a, Node *b);

#define MAX_EDGES 512

// Simple adjacency list representation
typedef struct AdjNode {
    int target;
    float weight;
    Edge *edge;
    struct AdjNode *next;
} AdjNode;

typedef struct {
    AdjNode *head;
} AdjList;

static void addEdge(AdjList *graph, int from, int to, float weight, Edge *edge) {
    AdjNode *newNode = (AdjNode *)malloc(sizeof(AdjNode));
    newNode->target = to;
    newNode->weight = weight;
    newNode->edge = edge;
    newNode->next = graph[from].head;
    graph[from].head = newNode;
}

// Count degree (for eulerian check)
static int getDegree(AdjList *graph, int v) {
    int degree = 0;
    AdjNode *cur = graph[v].head;
    while (cur) {
        degree++;
        cur = cur->next;
    }
    return degree;
}

// Dijkstra's algorithm for shortest path between all odd degree nodes
static void dijkstra_(int src, int totalNodes, float *dist, int *prev, AdjList *graph) {
    int visited[totalNodes];
    for (int i = 0; i < totalNodes; i++) {
        dist[i] = FLT_MAX;
        visited[i] = 0;
        prev[i] = -1;
    }
    dist[src] = 0;

    for (int count = 0; count < totalNodes - 1; count++) {
        int u = -1;
        float minDist = FLT_MAX;
        for (int i = 0; i < totalNodes; i++) {
            if (!visited[i] && dist[i] < minDist) {
                minDist = dist[i];
                u = i;
            }
        }
        if (u == -1) break;
        visited[u] = 1;

        AdjNode *cur = graph[u].head;
        while (cur) {
            if (!visited[cur->target] && dist[u] + cur->weight < dist[cur->target]) {
                dist[cur->target] = dist[u] + cur->weight;
                prev[cur->target] = u;
            }
            cur = cur->next;
        }
    }
}

// Hierholzer’s algorithm to construct Eulerian path
static void findEulerianCircuit(int start, AdjList *graph, Edge **path, int *pathLen) {
    int stack[MAX_EDGES], top = -1;
    Edge *output[MAX_EDGES];
    int outIndex = 0;

    stack[++top] = start;

    while (top >= 0) {
        int u = stack[top];
        AdjNode *cur = graph[u].head;

        if (cur == NULL) {
            top--;
            if (top >= 0) output[outIndex++] = NULL; // mark return point
        } else {
            stack[++top] = cur->target;
            graph[u].head = cur->next;
            output[outIndex++] = cur->edge;
            free(cur);
        }
    }

    int e = 0;
    for (int i = 0; i < outIndex; i++) {
        if (output[i] != NULL)
            path[e++] = output[i];
    }
    *pathLen = e;
}

// Entry point for Chinese Postman Problem solver
int buildCPPPath(Node *nodes, int totalNodes, Edge *edges, int numEdges, Edge **cppPath, int *cppLength) {
    AdjList graph[totalNodes];
    for (int i = 0; i < totalNodes; i++) graph[i].head = NULL;

    // Step 1: Build graph with only FUNCTIONAL and DAMAGE edges
    for (int i = 0; i < numEdges; i++) {
        if (edges[i].condition != DESTROYED) {
            int u = edges[i].start->id;
            int v = edges[i].end->id;
            addEdge(graph, u, v, edges[i].weight, &edges[i]);
            addEdge(graph, v, u, edges[i].weight, &edges[i]);
            // addEdge(&graph[u], v, edges[i].weight, &edges[i]);
            // addEdge(&graph[v], u, edges[i].weight, &edges[i]); // undirected
        }
    }

    // Step 2: Find odd-degree nodes
    int oddNodes[totalNodes], oddCount = 0;
    for (int i = 0; i < totalNodes; i++) {
        if (getDegree(graph, i) % 2 == 1)
            oddNodes[oddCount++] = i;
    }

    // Step 3: Pair odd-degree nodes with minimum cost
    // (simplified: greedy pairing, not optimal)
    for (int i = 0; i < oddCount; i += 2) {
        float dist[totalNodes];
        int prev[totalNodes];
        dijkstra_(oddNodes[i], totalNodes, dist, prev, graph);
        int target = oddNodes[i + 1];

        int cur = target;
        while (prev[cur] != -1) {
            Node *a = &nodes[cur];
            Node *b = &nodes[prev[cur]];
            float d = geodesicDistance(a, b);

            Edge *newEdge = (Edge *)malloc(sizeof(Edge));
            newEdge->start = b;
            newEdge->end = a;
            newEdge->weight = d;
            newEdge->condition = FUNCTIONAL;
            newEdge->capacity = 0;
            addEdge(graph, b->id, a->id, d, newEdge);
            addEdge(graph, a->id, b->id, d, newEdge);
            // addEdge(&graph[b->id], a->id, d, newEdge);
            // addEdge(&graph[a->id], b->id, d, newEdge);

            cur = prev[cur];
        }
    }

    // Step 4: Find Eulerian circuit from dome central (node 0)
    findEulerianCircuit(0, graph, cppPath, cppLength);

    return 1;
}

void afficherCheminInspectionAvecSens(Edge **cppPath, int pathLength, Node *start) {
    if (pathLength == 0 || !start) {
        printf("Chemin vide ou point de départ invalide.\n");
        return;
    }

    printf("Chemin d'inspection avec directions (total %d arêtes) :\n", pathLength);
    Node *current = start;
float distance_parcourue = 0;
    for (int i = 0; i < pathLength; i++) {
        Edge *e = cppPath[i];

        if (e->start == current) {
            printf("[%s ➝ %s] ➝ ", e->start->name, e->end->name);
            current = e->end;
        } else if (e->end == current) {
            printf("[%s ➝ %s] ➝ ", e->end->name, e->start->name);
            current = e->start;
        } else {
            // cas incohérent (ex : saut entre chemins appariés)
            printf("[?? ➝ ??] ");
            current = e->end; // fallback
        }
        distance_parcourue += geodesicDistance(e->start, e->end);
    }

    printf("\n");
    printf("Distance totale d'inspection : %.2f km\n",distance_parcourue);
}
*/
