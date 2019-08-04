#include "stdes.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{

  FICHIER *f1;
  FILE *f2;
  int return_value = 0;
  char* fichier1 = "./Tests/15_in.txt";
  printf("*--------------------------------* \n");
  printf("*- 16_fliref_mixte -* \n");
  printf("*--------------------------------* \n");
  f1 = ouvrir (fichier1, 'R');
  if (f1 == NULL)
    exit (-1);

  char a = '0';
  char b = '0';
  char c = '0';
  char d = '0';

  int i = -1;
  char* s = malloc(50);

  return_value = fliref(f1, " %c %c %c \n %c %d %s", &a, &b, &c, &d, &i, s);

  printf("    lecture de la sequence: a b c d -256 bonjour  \n");
  printf("    fliref: return_val:%d %c %c %c %c %d %s\n", return_value, a, b, c, d, i, s);
  fermer(f1);

  free(s);

  a = '0';
  b = '0';
  c = '0';
  d = '0';
  i = -1 ;
  s = malloc(50);

  f2 = fopen(fichier1, "r");
  return_value = fscanf(f2, " %c %c %c \n %c %d %s", &a, &b, &c, &d, &i, s);

  printf("    lecture de la sequence: a b c d -256 bonjour  \n");
  printf("    fscanf: return_val:%d %c %c %c %c %d %s\n", return_value, a, b, c, d, i, s);
  fclose(f2);

  free(s);
  return 0;
}
