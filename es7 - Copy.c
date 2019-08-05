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
void ContaLegamiH(char *filedati, int n, elementoP *E, grafo *pG);
int ContaAtomi(char *filedati);
float Distanza(elementoP p1, elementoP p2);
boolean Angolo(elementoP p1, elementoP p2, elementoP h);
void StampaGrafo (grafo *pG);							//DEL
void ComponentiConnesse (grafo *pG, vint C, int *pnc);
void StampaComponentiConnesse (int nc, vint C, int n);	//DEL
void BFS (grafo *pG, int s, vint C, int nc);
void DFS (grafo *pG, int s, vint C, int nc);
void DFSricorsiva (grafo *pG, int s, vint C, int c);
void InsertionSort (vint *V, int n);
void InserisceOrdinato (int x1, int x2, vint *V, int n);
int ** OrdinaStampaCluster(int ** VC,int * C, int nc, int NumM);
void Diametro(grafo *pG,int ** VC,int * C,int NumM);
boolean not_in_nodes(int x,int *nodes, int size);
void riordina(int **punti_arco,float *vettore_archi,int size);

// Definizione main
int main(int argc, char *argv[])
{
  // Inizializzazione variabili
  char filedati[ROW_LENGTH];
  int n;
  int **VC;
  grafo *G;
  int * C; //vettore che associa a ogni molecola il rispettivo cluster di appartenenza
  int nc; // numero cluster
  // Legge e interpreta linea di comando
  InterpretaLineaComando(argc,argv,filedati);
  // Lettura da file
  n=ContaAtomi(filedati);
  elementoP E[n];	// E sta per Elenco
  // Carica Elenco
  CaricaElenco(filedati, n, E);
  // Rapporto Legami-Atomi
  ContaLegamiH(filedati, n, E, G);
  StampaGrafo(G);
  // Suddivide in cluster POTRO' FARE UNA FUNZIONE GestisciCluster PER IL SECONDO TEMPO
  //int C[E[n-1].NumM];
  
  /* Determina le componenti connesse */
  C = calloc(G->n+1,sizeof(int));
  if (C == NULL)
  {
    fprintf(stderr,"Errore nell'allocazione di C!\n");
    exit(EXIT_FAILURE);
  }
  ComponentiConnesse(G,C,&nc);
  // 5. Ordina i Cluster
  printf("\nPrima di Ordinastampacluster");
  VC=OrdinaStampaCluster(VC,C, nc, E[n-1].NumM);
  printf("%d ",VC[1][1]);
  printf("\nPrima di diametro");
  Diametro(G,VC,C,E[n-1].NumM);
  // 6. Calcola d con i pesi
  free(C);
  // Dealloca il grafo e il vettore delle componenti connesse */
  distrugge_grafo(G);
  
  return 0;
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

void ContaLegamiH(char *filedati, int n, elementoP *E, grafo *pG)
{
  int i, j,cont,s,h,vlmax,aux,hy,nH;
  int *collegamenti;
  int **links;
  float peso;
  nH=0;
  links=malloc((E[n-1].NumM+1)*sizeof(int));
  for(i=0;i<E[n-1].NumM+1;i++){
    links[i]=calloc(E[n-1].NumM+1,sizeof(int));
  }
  int * VL; //Vettore legami per ogni molecola
  VL=calloc(E[n-1].NumM+1,sizeof(int));
  int * k;
  k=calloc(E[n-1].NumM+1,sizeof(int));
  elementoP ** Molecole;
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
  // 1.
  
  for (i=0; i<n; i++)
  {
	  if (E[i].C=='H')
		  nH++;
  }
  // 2.
  // Crea il grafo
  crea_grafo(E[n-1].NumM,pG);

  
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


  for(i=1;i<E[n-1].NumM+1;i++){
    for(j=1;j<E[n-1].NumM+1;j++){
     // VL[i]=VL[i]+collegamenti[(i-1)*E[n-1].NumM+j-1];
      VL[i]=VL[i]+links[i][j];
    }
  }
  
  // 5. Stampa dati
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

void ComponentiConnesse (grafo *pG, vint C, int *pnc)
{

  int s;
  *pnc = 0;

  for (s = 1; s < pG->n+1; s++)
    if (C[s] == 0)
    {
      (*pnc)++;
      DFSricorsiva(pG,s,C,*pnc);
    }
}

void DFSricorsiva (grafo *pG, int s, vint C, int pnc)
{
  int orig, dest;
  posarco pa;

  C[s] = pnc;
  for (pa = primolistaarchi(pG->FS[s]); !finelistaarchi(pG->FS[s],pa);pa = succlistaarchi(pG->FS[s],pa)){
    leggearco(pG->FS[s],pa,&orig,&dest);
    if(C[dest] == 0){
      DFSricorsiva(pG,dest,C,pnc);
    }
  }
}


// Stampa le componenti connesse 
void StampaComponentiConnesse (int nc, vint C, int n)
{
  int v, c;

  printf("Numero componenti: %d\n",nc);
  printf("C = [ ");
  for (v = 1; v <= n; v++)
    printf("%d ",C[v]);
  printf("]\n");

  for (c = 1; c <= nc; c++)
  {
    printf("U[%d] = ",c);
    for (v = 1; v <= n; v++)
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
  for (i=1;i<nc+1;i++){
      printf("\n Cluster %d :  %d molecole  \t", VC[i][2],VC[i][1]);
  }
	 
  // Ordina VC
  InsertionSort(VC, nc);
  printf("\n%d\t", VC[1][1]);
  //for (i=1; i<=nc; i++)
	//printf("VC[1][%d] vale: %d\tVC[2][%d] vale: %d\n", i, VC[1][i], i, VC[2][i]);
  /*// Cerco una molecola del cluster più grande
  for (i=0; i<nc; i++)
  {
	printf("Cluster %d: %d molecole - ", i+1, VC[i]);
	for (j=1; j<=NumM; j++)
	  if (C[j]==i+1)
		printf("%d ", j);
  }
  int Vaux[nc];
  for (i=1; i<=nc; i++)
	if (C[i]==)*/
  /*for (i=0; i<nc; i++)
	printf("VC[%d] vale: %d\n", i, VC[i]);*/
  // Stampa dei Cluster
  /*for (i=0; i<nc; i++)
  {
	printf("Cluster %d: %d molecole - ", i+1, VC[i]);
	for (j=1; j<=NumM; j++)
	  if (C[j]==i+1)
		printf("%d ", j);
	printf("\n");
  }*/
  return VC;
}

/* Ordina il vettore V di estremi 1 e n */
void InsertionSort (vint *V, int n)
{
  int j, x1, x2;
  for (j = 2; j <= n; j++)
  {
	x1 = V[j][1];
	x2 = V[j][2];
	/* Inserisce x nel sottovettore V[1,j-1] in maniera ordinata */
	InserisceOrdinato(x1,x2,V,j-1);
  }
}

void InserisceOrdinato (int x1, int x2, vint *V, int n)
{
  int i;
  for (i = 1; (i <= n) && (V[i][1] < x1); i++)
  {
    V[i+1][1] = V[i][1];
    V[i+1][2] = V[i][2];
  }
  V[i+1][1] = x1;
  V[i+1][2] = x2;
}

void Diametro(grafo *pG, int **VC,vint C,int NumM){
  printf("\nFino a qua va");
  int j,i,k,cont,cont2;
  posarco p;
  int **punti_arco;
  int *nodes;
  int legami;
  float cammino;
  cammino=0;
  legami=0;
  printf("\nFino a qua va");
  nodes=malloc(VC[1][1]*sizeof(int));
  listaarchi adiacenze;
  i=0;
  k=0;
  cont=0;
  printf("\nFino a qua va");
  for(j=1;j<=NumM;j++){
    if(C[j]==VC[1][2]){
      for(p=primolistaarchi(pG->FS[j]);!finelistaarchi(pG->FS[j],p);p=succlistaarchi(pG->FS[j],p)){
        printf("\nControllo molecola: %d :: %d",j,p->orig);
        printf("\n%d",p->orig);
        nodes[k]=p->orig;
        if(not_in_nodes(p->dest,nodes,VC[1][1])){  //Il controllo if sulla funzione not_in_nodes serve per escludere i collegamenti doppi
          cont++;
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
          printf("\nAggiunto arco da %d a %d con peso %f",p->orig,p->dest,p->peso);
          }
        }
        k++;
        
        printf("\nNodi:");
        for(cont2=0;cont2<k;cont2++){
          printf(" %d ",nodes[cont2]);
        }
      }
    }
    float * vettore_archi;
    vettore_archi=malloc(cont*sizeof(float));
    punti_arco=malloc(cont*sizeof(int*));
    for(i=0;i<cont;i++){
      punti_arco[i]=malloc(2*sizeof(int));
    }
    for(i=0;adiacenze!=NULL;i++){
      vettore_archi[i]=adiacenze->peso;
      punti_arco[i][0]=adiacenze->orig;
      punti_arco[i][1]=adiacenze->dest;
      adiacenze=adiacenze->pred;
      printf("\n %d %d %f",punti_arco[i][0],punti_arco[i][1],vettore_archi[i]);
    }
    riordina(punti_arco,vettore_archi,i);
    for(i=0;i<cont;i++){
      printf("\nArco da %d a %d : %f",punti_arco[i][0],punti_arco[i][1],vettore_archi[i]);
    }
    printf("\ncheck");
    //Algoritmo costruzione albero minimo
    
    /*
    for(i=0;i<VC[1][1];i++){
      printf("\nCiclo numero %d",iter);
      iter++;
      if(not_visited(punti_arco[i][0],nodes,VC[1][1]) || not_visited(punti_arco[i][1],nodes,VC[1][1])){
        printf("\nAumento il cammino");
        cammino=cammino+vettore_archi[i];
        legami++;
        
      }
      
    } */
    printf("\nDiametro: %d",legami);
    printf("\nCosto MST: %f (%d legami)",cammino,legami);
    
  }

  boolean not_visited(int x,int *nodes,int size){
    int i;
    for(i=0;i<size;i++){
      if(x==nodes[i]){
        printf("\nNot visited");
        nodes[i]=-1;
        printf("\nValore cambiato %d",x);
        return TRUE;
      }
    }
    return FALSE;
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
