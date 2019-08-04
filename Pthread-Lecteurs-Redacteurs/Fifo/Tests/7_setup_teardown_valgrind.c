#include "file.h"
int main(){
  File* f = malloc(sizeof(File));
  init_file(f);
  ajouter_lecteur_file(f, 0);
  ajouter_lecteur_file(f, 1);
  ajouter_lecteur_file(f, 2);
  ajouter_lecteur_file(f, 3);
  supprimer_element(f,0);
  supprimer_element(f,1);
  supprimer_element(f,2);
  supprimer_element(f,3);
  detruire_file(f);
  return 0;
}
