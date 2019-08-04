/*
M1 Informatique IM2AG-UGA
Tp SipHash:
Borne Jonathan - Duverney Thomas
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "sipHash.h"
int main (){

  printf("------------------------- \n");
  printf("--    Exemple papier   -- \n");
  printf("------------------------- \n");

  printf("Vérification du fonctionnement de la fonction siphash\n");
  printf("avec les valeurs de test fournies page 19 du papier SipHash:\n");
  printf(" a fast short-input PRF :https://eprint.iacr.org/2012/351.pdf \n");
  printf("Résultat attendu: a129ca6149be45e5\n");
  /* Exemple papier */

  uint64_t k[2] = {0x0706050403020100, 0x0f0e0d0c0b0a0908};
  uint8_t T[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e};
  uint64_t res = siphash_2_4(k, T, 15);
  printf("Résultat:\n");
  printf("%lx \n", res);
  return 0;
}
