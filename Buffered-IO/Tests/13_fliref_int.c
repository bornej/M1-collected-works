#include "stdes.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  FICHIER *f1;
  int return_value = 0;
  char* fichier1 = "./Tests/13_in.txt";
  printf("*--------------------------------* \n");
  printf("*- 13_fliref_int                 -* \n");
  printf("*--------------------------------* \n");
  f1 = ouvrir (fichier1, 'R');
  if (f1 == NULL)
    exit (-1);

  int i = -1;
  int j = -1;
  int k = -1;
  return_value = fliref(f1, "%d %d %d", &i, &j, &k);
  printf("    lectures des trois valeurs -12332, -23 et 18 \n");
  printf("    fliref: return_val:%d %d %d %d\n", return_value, i, j, k);

  fermer(f1);
  return 0;
}
