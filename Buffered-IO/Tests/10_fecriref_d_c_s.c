#include "stdes.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  FICHIER *f1;
  FILE *f2;
  int return_value = 0;
  char* fichier1 = "./Tests/10_out_fecriref.txt";
  char* fichier2 = "./Tests/10_out_fprintf.txt";
  printf("*---------------------* \n");
  printf("*- 10_fecriref_d_c_s -* \n");
  printf("*---------------------* \n");
  f1 = ouvrir (fichier1, 'W');
  if (f1 == NULL)
    exit (-1);

  /* Ecrit deux entiers dans le fichier f1 */
  return_value = fecriref(f1,"%d %c %s",-123, 'Z', "test");
  fermer(f1);
  printf("    fecriref valeur de retour: %d \n", return_value);

  /* Ecrit deux entiers dans le fichier f2 */
  f2 = fopen(fichier2, "w");
  return_value = fprintf(f2,"%d %c %s",-123, 'Z', "test");
  printf("    fprintf valeur de retour: %d \n", return_value);
  fclose(f2);

  return 0;
}
