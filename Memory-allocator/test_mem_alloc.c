#include "mem.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[]) {
	char* mem = get_memory_adr();
	mem_init(mem, get_memory_size());
	/* allocation */
	char* bloc = mem_alloc(127910);
	printf("taille bloc par mem_get_size: %zu\n", mem_get_size(bloc));
	/* taille inférieure a 24 */
	mem_alloc(10);
	/* plus de memoire */
	mem_alloc(18);
	/* mem_alloc fail */
	mem_alloc(1);
	printf("Fin init \n");
	printf("-----------\n");
	print_mem(mem);
	return 0;
}
