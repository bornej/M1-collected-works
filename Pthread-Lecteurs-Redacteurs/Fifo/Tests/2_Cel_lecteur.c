#include "file.h"

int main(){
  printf(" --                 2_Cel_lecteur                 -- \n");
  printf(" -- Creation et destruction d'une cellule Lecteur -- \n");
  Cel* c = (Cel*) malloc(sizeof(Cel));
  init_cel_lecteur(c, pthread_self());
  print_cel(c); printf("\n");
  detruire_cel(c);
}
