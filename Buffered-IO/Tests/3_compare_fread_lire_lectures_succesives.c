#include "stdes.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  printf("*----------------------------------------* \n");
  printf("*- 3_compare_fread_lectures_successives -*\n");
  printf("*----------------------------------------* \n");
  char* fichier = "./Tests/3_file.txt";
  FILE *f;
  FICHIER *f1;

  unsigned int taille = 1;
  unsigned int nbelem = 1;
  char *p = malloc(sizeof(char)*50);
  int nb_elem_lus = 0;

  /* Test d'ouverture de fichier en lecture */
  f1 = ouvrir (fichier, 'R');
  if (f1 == NULL) {
    printf("    Erreur lors de l'ouverture du fichier %s en lecture \n", fichier);
    exit (-1);
  }

  /* Test de la fonction lire */
  printf("    Fonction lire: \n");
  nb_elem_lus +=  lire(p+nb_elem_lus, taille, nbelem, f1);
  nb_elem_lus +=  lire(p+nb_elem_lus, taille, nbelem, f1);
  nb_elem_lus +=  lire(p+nb_elem_lus, taille, nbelem, f1);
  nb_elem_lus +=  lire(p+nb_elem_lus, taille, nbelem, f1);
  fermer (f1);
  printf("    nb_elem_lus: %d \n", nb_elem_lus);
  for (int i = 0; i<nb_elem_lus; i++) {
    printf("    %c\n", p[i]);
  }

  /* Test de la fonction fread() */
  printf("    Fonction fopen: \n");
  f = fopen(fichier, "r");
  nb_elem_lus = 0;
  nb_elem_lus += fread(p+nb_elem_lus,taille,nbelem, f);
  nb_elem_lus += fread(p+nb_elem_lus,taille,nbelem, f);
  nb_elem_lus += fread(p+nb_elem_lus,taille,nbelem, f);
  nb_elem_lus += fread(p+nb_elem_lus,taille,nbelem, f);
  fclose(f);
  printf("    nb_elem_lus: %d \n", nb_elem_lus);
  for (int i = 0; i<nb_elem_lus; i++) {
    printf("    %c\n", p[i]);
  }

  return 0;
}
