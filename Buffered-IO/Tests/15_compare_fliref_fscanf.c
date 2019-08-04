#include "stdes.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{

  FICHIER *f1;
  FILE *f2;
  int return_value = 0;
  char* fichier1 = "./Tests/14_in.txt";
  printf("*--------------------------------* \n");
  printf("*- 15_compare_fliref_fscanf -* \n");
  printf("*--------------------------------* \n");
  f1 = ouvrir (fichier1, 'R');
  if (f1 == NULL)
    exit (-1);

  char a = '0';
  char b = '0';
  char c = '0';
  char d = '0';
  return_value = fliref(f1, " %c %c %c \n %c", &a, &b, &c, &d);

  printf("    lecture de quatres caractères a b c d  \n");
  printf("    fliref: return_val:%d %c %c %c %c\n", return_value, a, b, c, d);
  fermer(f1);

  f2 = fopen(fichier1, "r");
  if (f2 == NULL)
    exit (-1);
  return_value = fscanf(f2, " %c %c %c \n %c", &a, &b, &c, &d);
  printf("    lecture de quatres caractères a b c d  \n");
  printf("    fscanf: return_val:%d %c %c %c %c\n", return_value, a, b, c, d);
  fclose(f2);

  return 0;
}
