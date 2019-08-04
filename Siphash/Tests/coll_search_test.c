/*
M1 Informatique IM2AG-UGA
Tp SipHash:
Borne Jonathan - Duverney Thomas

coll_search_test:
  calcul avec l'algorithme de Brent une unique collision de la fonction sip_hash_fix_32.
  affiche la longueur de la période de la suite u_n calculée
  par la fonction brentrec.
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "sipHash.h"
int main (){
  printf("------------------------- \n");
  printf("--   coll_search_test  -- \n");
  printf("------------------------- \n");

  uint32_t k = 0x00010203;
  uint64_t res = coll_search(k, *sip_hash_fix32);
  printf("Collision trouvée, taille période:%" PRId64 "\n", res);

  return 0;
}
