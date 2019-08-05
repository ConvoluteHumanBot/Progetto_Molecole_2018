#include "intqueue.h"


void CreateQueue (intqueue *pQ, int size)
{
  pQ->V = (int *) calloc(size,sizeof(int));
  if (pQ->V == NULL)
  {
    fprintf(stderr,"Errore nell'allocazione della coda!\n");
    exit(EXIT_FAILURE);
  }
  pQ->size = size;
  pQ->head = pQ->tail = 0;
}


void DestroyQueue (intqueue *pQ)
{
  free(pQ->V);
  pQ->V = NULL;
  pQ->size = pQ->head = pQ->tail = 0;
}


boolean IsEmptyQueue (intqueue *pQ)
{
  return (pQ->head == pQ->tail);
}


int Front (intqueue *pQ)
{
  return pQ->V[pQ->head];
}


void Enqueue (int i, intqueue *pQ)
{
  pQ->V[pQ->tail] = i;
  pQ->tail++;
}


void Dequeue (intqueue *pQ)
{
  pQ->head++;
}
