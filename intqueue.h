#ifndef __intqueue_h
#define __intqueue_h

#include <stdio.h>
#include <stdlib.h>


#define TRUE  1
#define FALSE 0
#define boolean int


typedef struct _intqueue intqueue;
struct _intqueue
{
  int *V;
  int size; /* dimensione massima della coda */
  int head; /* indice dell'ultima posizione occupata */
  int tail; /* indice della prima posizione libera */
};


void CreateQueue (intqueue *pQ, int size);

void DestroyQueue (intqueue *pQ);

boolean IsEmptyQueue (intqueue *pQ);

int Front (intqueue *pQ);

void Enqueue (int i, intqueue *pQ);

void Dequeue (intqueue *pQ);

#endif
