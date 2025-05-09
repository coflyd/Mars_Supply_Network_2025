#ifndef STRUCTURES_H_
#define STRUCTURES_H_

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define INF 999999

// le rayon de la sphère
#define SPHERE_RADIUS 100.0f


typedef enum {
	DOME, RESEARCH_STATION, SUPPLY_DEPOT
} Infrastructure;

typedef enum {
	FUNCTIONAL, DAMAGE, DESTROYED
} Condition;

typedef struct {
	float x, y, z;
	char *name;
	Infrastructure infrastructure;
	float radius;
	// Ajout de l'index pour identifier les nœuds
	int index;
} Node;

typedef struct {
	Node *start;
	Node *end;
	float weight;
	Condition condition;
	float capacity;
} Edge;

typedef struct {
	float zoom;
	float rotX;
	float rotY;
	int lastX;
	int lastY;
	int mouseDown;
} Display;

#endif
