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
  printf("*- 1_open_close_file -*\n");
  printf("*---------------------* \n");
  char* fichier = "./Tests/1_file.txt";
  FICHIER *f1;

  /* Test d'ouverture de fichier en lecture */
  f1 = ouvrir (fichier, 'R');
  if (f1 == NULL) {
    printf("    Erreur lors de l'ouverture du fichier %s en lecture \n", fichier);
    exit (-1);
  }

  fermer (f1);

  /* Test de lecture de fichier en écriture */
  f1 = ouvrir (fichier, 'W');
  if (f1 == NULL) {
    printf("    Erreur lors de l'ouverture du fichier %s en écriture \n", fichier);
    exit (-1);
  }

  fermer (f1);

  return 0;
}
