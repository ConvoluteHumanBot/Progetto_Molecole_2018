Visita(grafo *pG, int i, int *C)
{
  // Scorro il grafo (PrimoLista fino a ultimoLista e per ogni elemento con origine o dest i opero ricorsivamente Visita)
  int orig, dest;
  posarco pa;
  for (pa = primolistaarchi(pG->FS[s]); !finelistaarchi(pG->FS[s],pa); pa = succlistaarchi(pG->FS[s],pa))
  {
	leggearco(pG->FS[s],pa,&orig,&dest);
	if ((pG->FS).orig==i){
	  if(C[(pG->FS).dest] == 0){
		C[(pG->FS).dest]=C[(pG->FS).orig];
		Visita(pG, (pG->FS).dest, C);
	  }
	}
	else if((pG->FS).dest==i){
	  if(C[(pG->FS).orig] == 0){
		C[(pG->FS).orig]=C[(pG->FS).dest];
		Visita(pG, (pG->FS).orig, C);
	  }
	}
  }
}

void ComponentiConnesse (grafo *pG, int *C, int *pnc)
{
  int i; // Indici
  *pnc=0;; // Numero componenti connesse
  for (i=1;i<NumM+1;i++){
	if(C[i]==0){
		*pnc++;
		C[i]=*pnc;
		Visita (pG, i, C); // E' una funzione ricorsiva che visita il grafo in ampiezza e ogni molecola, collegata
						   // a partire dalla molecola i, viene contrassegnata con il valore di C[i]
	}
  }
}