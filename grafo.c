#include "grafo-fs.h"

void crea_grafo (int n, grafo *pG)
{
  int i;

  
  pG->n = n;
  pG->m = 0;

  pG->FS = (listaarchi *) calloc(n+1,sizeof(listaarchi));
  if (pG->FS == NULL)
  {
    fprintf(stderr,"Memoria insufficiente per allocare il grafo!\n");
    exit(EXIT_FAILURE);
  }
  for (i = 1; i <= n; i++)
    pG->FS[i] = crealistaarchi();
}


void distrugge_grafo (grafo *pG)
{
  int i;

  for (i = 1; i <= pG->n; i++)
    distruggelistaarchi(&pG->FS[i]);
  free(pG->FS);
  pG->FS = NULL;
  pG->n = 0;
  pG->m = 0;
}


void ins_arco (nodo orig, nodo dest, float peso, grafo *pG)
{
  listaarchi L = pG->FS[orig];
  inslistaarchi(L,succlistaarchi(ultimolistaarchi(L),L),orig,dest,peso);
  pG->m++;
}


void canc_arco (nodo orig, nodo dest, grafo *pG)
{
  listaarchi L;
  posarco p;
  nodo n1, n2;
  boolean trovato;

  L = pG->FS[orig];
  p = primolistaarchi(L);
  trovato = FALSE;
  while (!trovato && !finelistaarchi(L,p))
  {
    leggearco(L,p,&n1,&n2);
    trovato = ( (n1 == orig) && (n2 == dest) );
    if (!trovato) p = succlistaarchi(L,p);
  }

  if (trovato)
  {
    canclistaarchi(L,p);
    pG->m--;
  }
}


boolean esiste_arco (nodo orig, nodo dest, grafo *pG)
{
  listaarchi L;
  posarco p;
  boolean trovato;
  int n1, n2;

  L = pG->FS[orig];
  p = primolistaarchi(L);
  trovato = FALSE;
  while (!trovato && !finelistaarchi(L,p))
  {
    leggearco(L,p,&n1,&n2);
    trovato = ( (n1 == orig) && (n2 == dest) );
    p = succlistaarchi(L,p);
  }

  return trovato;
}
