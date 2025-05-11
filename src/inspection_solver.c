#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "structures.h"

float geodesicDistance(Node *a, Node *b);

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"


// Marque les arêtes visitées
typedef struct {
    Edge *edge;
    int visited;
} EdgeVisit;

static void dfsReachable(Node *current, Edge *edges, int numEdges, int *reachableNodes, int *reachableEdges) {
    if (reachableNodes[current->id]) return;
    reachableNodes[current->id] = 1;

    for (int i = 0; i < numEdges; i++) {
        if (edges[i].condition == DESTROYED) continue;

        Node *a = edges[i].start;
        Node *b = edges[i].end;
        if ((a == current || b == current) && !reachableEdges[i]) {
            reachableEdges[i] = 1;

            Node *next = (a == current) ? b : a;
            dfsReachable(next, edges, numEdges, reachableNodes, reachableEdges);
        }
    }
}

// DFS pour visiter toutes les arêtes non visitées (non DESTROYED) atteignables
static void inspectDFS(Node *current, EdgeVisit *visitedEdges, int numEdges, Edge **path, Node **nodesPath, int *pathLen, Node *prev) {
    for (int i = 0; i < numEdges; i++) {
        if (visitedEdges[i].visited) continue;

        Edge *e = visitedEdges[i].edge;
        if (e->condition == DESTROYED) continue;

        if (e->start == current || e->end == current) {
            visitedEdges[i].visited = 1;

            Node *next = (e->start == current) ? e->end : e->start;

            path[*pathLen] = e;
            nodesPath[*pathLen + 1] = next;  // car le premier est déjà mis à start
            (*pathLen)++;

            inspectDFS(next, visitedEdges, numEdges, path, nodesPath, pathLen, current);
        }
    }
}

// Fonction principale pour créer le chemin d'inspection
int buildInspectionPath(Node *start, Edge *edges, int numEdges,Edge **path, Node **nodesPath, int *pathLength,Edge **inaccessibleEdges, int *inaccessibleCount) {
    if (!start || !edges || numEdges == 0) return 0;

    // Initialisation
    EdgeVisit visitedEdges[MAX_EDGES];
    for (int i = 0; i < numEdges; i++) {
        visitedEdges[i].edge = &edges[i];
        visitedEdges[i].visited = 0;
    }

    *pathLength = 0;
    nodesPath[0] = start;

    inspectDFS(start, visitedEdges, numEdges, path, nodesPath, pathLength, NULL);

    // Détection des arêtes non atteignables
    int reachableEdges[MAX_EDGES] = {0};
    int reachableNodes[MAX_NODES] = {0};
    dfsReachable(start, edges, numEdges, reachableNodes, reachableEdges);

    *inaccessibleCount = 0;
    for (int i = 0; i < numEdges; i++) {
        if (edges[i].condition == DESTROYED) continue;
        if (!reachableEdges[i]) {
            inaccessibleEdges[(*inaccessibleCount)++] = &edges[i];
        }
    }

    return 1;
}

// Affichage du chemin (avec directions correctes)
void afficherCheminInspectionAvecSens(Node **nodesPath, Edge **path, int pathLen) {
    printf("Chemin d'inspection (total %d arêtes) :\n", pathLen);
    float total = 0;
int i;
    for (i = 0; i < pathLen; i++) {
        Node *from = nodesPath[i];
        Node *to = nodesPath[i + 1];
        printf("[%s ➝ %s] ➝ \n", from->name, to->name);
        total += geodesicDistance(from, to);
    }
    printf("[%s ➝ %s] \n", nodesPath[i]->name, nodesPath[0]->name);
    total += geodesicDistance(nodesPath[i], nodesPath[0]);
    printf("\nDistance totale parcourue : %.2f km\n\n", total);
}

// Affichage des routes inaccessibles
void afficherArcsInaccessibles(Edge **inaccessibles, int count) {
    if (count == 0) {
        printf("Toutes les routes non détruites sont accessibles depuis le nœud central.\n");
        return;
    }

    printf("Routes non atteignables (%d) :\n", count);
    for (int i = 0; i < count; i++) {
        Edge *e = inaccessibles[i];
        float distance = geodesicDistance(e->start, e->end);
        printf("   %s <=> %s  (%.2f km)\n", e->start->name, e->end->name, distance);
    }
}
