uint64_t siphash_2_4(uint64_t k[2], uint8_t *T, unsigned n);

uint32_t sip_hash_fix32(uint32_t k, uint32_t m);

/*
  coll_search
  Sémantique:
  Utilisation de l'algorithme de Brent pour la recherche de Collisions
  dans l'espace des hash d'une fonction de hashage cryptographique
  générant des hash sur 32 bits.
  Spécification:
  Prends en paramètre un entier k représentant la clé pour la fonction de hashage
  dont l'adresse (*fun) est fournie en second argument.
  Retourne la longueur de la période de la suite calculée par la fonction brentrec.
 */
uint64_t coll_search(uint32_t k, uint32_t (*fun)(uint32_t , uint32_t));

/*
  coll_search_stats
  Spécification:
    Prends en paramètre un descripteur de fichier initialisé avec un appel à fopen.
    Un entier permettant d'identifier la mesure courante.
    Un entier k représentant la clé pour la fonction de hashage
    dont l'adresse (*fun) est fournie en second argument.
  Sémantique:
  Ecrit dans le fichier f les valeurs finales des paramètres de la fonction sip_hash_fix_32
  à l'issue du calcul d'une collision.
  Les résultats sont écrits dans le fichier au format csv suivant:

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
void coll_search_stats(FILE *f, int run_id, uint32_t k, uint32_t (*fun)(uint32_t , uint32_t));

/*
  brentrec
  Sémantique:
  Implémentation de l'algorithme de Brent pour la recherche de
  la période d'une suite périodique.
  Voir cours d'Algèbre de Michel Demazure (p48).
  Application de l'algorithme à la recherche de collisions pour une fonction de Hashage cryptographique sur 32 bits:

  Soit fun une fonction de hashage génèrant des hash de 32 bits.
  L'espace des hash est un ensemble fini de taille 2^32 - 1.
  Donc la suite réccurente u_n définie par:
   _ u_0 = fun(k, 0);
   _ u_n = fun(k, u_(n-1))
  est périodique.
  Ainsi la recherche de collision se réduit, à appliquer la fonction
  de hashage sur ses propres sorties.
  On obtient une collision quand on trouve i, j avec i≠j tels que
  u_i = u_j. L'algorithme de brent permet le calcul de la période (j-i)
  d'une telle suite sans avoir à mémoriser les valeurs u_1, u_2,...
  En particulier l'algorithme nous permet de detecter les collisions.

  Spécifications:
  Prends en paramètre deux entiers i, j:
     indices pour les termes de la suite dont on calcule la période.
  Deux entiers unsigned sur 32 bits x, y:
     Valeurs des termes u_i et u_j, respectivement.
  Un entier unsigned sur 32 bits k:
     Clé pour la fonction de hashage cryptographique.
  Un pointeur *fun vers une fonction dont la signature correspond
  à celle de sip_hash_fix32.
  Retourne la longueur de la période de la suite u_n.
 */
uint64_t brentrec(int i, int j, uint32_t x, uint32_t y, uint32_t k, uint32_t (*fun)(uint32_t , uint32_t));
