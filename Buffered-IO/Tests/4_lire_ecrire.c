#include "stdes.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	FICHIER *f1;
	FICHIER *f2;
	char c;
	int i = 0;
	printf("*-----------------* \n");
	printf("*- 4_lire_ecrire -* \n");
	printf("*-----------------* \n");
	f1 = ouvrir ("./Tests/4_in.txt", 'R');
	if (f1 == NULL)
		exit (-1);

	f2 = ouvrir ("./Tests/4_out.txt", 'W');
	if (f2 == NULL)
		exit (-1);

	printf("    Contenu du fichier lu:\n");
	while (lire (&c, 1, 1, f1) > 0) {
		printf("%c",c);
		i+=1;
		ecrire (&c, 1, 1, f2);
	}
	printf("\n");

	fermer (f1);
	fermer (f2);

	f2 = ouvrir ("./Tests/4_out.txt", 'R');
	if (f2 == NULL)
		exit (-1);

	char* p = malloc(i*(sizeof(char)));
	int n = lire(p, 1, i, f2);
	fermer (f2);

	printf("    Contenu du fichier résultat (écrit):\n");
	for (int k = 0; k<n; k++) {
		printf("%c", p[k]);
	}
	printf("\n");

	return 0;
}
