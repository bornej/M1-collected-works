#include "stdes.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>




int main(int argc, char *argv[])
{
  printf("*---------------------* \n");
  printf("*- 2_open_read_close -*\n");
  printf("*---------------------* \n");
  char* fichier = "./Tests/1_file.txt";
  FICHIER *f1;

  /* Test d'ouverture de fichier en lecture */
  f1 = ouvrir (fichier, 'R');
  if (f1 == NULL) {
    printf("    Erreur lors de l'ouverture du fichier %s en lecture \n", fichier);
    exit (-1);
  }

  unsigned int taille = 1;
  unsigned int nbelem = 10;
  char* p = malloc(taille*nbelem*sizeof(char));
    /* Lecture depuis le fichier */

  int nb_elem_lus =  lire(p, taille, nbelem, f1);
  printf("     nb_elem_lus: %d \n", nb_elem_lus);
  for(int i = 0; i<taille*nb_elem_lus; i++) {
    printf("%c", p[i]);
  }
  printf("\n");
  fermer (f1);

  return 0;
}
