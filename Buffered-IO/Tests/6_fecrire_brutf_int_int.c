#include "stdes.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  FICHIER *f1;
  printf("*---------------------------* \n");
  printf("*- 6_fecrire_brutf_int_int -* \n");
  printf("*---------------------------* \n");
  f1 = ouvrir ("./Tests/6_out.txt", 'W');
  if (f1 == NULL)
    exit (-1);

  /* Ecrit deux entiers dans le fichier f1 */
  printf("    Ecriture avec fecrire_brutf() des entiers -123 et 9000 dans le fichier 6_out.txt\n");
  fecrire_brutf(f1,"%d%d",-123, 9000);
  fermer(f1);

  f1 = ouvrir("./Tests/6_out.txt", 'R');
  if (f1 == NULL)
    exit (-1);
  int* i = malloc(sizeof(int));
  int* j = malloc(sizeof(int));
  lire(i, sizeof(int),1,f1);
  lire(j, sizeof(int),1,f1);

  /* Lit et afficher les entiers */
  printf("    Lecture avec lire() des entiers -123 et 9000 depuis le fichier 6_out.txt\n");
  printf("i:%d\n", *i);
  printf("j:%d\n", *j);
  fermer(f1);
  return 0;
}
