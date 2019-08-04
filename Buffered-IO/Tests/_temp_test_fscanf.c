#include "stdes.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  FICHIER *f1;
  FILE *f2;
  int return_value = 0;
  char* fichier1 = "./Tests/_temp_test_fscanf_in.txt";
  printf("*----------------------------* \n");
  printf("*- _temp_test_fscanf_in -* \n");
  printf("*----------------------------* \n");
  f2 = fopen (fichier1, "r");
  if (f2 == NULL)
    exit (-1);

  char* s = malloc(50);
  int i, j;

  return_value = fscanf(f2, "%d%s %d", &i, s, &j);

  printf("    fscanf: return_val:%d i:%d s:%s j:%d \n", return_value, i, s, j);
  free(s);
  fclose(f2);

  f1 = ouvrir(fichier1, 'R');
  if (f1 == NULL)
    exit (-1);

  s = malloc(50);
  i = 0;
  j = 0;

  return_value = fliref(f1, "%d%s %d", &i, s, &j);
  printf("    fliref: return_val:%d i:%d s:%s j:%d \n", return_value, i, s, j);
  free(s);
  fermer(f1);

  return 0;
}
