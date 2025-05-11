#ifndef STACK_H
#define STACK_H

typedef struct {
    int capacity;
    int * data;
    int top;
} intStack;

intStack CreateStack(int capacity);
void freeStack(intStack *p);

int isEmptyStack(intStack p);
int isFullStack(intStack p);

int Push(intStack *p, int e);

int Pop(intStack *p);
int Top(intStack p, int *e);

void print_stack(intStack q);

#endif