/* Inclusione delle librerie */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "listaarchi.c"
#include "grafo.c"
// Per il secondo tempo
#include "intqueue-solution.c"
#include "intstack-solution.c"

#define ROW_LENGTH 256
#define boolean int
#define TRUE  1
#define FALSE 0
typedef int *vint;

/*	struct del dato	*/
typedef struct _elementoP elementoP;

struct _elementoP {
  char s[ROW_LENGTH];
  int NumA;
  char C; // Nome Atomo
  char NomeM[ROW_LENGTH];
  int NumM;
  float x;
  float y;
  float z;
};

// Legge dalla linea di comando il file che contiene il grafo da visitare
void InterpretaLineaComando (int argc, char *argv[], char *filedati);
void CaricaElenco(char *filedati, int n, elementoP *E);
int ** ContaLegamiH(int **links,int *k,elementoP **Molecole, int n, elementoP *E, grafo *pG);
int ContaAtomi(char *filedati);
float Distanza(elementoP p1, elementoP p2);
boolean Angolo(elementoP p1, elementoP p2, elementoP h);
void StampaGrafo (grafo *pG);							//DEL
void ComponentiConnesse (int *C, int **links,int *nc,int NumM);
void Visita(grafo *pG, int i, int *C);
//void ComponentiConnesse (grafo *pG, int *C, int *pnc);
int ricorsiva(posarco albero_minimo,int lati,int i,int memoria);
void DFSricorsiva (int **links, int i, int * C, int nc,int NumM);
void StampaComponentiConnesse (int nc, vint C, int n);	//DEL
void BubbleSort (vint *VC, int nc);
int ** OrdinaStampaCluster(int ** VC,int * C, int nc, int NumM);
void Diametro(grafo *pG,int ** links, int ** VC,int * C,int NumM);
boolean not_in_nodes(int x,int *nodes, int size);
void riordina(int **punti_arco,float *vettore_archi,int size);
elementoP **MolecoleOrdinate(elementoP **Molecole,int *k,elementoP *E,int n);
int cerca_sottinsieme(sottinsieme * sottinsiemi, int i);
void unione_valori(sottinsieme *sottinsiemi, int sorgente, int destinazione);

// Definizione main
int main(int argc, char *argv[])
{
  // Inizializzazione variabili
  char filedati[ROW_LENGTH];
  int i,j;
  int n;
  int **VC;
  grafo G;
  int * C; //vettore che associa a ogni molecola il rispettivo cluster di appartenenza
  int nc; // numero cluster
  int **links;
  elementoP **Molecole;
  int * k;
  // Legge e interpreta linea di comando
  InterpretaLineaComando(argc,argv,filedati);
  // Lettura da file
  n=ContaAtomi(filedati);
  elementoP E[n];	// E sta per Elenco
  // Carica Elenco
  CaricaElenco(filedati, n, E);
  // Rapporto Legami-Atomi
  k=calloc(E[n-1].NumM+1,sizeof(int));
  crea_grafo(E[n-1].NumM,&G);
  StampaGrafo(&G);
 // printf("\nGrafo creato, n: %d",&(G)->n);
  Molecole=MolecoleOrdinate(Molecole,k,E,n); // Lista di adiacenza
  links=ContaLegamiH(links,k,Molecole, n, E, &G); //Matrice di adiacenza
  printf("\n");
  //StampaGrafo(&G);
  // Suddivide in cluster POTRO' FARE UNA FUNZIONE GestisciCluster PER IL SECONDO TEMPO
  //int C[E[n-1].NumM];
  
  /* Determina le componenti connesse */
  C = calloc(G.n+1,sizeof(int));
  if (C == NULL)
  {
    fprintf(stderr,"Errore nell'allocazione di C!\n");
    exit(EXIT_FAILURE);
  }
  ComponentiConnesse(C,links,&nc,G.n);
  //ComponentiConnesse(&G,C,&nc);
  //StampaComponentiConnesse(nc,C,G.n);
  // 5. Ordina i Cluster
  VC=OrdinaStampaCluster(VC,C, nc, G.n);
  Diametro(&G,links,VC,C,G.n);
  // 6. Calcola d con i pesi
  free(C);
  // Dealloca il grafo e il vettore delle componenti connesse */
  distrugge_grafo(&G);
  
  return 0;
}

void Visita(grafo *pG, int i, int *C)
{
  // Scorro il grafo (PrimoLista fino a ultimoLista e per ogni elemento con origine o dest i opero ricorsivamente Visita)
  int orig, dest;
  posarco pa;
  for (pa = primolistaarchi(pG->FS[i]); !finelistaarchi(pG->FS[i],pa); pa = succlistaarchi(pG->FS[i],pa))
  {
	leggearco(pG->FS[i],pa,&orig,&dest);
	if (pG->FS[i]->orig==i){
	  if(C[pG->FS[i]->dest] == 0){
		C[pG->FS[i]->dest]=C[pG->FS[i]->orig];
		Visita(pG, pG->FS[i]->dest, C);
	  }
	}
	else if(pG->FS[i]->dest==i){
	  if(C[pG->FS[i]->orig] == 0){
		C[pG->FS[i]->orig]=C[pG->FS[i]->dest];
		Visita(pG, pG->FS[i]->orig, C);
	  }
	}
  }
}

/*
void ComponentiConnesse (grafo *pG, int *C, int *pnc)
{
  int i; // Indici
  *pnc=0;; // Numero componenti connesse
  for (i=1;i<pG->n;i++){
	if(C[i]==0){
		*pnc++;
		C[i]=*pnc;
		Visita (pG, i, C); // E' una funzione ricorsiva che visita il grafo in ampiezza e ogni molecola, collegata
						   // a partire dalla molecola i, viene contrassegnata con il valore di C[i]
	}
  }
} */

elementoP **MolecoleOrdinate(elementoP **Molecole,int *k,elementoP *E,int n){
  int i,j,s,cont;
  Molecole=malloc((E[n-1].NumM+1)*sizeof(elementoP *));
  Molecole[0]=malloc(sizeof(elementoP));
  for(j=1;j<E[n-1].NumM+1;j++)
  {
    s=0;
    for(i=0; i<n; i++)
    {
      if(E[i].NumM==j)
      {
        k[j]=k[j]+1;
        s=i;
      }
    }
    Molecole[j]=malloc(k[j]*sizeof(elementoP));
    if(Molecole[j]==NULL){
      printf("Non alloca Molec\n");
    }
    cont=0;
    for(i=s-k[j]+1;i<s+1;i++){
        Molecole[j][cont].C=E[i].C;
        Molecole[j][cont].NumM=E[i].NumM;
        Molecole[j][cont].x=E[i].x;
        Molecole[j][cont].y=E[i].y;
        Molecole[j][cont].z=E[i].z;
        cont++;
    }
  }
  return Molecole;
}

// Definizione delle procedure secondarie

void InterpretaLineaComando (int argc, char *argv[], char *filedati)//OKAY
{
  if (argc != 2)
  {
    fprintf(stderr,"Errore nella linea di comando!\n");
    exit(EXIT_FAILURE);
  }
  strcpy(filedati,argv[1]);
}


void CaricaElenco(char *filedati, int n, elementoP *E)//OKAY
{
  //char Riga[ROW_LENGTH];
  int i;
  FILE *fp;
  //Apre il file
  fp = fopen(filedati,"r");
  if (fp == NULL)
  {
    fprintf(stderr,"File %s could not be opened!\n",filedati);
    exit(EXIT_FAILURE);
  }
  //Trascrive
  i=0;
  while (fscanf(fp, "%s %d %c %s %d %f %f %f", &(*(E[i].s)), &(E[i].NumA), &(E[i].C), &(*(E[i].NomeM)), &(E[i].NumM), &(E[i].x), &(E[i].y), &(E[i].z)) != EOF)
  {
	i++;
  }
  //Chiude il file
  fclose(fp);
}

int ** ContaLegamiH(int **links,int *k,elementoP **Molecole ,int n, elementoP *E, grafo *pG)
{
  int i, j,cont,s,h,vlmax,aux,hy,nH;
  float peso;
  int * VL; //Vettore legami per ogni molecola
  VL=calloc(E[n-1].NumM+1,sizeof(int));
  nH=0;
  links=malloc((E[n-1].NumM+1)*sizeof(int*));
  for(i=0;i<E[n-1].NumM+1;i++){
    links[i]=calloc(E[n-1].NumM+1,sizeof(int));
  }
  // 1. Conta nH
  
  for (i=0; i<n; i++)
  {
	  if (E[i].C=='H')
		  nH++;
  }
  // 2.
  // Crea il grafo
  
  
  // 3. Carica Grafo
 
  for(i=1;i<E[n-1].NumM+1;i++){
    for(j=0;j<k[i];j++){
      if(Molecole[i][j].C!='H'){
        for(s=1;s<E[n-1].NumM+1;s++){
          if(s!=i){
          for(cont=0;cont<k[s];cont++){
            if(Molecole[s][cont].C!='H'){
              peso=Distanza(Molecole[i][j],Molecole[s][cont]);
              if(peso<=3.500){
                for(h=0;h<k[i];h++){
                  if(Molecole[i][h].C=='H'){
                    if(Angolo(Molecole[i][j],Molecole[s][cont],Molecole[i][h])){
                      ins_arco(i,s,peso,pG);
                      ins_arco(s,i,peso,pG);
                      links[i][s]=1;
                      links[s][i]=1;
                    }
                  }
                }
              }
            }
          }
        }
        }
      }
    }
  }
  
  /*
  for(i=1;i<E[n-1].NumM+1;i++){
    for(j=0;j<k[i];j++){
      if(Molecole[i][j].C!='H'){
        for(hy=0;hy<k[i];hy++){
          if(Molecole[i][hy].C=='H'){
            for(s=1;s<E[n-1].NumM+1;s++){
              if(s!=i){
              for(cont=0;cont<k[s];cont++){
                if(Molecole[s][cont].C!='H'){
                  peso=Distanza(Molecole[i][j],Molecole[s][cont]);
                  if(peso<=3.500){
                    if(Angolo(Molecole[i][j],Molecole[s][cont],Molecole[i][hy])){
                      printf("\nLegame molecola %d con molecola %d",i,s);
                      ins_arco(i,s,peso,pG);
                      links[i][s]=1;
                      links[s][i]=1;
                    }
                  }
                }
              }
            }
            }
          }
        }     
      }
    }
  }
  */

  // 5. Preparazione dati: VL ,hy, aux
  for(i=1;i<E[n-1].NumM+1;i++){
    for(j=1;j<E[n-1].NumM+1;j++){
      VL[i]=VL[i]+links[i][j];
    }
  }
  
  
  vlmax=0;
  for(i=1;i<E[n-1].NumM+1;i++){
    if(VL[i]>vlmax){
      vlmax=VL[i];
    }
  }
  hy=0;
  for(i=0;i<vlmax+1;i++){
    aux=0;
    for(j=1;j<E[n-1].NumM+1;j++){
      if(VL[j]==i){
        aux=aux+1;
      }
    }
    hy=hy+aux*i;
  }
  hy=hy/2;

  // 6. Stampa dati
  printf("\nLegami:\t%d su %d atomi\n", hy, nH);
  for (i=0;i<vlmax+1;i++){
    aux=0;
    for(j=1;j<E[n-1].NumM+1;j++){
      if(VL[j]==i){
        aux=aux+1;
      }
    }
    printf("Grado %d: %d molecole\n", i, aux);
  }
  return links;
}

int ContaAtomi (char *filedati)
{
  FILE *fp;
  int n;
  char Riga[ROW_LENGTH];
  // Inizializzo
  n=0;
  // Apre il file
  fp = fopen(filedati,"r");
  if (fp == NULL)
  {
    fprintf(stderr,"File %s could not be opened!\n",filedati);
    exit(EXIT_FAILURE);
  }
  do{
	Riga[0]='\0';
    fgets(Riga,ROW_LENGTH,fp);
	n++;
  }while (Riga[0]!='\0');
  // Chiude il file
  fclose(fp);
  return n-1; 
}

float Distanza(elementoP p1, elementoP p2)
{
	float d;
  d=sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y)+(p1.z-p2.z)*(p1.z-p2.z));
	return d;
}

boolean Angolo(elementoP  p1, elementoP  p2, elementoP  h)
{
	float alpha;
  // Calcolo alpha
  float dista,distb, distc;
  dista=Distanza(p1,p2);	distb=Distanza(p1,h);		distc=Distanza(p2,h);
  alpha=(-dista*dista+distb*distb+distc*distc)/(2*distb*distc);
  alpha=acos(alpha);
  alpha=alpha*180/3.141592;
  if (alpha>=150)
  {
  return TRUE;
  }
	return FALSE;
}

void StampaGrafo (grafo *pG)
{
  int i, j;
  posarco p;
  int k;
  for (i = 1; i <= pG->n; i++)
    for (p = primolistaarchi(pG->FS[i]); !finelistaarchi(pG->FS[i],p); p = succlistaarchi(pG->FS[i],p))
  {
    leggearco(pG->FS[i],p,&k,&j);
    printf("(%d,%d) ",i,j);
  }
  printf("\n");
}

void ComponentiConnesse (int *C, int **links,int *nc,int NumM)
{
  int i,j;
  *nc=0;
  for (i=1;i<=NumM;i++){
    if (C[i] == 0)
    {
      *nc=*nc+1;
      DFSricorsiva(links,i,C,*nc,NumM);
    }
  }
}

void DFSricorsiva (int **links, int i, int * C, int nc,int NumM)
{
  int j;
  C[i] = nc;
  for (j=0;j<=NumM;j++)
  {
    if(links[i][j]==1){
      if (C[j]==0)
      DFSricorsiva(links,j,C,nc,NumM);
  }
  }
}


// Stampa le componenti connesse 
void StampaComponentiConnesse (int nc, vint C, int NumM)
{
  int v, c;

  printf("Numero componenti: %d\n",nc);
  printf("C = [ ");
  for (v = 1; v <= NumM; v++)
    printf("%d ",C[v]);
  printf("]\n");

  for (c = 1; c <= nc; c++)
  {
    printf("U[%d] = ",c);
    for (v = 1; v <= NumM; v++)
      if (C[v] == c) printf("%d ",v);
    printf("\n");
  }
}



int ** OrdinaStampaCluster(int ** VC,int * C, int nc, int NumM)
{
  int i, j;
  VC = malloc((nc+1)*sizeof(int *)); 
  for (i=1; i<=nc; i++)
  {
	  VC[i]=calloc(2+1, sizeof(int));  
  }
  // Carica VC
  for (i=1;i<nc+1;i++){
    VC[i][2]=i;  
  }
	
  for (i=1;i<nc+1;i++){
    for (j=1; j<NumM+1; j++){
        if (C[j]==i){
            VC[i][1]++; 
        }
    } 
  }
  
	 
  // Ordina VC
  BubbleSort(VC, nc);
  //for (i=1; i<=nc; i++)
	
  for (i=1;i<nc+1;i++){
      printf("\n Cluster %d :  %d molecole - \t", VC[i][2],VC[i][1]);
      for (j=1; j<=NumM; j++){
	      if (C[j]==VC[i][2]){
		      printf("%d ",j);
        }
      }
	    printf("\n");
  }
  return VC;
}

/* Ordina il vettore V di estremi 1 e n */
void BubbleSort (vint *VC, int nc)
{
  int i,j;
  int buffer;
  for(j=1;j<nc;j++){
    for(i=1;i<nc;i++){
      if(VC[i+1][1]>VC[i][1]){
        buffer=VC[i][1];
        VC[i][1]=VC[i+1][1];
        VC[i+1][1]=buffer;
        buffer=VC[i][2];
        VC[i][2]=VC[i+1][2];
        VC[i+1][2]=buffer;
      }
    }
  }
}

void Diametro(grafo *pG,int ** links, int **VC,vint C,int NumM){
  int j,i,k,cont,cont2,diametro,bivio,check,testa;
  posarco p;
  int **collegamenti;
  int **punti_arco;
  float * vettore_archi;
  int *nodes;
  int legami;
  float cammino,buffer,costoMST,heu;
  posarco albero_minimo;
  albero_minimo=malloc((VC[1][1])*sizeof(arco));
  cammino=0;
  legami=0;
  listaarchi adiacenze;
  i=0;
  k=0;
  bivio=0;
  cont2=0;
  cont=0;
  testa=0;

  // 1. Creo la lista di tutti gli archi possibili del grafo non orientato


  for(j=1;j<=NumM;j++){
    if(C[j]==VC[1][2]){
      for(p=primolistaarchi(pG->FS[j]);!finelistaarchi(pG->FS[j],p);p=succlistaarchi(pG->FS[j],p)){
        if(p->dest!=p->orig){
          if(i==0){
            adiacenze=malloc(sizeof(arco));
            adiacenze->pred=NULL;
            adiacenze->orig=p->orig;
            adiacenze->dest=p->dest;
            adiacenze->peso=p->peso;
            adiacenze->succ=NULL;
            i++;
          } 
            else{
            adiacenze->succ=malloc(sizeof(arco));
            adiacenze->succ->pred=adiacenze;
            adiacenze=adiacenze->succ;
            adiacenze->orig=p->orig;
            adiacenze->dest=p->dest;
            adiacenze->peso=p->peso;
            adiacenze->succ=NULL;
        }
        cont2++;
        }
      }
    }
  }
    // 2. Salvo i punti di origine e destinazione di ogni arco e il relativo peso in due vettori adiacenti
    vettore_archi=malloc(cont2*sizeof(float));
    punti_arco=malloc(cont2*sizeof(int*));
    for(i=0;i<cont2;i++){
      punti_arco[i]=malloc(2*sizeof(int));
    }
    for(i=0;i<cont2;i++){
      vettore_archi[i]=adiacenze->peso;
      punti_arco[i][0]=adiacenze->orig;
      punti_arco[i][1]=adiacenze->dest;
      adiacenze=adiacenze->pred;
    }
    // 3. Riordino i vettori adiacenti degli archi del grafo in ordine crescente rispetto al loro peso

    riordina(punti_arco,vettore_archi,cont2);

    // 4. Cerco l'albero minimo con l'algoritmo di Kruskal

    // 5. Genero per ogni molecola del cluser il sottoinsieme di appartenenza con struttura padre-figlio

    sottinsieme *sottinsiemi;
    sottinsiemi=malloc((NumM+1)*sizeof(sottinsieme));
    for(i=0;i<=NumM;i++){
      sottinsiemi[i].padre=i;
      sottinsiemi[i].figlio=0;
    }
    int lati;
    lati=0;
    i=0;
    j=0;
    int sorgente,destinazione;
    // 6. Costruisco l'albero minimo
    while(lati<VC[1][1]-1){
      sorgente=cerca_sottinsieme(sottinsiemi,punti_arco[i][0]);
      destinazione=cerca_sottinsieme(sottinsiemi,punti_arco[i][1]);
      i++;
      if(sorgente!=destinazione){
        albero_minimo[lati].orig=punti_arco[i][0];
        albero_minimo[lati].dest=punti_arco[i][1];
        albero_minimo[lati].peso=vettore_archi[i];
        lati++;
        unione_valori(sottinsiemi,sorgente,destinazione);
      }
    }

    // 7. Calcolo il Costo
    for(i=0;i<lati;i++){
      costoMST=costoMST+albero_minimo[i].peso;
    }

    // 8. Cerco il diametro massimo ricorsivamente a partire da una molecola che fa parte del cluster piu' grande
    /*
    printf("\nCerco il diametro");
    heu=0;
    diametro=0;
    for(i=0;i<=NumM;i++){
      check=0;
      if(C[i]==VC[1][2]){
        for(j=0;j<lati;j++){
          if(albero_minimo[j].dest==i || albero_minimo[j].orig==i){
            check++;
          }
        }
        if(check==1){
          printf("\nControllo la molecola foglia %d",i);
          heu=ricorsiva(albero_minimo,lati,i,0);
          if(heu>diametro){
            diametro=heu;
          }
        }
      }
    } */
    printf("\nDiametro: %d",diametro);
    printf("\nCosto MST: %f (%d legami)",costoMST,lati);
}
   
 

  void riordina(int **punti_arco,float *vettore_archi,int size){
    int j,s;
    float buffer;
    int coppia[2];
    for(s=0;s<size-1;s++){
      for(j=0;j<size-1;j++){
        if(vettore_archi[j+1]<vettore_archi[j]){
          buffer=vettore_archi[j];
          vettore_archi[j]=vettore_archi[j+1];
          vettore_archi[j+1]=buffer;
          coppia[0]=punti_arco[j][0];
          coppia[1]=punti_arco[j][1];
          punti_arco[j][0]=punti_arco[j+1][0];
          punti_arco[j][1]=punti_arco[j+1][1];
          punti_arco[j+1][0]=coppia[0];
          punti_arco[j+1][1]=coppia[1];
        }
      }
    }
  }

  boolean not_in_nodes(int x,int *nodes, int size){
    int i;
    for(i=0;i<size;i++){
      if(x==nodes[i]){
        return FALSE;
      }
    }
    return TRUE;
  }

  int ricorsiva(posarco albero_minimo,int lati,int i,int memoria){
    int res,j;
    res=0;
    for(j=0;j<lati;j++){
      if(albero_minimo[j].orig==i && albero_minimo[j].dest!=memoria ){
  //      printf("\tsi collega con - %d",albero_minimo[j].dest);
        if(res<ricorsiva(albero_minimo,lati,albero_minimo[j].dest,albero_minimo[j].orig)){
          res=ricorsiva(albero_minimo,lati,albero_minimo[j].dest,albero_minimo[j].orig);
        }
      }
      if(albero_minimo[j].dest==i && albero_minimo[j].orig!=memoria){
   //     printf("\tsi collega con - %d",albero_minimo[j].orig);
        if(res<ricorsiva(albero_minimo,lati,albero_minimo[j].orig,albero_minimo[j].dest)){
          res=ricorsiva(albero_minimo,lati,albero_minimo[j].orig,albero_minimo[j].dest);
        }
      }
    }
    return res+1;
  }

int cerca_sottinsieme(sottinsieme * sottinsiemi, int i) 
{ 
    // Cerco ricorsivamente il sottinsieme in cui si trova la molecola

    if (sottinsiemi[i].padre != i) 
        sottinsiemi[i].padre = cerca_sottinsieme(sottinsiemi, sottinsiemi[i].padre); 
  
    return sottinsiemi[i].padre; 
} 

void unione_valori(sottinsieme *sottinsiemi, int sorgente, int destinazione){

  int radice_sorgente;
  int radice_destinazione;

  radice_sorgente=cerca_sottinsieme(sottinsiemi,sorgente);
  radice_destinazione=cerca_sottinsieme(sottinsiemi,destinazione);

  if(sottinsiemi[radice_sorgente].figlio<sottinsiemi[radice_destinazione].figlio){
    sottinsiemi[radice_sorgente].padre=radice_destinazione;
  }
  if(sottinsiemi[radice_sorgente].figlio>sottinsiemi[radice_destinazione].figlio){
    sottinsiemi[radice_destinazione].padre=radice_sorgente;
  }
  if(sottinsiemi[radice_sorgente].figlio==sottinsiemi[radice_destinazione].figlio){
    sottinsiemi[radice_destinazione].padre=radice_sorgente;
    sottinsiemi[radice_sorgente].figlio=sottinsiemi[radice_sorgente].figlio+1;
  
}
}