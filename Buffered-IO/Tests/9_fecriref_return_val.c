#include "stdes.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  FICHIER *f1;
  FILE *f2;
  int return_value = 0;
  char* fichier1 = "./Tests/9_out_fecriref.txt";
  char* fichier2 = "./Tests/9_out_fprintf.txt";
  printf("*-------------------------* \n");
  printf("*- 9_fecriref_return_val -* \n");
  printf("*-------------------------* \n");
  f1 = ouvrir (fichier1, 'W');
  if (f1 == NULL)
    exit (-1);

  /* Ecrit deux entiers dans le fichier f1 */
  printf("    Ecriture avec fecriref() des entiers -123 et 9000 dans le fichier 9_out_fecriref.txt\n");
  return_value = fecriref(f1,"%d %d",-123, 9000);
  fermer(f1);
  printf("    fecriref valeur de retour: %d \n", return_value);

  /* Ecrit deux entiers dans le fichier f2 */
  f2 = fopen(fichier2, "w");
  printf("    Ecriture avec fprintf() des entiers -123 et 9000 dans le fichier 9_out_fprintf.txt\n");
  return_value = fprintf(f2, "%d %d", -123, 9000);
  printf("    fprintf valeur de retour: %d \n", return_value);
  fclose(f2);

  return 0;
}
