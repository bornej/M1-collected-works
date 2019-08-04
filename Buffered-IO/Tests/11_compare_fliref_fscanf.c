#include "stdes.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  FILE *f2;
  int return_value = 0;
  char* fichier1 = "./Tests/11_in.txt";
  printf("*----------------------------* \n");
  printf("*- 11_compare_fliref_fscanf -* \n");
  printf("*----------------------------* \n");
  f2 = fopen (fichier1, "r");
  if (f2 == NULL)
    exit (-1);

  int i, j, k;

  return_value = fscanf(f2, "%d %d text %d", &i, &j, &k);

  printf("    fscanf: return_val:%d i:%d j:%d k:%d \n", return_value, i, j, k);

  fclose(f2);
  return 0;
}
