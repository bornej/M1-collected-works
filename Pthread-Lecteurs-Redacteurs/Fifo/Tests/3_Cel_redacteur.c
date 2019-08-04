#include "file.h"

int main(){
  printf(" --                 3_Cel_redacteur                 -- \n");
  printf(" -- Creation et destruction d'une cellule Redacteur -- \n");
  Cel* c = (Cel*) malloc(sizeof(Cel));
  init_cel_redacteur(c, pthread_self());
  printf("     ");
  print_cel(c); printf("\n");
  printf("\n");
  detruire_cel(c);
}
