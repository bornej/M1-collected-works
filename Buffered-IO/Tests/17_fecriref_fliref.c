#include "stdes.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  FICHIER *f1;
  printf("*-------------------------* \n");
  printf("*- 17_fecrire_char_string -* \n");
  printf("*-------------------------* \n");
  f1 = ouvrir ("./Tests/17_out.txt", 'W');
  if (f1 == NULL)
    exit (-1);
  printf("     Ecriture dans fichier 17_out.txt avec fecrire_brutf\n");
  fecriref(f1, "BONJOUR\n%c\n%s\n", 'a', "blabla");
  fermer(f1);

  f1 = ouvrir ("./Tests/17_out.txt", 'R');
  if (f1 == NULL)
    exit (-1);
  char c = '0';
  char* s = malloc(50);
  fliref(f1, "BONJOUR\n%c\n%s\n", &c, s);
  printf("lecture depuis le fichier du caractère 'a' et de la chaine blabla \n");
  printf("%c %s \n", c, s);

  return 0;
}
