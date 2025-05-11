#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int capacity;
    int * data;
    int top;
} intStack;

intStack CreateStack(int capacity)
{
    intStack p;
    p.capacity = capacity;
    p.top = -1;
    p.data = malloc(capacity * sizeof(int));
    return p;
}

void freeStack(intStack *p){
    free(p->data);
}

int isEmptyStack(intStack p)
{
    return (p.top == -1);
}

int isFullStack(intStack p)
{
    return (p.top == p.capacity - 1);
}

int Push(intStack *p, int e)
{
    if (isFullStack(*p)) return 0;
    p->top = p->top + 1;
    p->data[p->top] = e;
    return 1;
}

int Pop(intStack *p)
{
    if (isEmptyStack(*p)) return 0;
    p->top = p->top - 1;
    return 1;
}

int Top(intStack p, int *e)
{
    if (isEmptyStack(p)) return 0;
    *e = p.data[p.top];
    return 1;
}

void print_stack(intStack q)
{
    int e;
    while(Top(q,&e))
    {
        printf("%d ",e);
        Pop(&q);
    }
}