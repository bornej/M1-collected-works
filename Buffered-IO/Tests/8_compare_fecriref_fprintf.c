#include "stdes.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  FICHIER *f1;
  FILE *f2;
  char* fichier1 = "./Tests/8_out_fecriref.txt";
  char* fichier2 = "./Tests/8_out_fprintf.txt";
  printf("*------------------------------* \n");
  printf("*- 8_compare_fecriref_fprintf -* \n");
  printf("*------------------------------* \n");
  f1 = ouvrir (fichier1, 'W');
  if (f1 == NULL)
    exit (-1);

  /* Ecrit deux entiers dans le fichier f1 */
  printf("    Ecriture avec fecriref() des entiers -123 et 9000 dans le fichier 8_out_fecriref.txt\n");
  fecriref(f1,"%d %d",-123, 9000);
  fermer(f1);

  f2 = fopen(fichier1, "r");
  if (f2 == NULL)
    exit (-1);
  int* i = malloc(sizeof(int));
  int* j = malloc(sizeof(int));
  //char* space = malloc(sizeof(char));
  /* Lit et afficher les entiers */
  fscanf(f2, "%d%d", i,j);
  printf("    Lecture avec fscanf() des entiers -123 et 9000 depuis le fichier 8_out_fecriref.txt\n");
  printf("i:%d\n", *i);
  printf("j:%d\n", *j);
  free(i);
  free(j);
  fclose(f2);

  i = malloc(sizeof(int));
  j = malloc(sizeof(int));
  f2 = fopen(fichier2, "w");
  printf("    Ecriture avec fprintf() des entiers -123 et 9000 dans le fichier 8_out_fprintf.txt\n");
  fprintf(f2, "%d %d", -123, 9000);
  fclose(f2);
  f2 = fopen(fichier2, "r");
  printf("    Lecture avec fscanf() des entiers -123 et 9000 depuis le fichier 8_out_fprintf.txt\n");
  fscanf(f2, "%d%d", i, j);
  printf("i:%d\n", *i);
  printf("j:%d\n", *j);
  fclose(f2);

  free(i);
  free(j);

  return 0;
}
