#ifndef __grafofs_h
#define __grafofs_h

#include <stdio.h>
#include <stdlib.h>

#include "listaarchi.h"


#define TRUE  1
#define FALSE 0
#define boolean int


typedef struct _grafo grafo;

struct _grafo
{
  int n; //Numero Nodi
  int m; //Numero collegamenti
  listaarchi *FS;
};


void crea_grafo (int n, grafo *pG);

void distrugge_grafo (grafo *pG);

/* Aggiunge l'arco (orig,dest) al grafo *pG.
   Non verifica che l'arco non esista gia'! */
void ins_arco (nodo orig, nodo dest, float peso, grafo *pG);

/* Cancella l'arco (orig,dest) dal grafo *pG.
   Non verifica che l'arco esista! */
void canc_arco (nodo orig, nodo dest, grafo *pG);

boolean esiste_arco (nodo orig, nodo dest, grafo *pG);

#endif
