#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>

#include <math.h>
#include <stdlib.h>
#include "structures.h"

void drawSolidSphereGLU(float radius, int slices, int stacks) {
	GLUquadric *quadric = gluNewQuadric();
	gluQuadricNormals(quadric, GLU_SMOOTH);
	gluSphere(quadric, radius, slices, stacks);
	gluDeleteQuadric(quadric);
}

void drawSphere(float radius, int lats, int longs) {
	glColor3f(0.8f, 0.4f, 0.1f); // Couleur rouge-orangÃ© pour Mars
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
	switch (node->infrastructure) {
		case DOME: glColor3f(1.0f, 0.0f, 0.0f); break;
		case RESEARCH_STATION: glColor3f(0.0f, 0.0f, 1.0f); break;
		case SUPPLY_DEPOT: glColor3f(0.0f, 1.0f, 0.0f); break;
		default: glColor3f(1.0f, 1.0f, 1.0f); break;
	}
	glPushMatrix();
	glTranslatef(node->x, node->y, node->z);
	drawSolidSphereGLU(node->radius, 10, 10);
	glPopMatrix();
}

void drawEdge(Edge *edge, int segments) {
	switch (edge->condition) {
		case FUNCTIONAL: glColor3f(0.0f, 1.0f, 1.0f); break;
		case DAMAGE: glColor3f(1.0f, 1.0f, 0.0f); break;
		case DESTROYED: glColor3f(1.0f, 0.0f, 0.0f); break;
		default: glColor3f(1.0f, 1.0f, 1.0f); break;
	}
	glBegin(GL_LINE_STRIP);
	Node start = *edge->start;
	Node end = *edge->end;
	for (int i = 0; i <= segments; i++) {
		float t = (float)i / segments;
		float x = start.x * (1 - t) + end.x * t;
		float y = start.y * (1 - t) + end.y * t;
		float z = start.z * (1 - t) + end.z * t;
		float length = sqrt(x * x + y * y + z * z);
		x /= length; y /= length; z /= length;
		glVertex3f(x, y, z);
	}
	glEnd();
}

void handleEvents(SDL_Event *event, Display *display) {
	while (SDL_PollEvent(event)) {
		if (event->type == SDL_QUIT) {
			exit(0);
		} else if (event->type == SDL_MOUSEWHEEL) {
			display->zoom *= (event->wheel.y > 0) ? 0.9f : 1.1f;
		} else if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
			display->mouseDown = 1;
			display->lastX = event->button.x;
			display->lastY = event->button.y;
		} else if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT) {
			display->mouseDown = 0;
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
