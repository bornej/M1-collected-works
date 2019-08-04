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
  Alloue des blocs de taille fixe et libère ces blocs dans un ordre aléatoire
*/
int main(int argc, char *argv[]) {
	char* allocated[20000];
	char* mem = get_memory_adr();
	int size = 0;
	int nb_iter = 100;

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

	for (int i = 0; i< nb_iter; i++) {
		allocated[i] = mem_alloc(size);
	}

   	printf("----------------------\n");
	printf("Etat mémoire après allocation.\n");
	printf("----------------------\n");

	mem_show(afficher_zone);

	/* Shuffle */
	for (int i= 0; i< nb_iter; i++) {
		int j = (rand() % nb_iter);
		char* temp = allocated[i];
		allocated[i] = allocated[j];
		allocated[j] = temp;
	}

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
