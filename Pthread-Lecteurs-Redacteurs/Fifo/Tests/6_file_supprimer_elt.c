#include "file.h"

int main(){
  printf(" --                 6_file_supprimer_element                 -- \n");
  printf(" -- Creation d'une file, Ajout d'elements, suppression d'elements dans la file, destruction -- \n");
  File* f1 = malloc(sizeof(File));
  init_file(f1);
  printf("     ");
  printf("Ajout d'un lecteur dans la file d' identifiant = 0.\n");
  ajouter_lecteur_file(f1, 0);
  afficher_file(f1);
  printf("     ");
  printf("Suppression de l'élement de la file ayant l' identifiant = 0.\n");
  supprimer_element(f1, 0);
  afficher_file(f1);
  printf("     ");
  printf("Ajout d'un lecteur dans la file d' identifiant = 0.\n");
  ajouter_lecteur_file(f1, 0);
  printf("     ");
  printf("Ajout d'un lecteur dans la file d' identifiant = 1.\n");
  ajouter_lecteur_file(f1, 1);
  printf("     ");
  printf("Ajout d'un lecteur dans la file d' identifiant = 2.\n");
  ajouter_lecteur_file(f1, 2);
  afficher_file(f1);

  /* Supprimer au milieu */
  printf("     ");
  printf("Suppression de l'élement de la file ayant l' identifiant = 1.\n");
  supprimer_element(f1, 1);
  afficher_file(f1);

  /* Supprimer en tête */
  printf("     ");
  printf("Suppression de l'élement de la file ayant l' identifiant = 0.\n");
  supprimer_element(f1, 0);
  afficher_file(f1);

  /* Supprimer en queue/tête */
  printf("     ");
  printf("Suppression de l'élement de la file ayant l' identifiant = 2.\n");
  supprimer_element(f1, 2);
  afficher_file(f1);
  printf("     ");
  printf("Destruction de la file.\n");
  detruire_file(f1);
  return 0;
}
