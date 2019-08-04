#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
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
  uint64_t v1 = k[1] ^ 0x646f72616e646f6d;
  uint64_t v2 = k[0] ^ 0x6c7967656e657261;
  uint64_t v3 = k[1] ^ 0x7465646279746573;
  int q = n / 8;
  int r = n % 8;
  uint64_t T2[q+1];
  int i, j;

  /*
     Pour les q premières cases de T2,
     on veut placer dans l'élément T2[i] (uint64), huit entiers uint8
     consécutifs du tableau T. L'adresse du premier de ces huits entiers
     est &T[i*8] (i dans [0,q-1]). On caste &T[i*8] en adresse vers un entier sur 64 bits.
     Ainsi (uint64_t*) &T[i*8] est l'adresse de l'entier 0x(T[i*8+7]|T[i*8+6]|...|T[i*8]).
     On a plus qu'à déréférencer cette adresse et stoquer la valeur dans T2[i].
  */
  for (i = 0; i < q; i++) {
    T2[i] = 0x0000000000000000;
    T2[i] = *((uint64_t*) &T[i*8]);
  }

  /* Transfert des r derniers octets de T dans la dernière case de T2 */
  /* TODO tranformer l'itération en une seule opération avec un masque */
  T2[q] = 0x0000000000000000;
  for (i = 0 ; i < r; i++) {
    T2[q] |= ((uint64_t) T[q*8+i]) << i*8 ;
  }

  /* Le dernier octet de la dernière case de T2 a pour valeur n (mod 256) */
  T2[q] |= ((uint64_t) (n % 256)) << 56;

  /* Xor/c*sipround/Xor sur les éléments de T2 */
  for (i = 0; i < q+1 ; i++) {
      v3 ^= T2[i];
      for (j = 0; j < 2; j++) {
          sipround(&v0, &v1, &v2, &v3);
      }
      v0 ^= T2[i];
  }

  /* Finalisation */
  v2 ^= 0xff;
  /* d*sipround */
  for (i = 0; i < 4; i++) {
      sipround(&v0, &v1, &v2, &v3);
  }
  return (v0 ^ v1 ^ v2 ^ v3);
}

uint32_t sip_hash_fix32(uint32_t k, uint32_t m) {
  uint64_t k1[2] = {(uint64_t) k, (uint64_t) k};
  uint8_t *m1 = malloc(4*sizeof(uint8_t));
  *((uint32_t*) m1) = m;
  uint64_t res = siphash_2_4(k1,m1,4);
  return ((uint32_t) res);
}


uint64_t brentrec(uint64_t i, uint64_t j, uint32_t x, uint32_t y, uint32_t k, uint32_t (*fun)(uint32_t , uint32_t)){
    if(x == y){
        printf("Collision: i:%" PRId64 " j:%" PRId64 " x:%u y:%u \n", i, j, x, y);
        return (i-j);
    }
    else if (j > i) return(brentrec(i + j, 1, y, fun(k,y), k, fun));
    else return(brentrec(i, j+1, x, fun(k, y), k, fun));
}


uint64_t coll_search(uint32_t k, uint32_t (*fun)(uint32_t , uint32_t)){
    uint64_t i = 0;
    uint64_t j = 1;
    uint32_t x = fun(k, 0x00000000);
    uint32_t y = fun(k, x);
    return(brentrec(i, j, x, y, k, fun));
}

void brentrec_stats(FILE *f, int run_id, int nb_iter, int i, int j, uint32_t x, uint32_t y, uint32_t k, uint32_t (*fun)(uint32_t , uint32_t)){
  if(x == y) fprintf(f, "%d;%d;%d;%d;%u;%u;%u\n", run_id, nb_iter, i, j, x, y, k);
  else if (j > i) brentrec_stats(f, run_id, nb_iter+1, i + j, 1, y, fun(k,y), k, fun);
  else brentrec_stats(f, run_id, nb_iter+1, i, j+1, x, fun(k, y), k, fun);
}

void coll_search_stats(FILE *f, int run_id, uint32_t k, uint32_t (*fun)(uint32_t , uint32_t)){
    int nb_iter = 0;
    int i = 0;
    int j = 1;
    uint32_t x = fun(k, 0x00000000);
    uint32_t y = fun(k, x);
    brentrec_stats(f, run_id, nb_iter, i, j, x, y, k, fun);
}
