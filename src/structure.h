
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define bool int
#define true 1
#define false 0

// Vertices
typedef enum {
	DOME, RESEARCH_STATION, SUPPLY_DEPOT
} Infrastructure;
// Vertices Structure
typedef struct {
	int index;
	int x;
	int y;
	int vx;
	int vy;
	char label[15];
	bool isDragging;
	Infrastructure infrastructure;
	int radius;
} Node;


//Edges
typedef enum {
	FUNCTIONAL, DAMAGE, DESTROYED
} Condition;

// Edges  Structure
typedef struct {
	Node *from;
	Node *to;
	unsigned int distance;
	Condition condition;
	unsigned int capacity;
} Edge;

