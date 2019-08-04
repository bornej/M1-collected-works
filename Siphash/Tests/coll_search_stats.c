/*
M1 Informatique IM2AG-UGA
Tp SipHash:
Borne Jonathan - Duverney Thomas

Coll_search_stats:
Description:
  Mesure du nombre d'itérations de la fonction brentrec néccessaires au calcul
  d'une collision de la fonction siphash_fix_32.

  Calcule une collision pour 1000 clés différentes et écrit
  dans le fichier Collision.csv les valeurs finales des paramètres de la fonction brentrec
  à l'issue du calcul de chaque collision.

Structure du fichier csv:

  run_id;nb_iter;i;j;x;y;k
  0;27658;16383;11276;2591630181;2591630181;0

  - run_id: correspond a l'identifiant de la mesure effectuée dans le cas ou l'on lancerai la mesure
    plusieurs fois.
  - nb_iter: correspond aux nombre d'appel total de la fonction brent_rec.
  - i-j: correspond à la periode de la suite u_n définie par
     u_0 = sip_hash_fix32(k, 0)
     u_n = sip_hash_fix32(k, u_(n-1))
  - x: correspond à la valeur de u_i au moment de la collision
  - y: correspond à la valeur de u_j au moment de la collision (est égal à x).
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "sipHash.h"
#define N 1000
int main (){
  printf("------------------------- \n");
  printf("--   coll_search_stats -- \n");
  printf("------------------------- \n");

  printf("Calcule une collision pour 1000 valeurs de clé différentes et écrit\n");
  printf("dans le fichier Collision.csv les valeurs finales des paramètres de la\n");
  printf("fonction brentrec à l'issue du calcul de chaque collision.\n");

  printf("Structure du fichier csv:\n");

  printf("  run_id;nb_iter;i;j;x;y;k\n");
  printf("  0;27658;16383;11276;2591630181;2591630181;0\n\n");

  printf("   - run_id: correspond a l'identifiant de la mesure effectuée dans le cas ou l'on lancerai la mesure\n");
  printf("     plusieurs fois.\n");
  printf("   - nb_iter: correspond aux nombre d'appel total de la fonction brent_rec.\n");
  printf("   - i-j: correspond à la periode de la suite u_n définie par\n");
  printf("      u_0 = sip_hash_fix32(k, 0)\n");
  printf("      u_n = sip_hash_fix32(k, u_(n-1))\n");
  printf("   - x: correspond à la valeur de u_i au moment de la collision\n");
  printf("   - y: correspond à la valeur de u_j au moment de la collision (est égal à x).\n");

  uint32_t k = 0;
  int run_id = 0;
  int n = 0;
  FILE *f = fopen("./Collisions.csv", "w");
  fprintf(f, "run_id;nb_iter;i;j;x;y;k\n");
  for (n = 0; n < N; n++){
    coll_search_stats(f, run_id, k, *sip_hash_fix32);
    k += 1;
  }
  return 0;
}
