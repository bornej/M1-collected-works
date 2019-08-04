#include "file.h"

int main(){
  printf(" --                 5_file_supprimer_tete                                   -- \n");
  printf(" -- Creation d'une file, Ajout d'elements, suppression en tête, destruction -- \n");
  File* f = malloc(sizeof(File));
  init_file(f);
  printf("     ");
  printf("Ajout d'un lecteur dans la file.\n");
  ajouter_lecteur_file(f, pthread_self());
  printf("     ");
  printf("Ajout d'un lecteur dans la file.\n");
  ajouter_lecteur_file(f, pthread_self());
  afficher_file(f);
  printf("     ");
  printf("Suppresion de la tête.\n");
  supprimer_tete(f);
  afficher_file(f);
  printf("     ");
  printf("Suppresion de la tête.\n");
  supprimer_tete(f);
  afficher_file(f);
  printf("     ");
  printf("Suppresion de la tête.\n");
  supprimer_tete(f);
  afficher_file(f);
  printf("     ");
  printf("Suppresion de la tête.\n");
  supprimer_tete(f);
  afficher_file(f);
  printf("     ");
  printf("Destruction de la file.\n");
  detruire_file(f);
}
