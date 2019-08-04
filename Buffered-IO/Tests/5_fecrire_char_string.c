#include "stdes.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  FICHIER *f1;
  printf("*-------------------------* \n");
  printf("*- 5_fecrire_char_string -* \n");
  printf("*-------------------------* \n");
  f1 = ouvrir ("./Tests/5_out.txt", 'W');
  if (f1 == NULL)
    exit (-1);
  printf("Ecriture dans fichier 5_out.txt avec fecrire_brutf\n");
  fecrire_brutf(f1, "BONJOUR\n%c\n%s\n", 'a', "blabla");
  fermer(f1);

  return 0;
}
