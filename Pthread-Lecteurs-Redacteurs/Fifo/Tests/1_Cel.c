#include "file.h"

int main(){
  printf(" --                 1_Cel                 -- \n");
  printf(" -- Creation et destruction d'une cellule -- \n");
  Cel* c = (Cel*) malloc(sizeof(Cel));
  init_cel(c, 0, pthread_self());
  print_cel(c); printf("\n");
  detruire_cel(c);
}
