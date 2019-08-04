#include "stdes.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  FICHIER *f1;
  FILE *f2;
  char* fichier1 = "./Tests/7_out_fecrire_brutf.txt";
  char* fichier2 = "./Tests/7_out_fprintf.txt";
  printf("*-----------------------------------* \n");
  printf("*- 7_compare_fecrire_brutf_fprintf -* \n");
  printf("*-----------------------------------* \n");
  f1 = ouvrir (fichier1, 'W');
  if (f1 == NULL)
    exit (-1);

  /* Ecrit deux entiers dans le fichier f1 */
  printf("    Ecriture avec fecrire_brutf() des entiers -123 et 9000 dans le fichier 7_out_fecrire_brutf.txt\n");
  fecrire_brutf(f1,"%d %d",-123, 9000);
  fermer(f1);

  f1 = ouvrir(fichier1, 'R');
  if (f1 == NULL)
    exit (-1);
  int* i = malloc(sizeof(int));
  int* j = malloc(sizeof(int));
  char* space = malloc(sizeof(char));
  lire(i, sizeof(int),1,f1);
  lire(space, sizeof(char),1,f1);
  lire(j, sizeof(int),1,f1);

  /* Lit et afficher les entiers */
  printf("    Lecture avec lire() des entiers -123 et 9000 depuis le fichier 7_out_fichierf.txt\n");
  printf("i:%d\n", *i);
  printf("j:%d\n", *j);
  free(i);
  free(j);
  fermer(f1);

  i = malloc(sizeof(int));
  j = malloc(sizeof(int));
  f2 = fopen(fichier2, "w");
  printf("    Ecriture avec fecrire_brutf() des entiers -123 et 9000 dans le fichier 7_out_fprintf.txt\n");
  fprintf(f2, "%d %d", -123, 9000);
  fclose(f2);
  f2 = fopen(fichier2, "r");
  printf("    Lecture avec fscanf() des entiers -123 et 9000 depuis le fichier 7_out_fprintf.txt\n");
  fscanf(f2, "%d%d", i, j);
  printf("i:%d\n", *i);
  printf("j:%d\n", *j);
  fclose(f2);

  free(i);
  free(j);
  free(space);
  return 0;
}
