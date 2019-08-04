/*
M1 Informatique IM2AG-UGA
SipHash:
Borne Jonathan - Duverney Thomas

Siphash Demo:
Description:
  Vérification du fonctionnement de la fonction siphash avec les valeurs de test fournies page 19 du
  papier "SipHash: a fast short-input PRF :https://eprint.iacr.org/2012/351.pdf".

  Affiche les valeurs intermédiaires de la fonction
  Siphash_2_4(000102030405060708090a0b0c0d0e, 000102030405060708090a0b0c0d0e0f).
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint64_t leftRotate(uint64_t n, unsigned r){
  uint64_t res = n << r;
  return ( res | n >> (64 -r));
}

void sipround(uint64_t *v0, uint64_t *v1, uint64_t *v2, uint64_t *v3){
    *v0 += *v1;
    *v2 += *v3;
    *v1 = leftRotate(*v1, 13);
    *v3 = leftRotate(*v3, 16);
    *v1 ^= *v0;
    *v3 ^= *v2;
    *v0 = leftRotate(*v0, 32);
    *v2 += *v1;
    *v0 += *v3;
    *v1 = leftRotate(*v1, 17);
    *v3 = leftRotate(*v3, 21);
    *v1 ^= *v2;
    *v3 ^= *v0;
    *v2 = leftRotate(*v2, 32);
}

uint64_t siphash_2_4(uint64_t k[2], uint8_t *T, unsigned n){

  uint64_t v0 = k[0] ^ 0x736f6d6570736575;
  printf("v0:%lx ", v0);
  uint64_t v1 = k[1] ^ 0x646f72616e646f6d;
  printf(" v1:%lx", v1);
  uint64_t v2 = k[0] ^ 0x6c7967656e657261;
  printf(" v2:%lx", v2);
  uint64_t v3 = k[1] ^ 0x7465646279746573;
  printf(" v3:%lx\n", v3);
  int q = n / 8;
  int r = n % 8;

  uint64_t T2[q+1];
  int i, j;

  /*
     Pour les q premières cases de T2,
     on veut placer dans l'élément T2[q] (uint64), huit entiers uint8
     consécutifs du tableau T. L'adresse du premier de ces huits entiers
     est &T[i*8]. On caste cette adresse en adresse vers un entier sur 64 bits.
     Ainsi (uint64_t*) &T[i*8] est l'adresse de l'entier 0x(T[i*8+7]|T[i*8+6]|...|T[i*8]).
     On a plus qu'à déréférencer cette adresse et stoquer la valeur dans T2[q].
  */
  for (i = 0; i < q; i++) {
    T2[i] = 0x0000000000000000;
    T2[i] = *((uint64_t*) &T[i*8]);
  }

  /* Transfert des r derniers octets de T dans la dernière case de T2 */
  T2[q] = 0x0000000000000000;
  for (i = 0 ; i < r; i++) {
    T2[q] |= ((uint64_t) T[q*8+i]) << i*8 ;
  }

  /* Le dernier octet de la dernière case de T2 a pour valeur n (mod 256) */
  T2[q] |= ((uint64_t) (n % 256)) << 56;

  /* Affichage de T */
  printf("T : ");
  for (i = 0; i < n; i++) {
      printf("%x",  T[i]);
      if (i != n-1) printf(", ");
  }
  printf("\n");

  /* Affichage de T2 */
  for (i = 0; i < q+1; i++) {
      printf("T2[%d]:%lx\n",  i, T2[i]);
  }
   /* On passe chaque élément de T2 dans la moulinette */
  for (i = 0; i < q+1 ; i++) {

    printf("Processing T2 %d \n\n", i);

    v3 ^= T2[i];
    printf("Xored to v3 \n");
    printf("v0:%lx ", v0);
    printf(" v1:%lx", v1);
    printf(" v2:%lx", v2);
    printf(" v3:%lx\n", v3);
    /* c passes de sipround */
    for (j = 0; j < 2; j++) {
        sipround(&v0, &v1, &v2, &v3);
    }
    printf("after c passes of sipround \n");
    printf("v0:%lx ", v0);
    printf(" v1:%lx", v1);
    printf(" v2:%lx", v2);
    printf(" v3:%lx\n", v3);

    v0 ^= T2[i];

    printf("xored to v0\n");
    printf("v0:%lx ", v0);
    printf(" v1:%lx", v1);
    printf(" v2:%lx", v2);
    printf(" v3:%lx\n", v3);
  }

  /* Finalisation */

  v2 ^= 0xff;
  printf("xored to ff\n");
  printf("v0:%lx ", v0);
  printf(" v1:%lx", v1);
  printf(" v2:%lx", v2);
  printf(" v3:%lx\n", v3);

  /* d passes de sipround */
  for (i = 0; i < 4; i++) {
      sipround(&v0, &v1, &v2, &v3);
  }
  printf("D passes sipround \n");
  printf("v0:%lx ", v0);
  printf(" v1:%lx", v1);
  printf(" v2:%lx", v2);
  printf(" v3:%lx\n", v3);
  
  return (v0 ^ v1 ^ v2 ^ v3);
}


int main (){

  /* Exemple papier */

  uint64_t k[2] = {0x0706050403020100, 0x0f0e0d0c0b0a0908};
  uint8_t T[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e};
  uint64_t res = siphash_2_4(k, T, 15);
  /*
  uint64_t k[2] = {0,0};
  //uint8_t T[] = {0xab};
  uint8_t T[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
  //uint8_t T[] = {0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00};
  uint64_t res = siphash_2_4(k, T, 8);
  */
  printf("%lx \n", res);
  return 0;
}
