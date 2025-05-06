/*
 * structures.h
 *
 *  Created on: 4 mai 2025
 *      Author: yves
 */

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

/*#define bool int
#define true 1
#define false 0*/

#define INF 999999

#define SPHERE_RADIUS 1.0f // Définir le rayon de la sphère


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
	int index; // Ajout de l'index pour identifier les nœuds
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

#endif /* STRUCTURES_H_ */
