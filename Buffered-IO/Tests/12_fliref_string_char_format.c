#include "stdes.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  FICHIER *f1;
  int return_value = 0;
  char* fichier1 = "./Tests/12_in.txt";
  printf("*--------------------------------* \n");
  printf("*- 12_fliref_string_char_format -* \n");
  printf("*--------------------------------* \n");
  f1 = ouvrir (fichier1, 'R');
  if (f1 == NULL)
    exit (-1);

  char* text = malloc(50);
  char c = 'a';
  char* autre = malloc(50);
  char* fin = malloc(50);
  return_value = fliref(f1, " %s %c %s %s", text, &c, autre, fin);
  printf("    fliref: return_val:%d %s %c %s %s\n", return_value, text, c, autre, fin);

  free(text);
  free(autre);
  free(fin);
  fermer(f1);

  return 0;
}
