/*
 * display.c
 *
 *  Created on: 5 mai 2025
 *      Author: yves
 */
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
/*
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>


#include <GL/glut.h> // Pour glutSolidSphere
#include <GL/glu.h>  // Pour gluPerspective
*/

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif


#include "structures.h"



void drawSphere(float radius, int lats, int longs) {
	glColor3f(0.8f, 0.4f, 0.1f); // Couleur rouge-orangé pour Mars
	for (int i = 0; i < lats; i++) {
		float lat0 = M_PI * (-0.5 + (float) (i - 1) / lats);
		float z0 = sin(lat0);
		float zr0 = cos(lat0);

		float lat1 = M_PI * (-0.5 + (float) i / lats);
		float z1 = sin(lat1);
		float zr1 = cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j <= longs; j++) {
			float lng = 2 * M_PI * (float) (j - 1) / longs;
			float x = cos(lng);
			float y = sin(lng);

			glVertex3f(x * zr0 * radius, y * zr0 * radius, z0 * radius);
			glVertex3f(x * zr1 * radius, y * zr1 * radius, z1 * radius);
		}
		glEnd();
	}
}

void drawNode(Node *node) {
	// Définir la couleur en fonction du type d'infrastructure
	switch (node->infrastructure) {
	case DOME:
		glColor3f(1.0f, 0.0f, 0.0f); // Rouge pour les dômes
		break;
	case RESEARCH_STATION:
		glColor3f(0.0f, 0.0f, 1.0f); // Bleu pour les stations de recherche
		break;
	case SUPPLY_DEPOT:
		glColor3f(0.0f, 1.0f, 0.0f); // Vert pour les dépôts
		break;
	}

	glPushMatrix();
	glTranslatef(node->x, node->y, node->z);
	glutSolidSphere(node->radius, 10, 10); // Dessiner une sphère avec le rayon spécifié
	glPopMatrix();
}
/*
void drawNode(Node *node) {
	switch (node->infrastructure) {
		case DOME:
			glColor3f(1.0f, 0.0f, 0.0f); // Rouge pour les dômes
			break;
		case RESEARCH_STATION:
			glColor3f(0.0f, 0.0f, 1.0f); // Bleu pour les stations de recherche
			break;
		case SUPPLY_DEPOT:
			glColor3f(0.0f, 1.0f, 0.0f); // Vert pour les dépôts
			break;
		}

	glPushMatrix();
	glTranslatef(node->x, node->y, node->z);
	glColor3f(0.0f, 1.0f, 0.0f); // exemple de couleur
	drawSphere(0.1f, 12, 12); // remplace glutSolidSphere
	glPopMatrix();
}*/

void drawEdge(Edge *edge, int segments) {
	// Définir la couleur en fonction de l'état de la route
	switch (edge->condition) {
	case FUNCTIONAL:
		glColor3f(0.0f, 1.0f, 1.0f); // Vert pour les routes fonctionnelles
		break;
	case DAMAGE:
		glColor3f(1.0f, 1.0f, 0.0f); // Jaune pour les routes endommagées
		break;
	case DESTROYED:
		glColor3f(1.0f, 0.0f, 0.0f); // Rouge pour les routes détruites
		break;
	default:
		glColor3f(1.0f, 1.0f, 1.0f); // Blanc par défaut (au cas où)
		break;
	}

	glBegin(GL_LINE_STRIP);

	Node start = *edge->start;
	Node end = *edge->end;

	for (int i = 0; i <= segments; i++) {
		float t = (float) i / segments;
		float x = start.x * (1 - t) + end.x * t;
		float y = start.y * (1 - t) + end.y * t;
		float z = start.z * (1 - t) + end.z * t;

		// Normaliser le point pour qu'il soit sur la surface de la sphère
		float length = sqrt(x * x + y * y + z * z);
		x /= length;
		y /= length;
		z /= length;

		glVertex3f(x, y, z);
	}

	glEnd();
}

void handleEvents(SDL_Event *event, Display *display) {
	while (SDL_PollEvent(event)) {
		if (event->type == SDL_QUIT) {
			exit(0); // Quitter l'application
		} else if (event->type == SDL_MOUSEWHEEL) {
			if (event->wheel.y > 0) {
				display->zoom *= 0.9f;
			} else if (event->wheel.y < 0) {
				display->zoom *= 1.1f;
			}
		} else if (event->type == SDL_MOUSEBUTTONDOWN) {
			if (event->button.button == SDL_BUTTON_LEFT) {
				display->mouseDown = 1;
				display->lastX = event->button.x;
				display->lastY = event->button.y;
			}
		} else if (event->type == SDL_MOUSEBUTTONUP) {
			if (event->button.button == SDL_BUTTON_LEFT) {
				display->mouseDown = 0;
			}
		} else if (event->type == SDL_MOUSEMOTION && display->mouseDown) {
			int x, y;
			SDL_GetMouseState(&x, &y);
			display->rotX += (y - display->lastY) * 0.5f;
			display->rotY += (x - display->lastX) * 0.5f;
			display->lastX = x;
			display->lastY = y;
		}
	}
}

void connectNodes(Node nodes[], int numDomes, Edge edges[], int *numEdges) {
	int numResearchStations = (3 * numDomes) / 2;
	int numSupplyDepots = (3 * numDomes) / 2;

	// Relier chaque dôme aux dômes les plus proches sans créer de routes qui se coupent
	for (int i = 1; i < numDomes; i++) {
		float minDist = INFINITY;
		int closestDomeIndex = -1;
		for (int j = 0; j < numDomes; j++) {
			if (i == j)
				continue; // Ne pas relier un dôme à lui-même
			float dx = nodes[i].x - nodes[j].x;
			float dy = nodes[i].y - nodes[j].y;
			float dz = nodes[i].z - nodes[j].z;
			float dist = sqrt(dx * dx + dy * dy + dz * dz);
			if (dist < minDist) {
				minDist = dist;
				closestDomeIndex = j;
			}
		}
		if (closestDomeIndex != -1) {
			edges[*numEdges] = (Edge ) { &nodes[i], &nodes[closestDomeIndex],
							1.0f, FUNCTIONAL, 1.0f };
			(*numEdges)++;
		}
	}

	// Relier les stations de recherche et les dépôts aux dômes les plus proches
	for (int i = numDomes; i < numDomes + numResearchStations + numSupplyDepots;
			i++) {
		float minDist = INFINITY;
		int closestDomeIndex = -1;
		for (int j = 0; j < numDomes; j++) {
			float dx = nodes[i].x - nodes[j].x;
			float dy = nodes[i].y - nodes[j].y;
			float dz = nodes[i].z - nodes[j].z;
			float dist = sqrt(dx * dx + dy * dy + dz * dz);
			if (dist < minDist) {
				minDist = dist;
				closestDomeIndex = j;
			}
		}
		if (closestDomeIndex != -1) {
			edges[*numEdges] = (Edge ) { &nodes[i], &nodes[closestDomeIndex],
							1.0f, FUNCTIONAL, 1.0f };
			(*numEdges)++;
		}
	}

	// Vérifier la connexité du graphe uniquement entre les dômes
	bool *visited = (bool*) malloc(numDomes * sizeof(bool));
	for (int i = 0; i < numDomes; i++) {
		visited[i] = false;
	}

	// Utiliser un parcours en largeur (BFS) pour vérifier la connexité
	int queue[numDomes];
	int front = 0, rear = 0;
	queue[rear++] = 0; // Commencer par le dôme principal
	visited[0] = true;

	while (front < rear) {
		int current = queue[front++];
		for (int i = 0; i < *numEdges; i++) {
			if ((edges[i].start == &nodes[current]
					|| edges[i].end == &nodes[current])
					&& edges[i].start->infrastructure == DOME
					&& edges[i].end->infrastructure == DOME) {
				Node *neighbor =
						(edges[i].start == &nodes[current]) ?
								edges[i].end : edges[i].start;
				int neighborIndex = neighbor - nodes;
				if (!visited[neighborIndex]) {
					visited[neighborIndex] = true;
					queue[rear++] = neighborIndex;
				}
			}
		}
	}

	// Si des dômes ne sont pas visités, les relier au dôme principal
	for (int i = 0; i < numDomes; i++) {
		if (!visited[i]) {
			edges[*numEdges] = (Edge ) { &nodes[i], &nodes[0], 1.0f, FUNCTIONAL,
							1.0f };
			(*numEdges)++;
		}
	}

	free(visited);

	// Attribuer des états aux routes selon la répartition souhaitée
	int totalEdges = *numEdges;
	int functionalCount = (int) (0.5 * totalEdges);
	int damageCount = (int) (0.3 * totalEdges);
	// int destroyedCount = totalEdges - functionalCount - damageCount;

	for (int i = 0; i < totalEdges; i++) {
		if (i < functionalCount) {
			edges[i].condition = FUNCTIONAL;
		} else if (i < functionalCount + damageCount) {
			edges[i].condition = DAMAGE;
		} else {
			edges[i].condition = DESTROYED;
		}
	}
}
