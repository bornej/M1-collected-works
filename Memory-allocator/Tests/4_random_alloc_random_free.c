#include "mem.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

void afficher_zone(void *adresse, size_t taille, int free)
{
	printf("Zone %s, Adresse : %lx, Taille : %lu\n", free?"libre":"occupee",
		   (unsigned long) adresse, (unsigned long) taille);
}

void afficher_zone_libre(void *adresse, size_t taille, int free)
{
	if (free)
		afficher_zone(adresse, taille, 1);
}

void afficher_zone_occupee(void *adresse, size_t taille, int free)
{
	if (!free)
		afficher_zone(adresse, taille, 0);
}

/*
  Alloue des blocs de taille alléatoire sans dépasser la taille de la mémoire.
  Libère les blocs dans un ordre aléatoire
 */
int main(int argc, char *argv[]) {
	char* allocated[20000];
	char* mem = get_memory_adr();
	size_t memsize = get_memory_size();
	int nb_iter = 10;

	srand(time(NULL));
	mem_init(mem, get_memory_size());

	printf("----------------------\n");
	printf("----------------------\n");
	printf("Début test.\n");
	printf("Initialisation.\n");
	printf("----------------------\n");
	printf("Etat mémoire initial.\n");

	mem_show(afficher_zone);

	printf("----------------------\n");

	/* allocation */
	for (int i = 0; i< nb_iter; i++) {
		int size = (rand() % ((int)memsize/nb_iter));
		allocated[i] = mem_alloc(size);
	}

	/* Shuffle */
	for (int i= 0; i< nb_iter; i++) {
		int j = (rand() % nb_iter);
		char* temp = allocated[i];
		allocated[i] = allocated[j];
		allocated[j] = temp;
	}

   	printf("----------------------\n");
	printf("Etat mémoire après allocation.\n");
	printf("----------------------\n");

	mem_show(afficher_zone);

	for (int i = nb_iter-1; i>= 0; i--) {
		mem_free(allocated[i]);
		printf("----------------------\n");
		printf("Etat mémoire après dé-allocation.\n");
		printf("----------------------\n");
		mem_show(afficher_zone);
	}

	printf("----------------------\n");
	printf("Fin test.\n");
	printf("----------------------\n");

	return 0;
}
