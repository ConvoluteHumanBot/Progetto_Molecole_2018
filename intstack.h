#ifndef __intstack_h
#define __intstack_h

#include <stdio.h>
#include <stdlib.h>


#define TRUE  1
#define FALSE 0
#define boolean int


typedef struct _intstack intstack;
struct _intstack
{
  int *V;
  int size; /* dimensione massima della coda */
  int top;  /* indice dell'ultima posizione occupata */
};


void CreateStack (intstack *pS, int size);

void DestroyStack (intstack *pS);

boolean IsEmptyStack (intstack *pS);

int Top (intstack *pS);

void Push (int i, intstack *pS);

void Pop (intstack *pS);

#endif
