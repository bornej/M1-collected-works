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
  printf("-- sip_hash_fix32_test -- \n");
  printf("------------------------- \n");
  uint32_t m = 0x00010203;
  uint32_t k = 0x00010203;
  uint32_t res =  sip_hash_fix32(k, m);
  printf("message 32 bits:%x \n", m);
  printf("clé 32 bits:%x \n", k);
  printf("resultat 32 bits:%x \n", res);
  return res;
}
