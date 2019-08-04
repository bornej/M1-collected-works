/*  M1 Informatique IM2AG-UGA
 *  Parallel Algorithms and Programming - TP5:
 *  Borne Jonathan Isnel Maxime
 *
 *  Sujet et sources:
 *  F.Desprez (Inria), J-F. Méhaut (UGA/Polytech),
 *  T.Ropars (UGA/IM2AG), B. Videau (CEA).
 */
#include <stdio.h>
#include "matrix_operations.h"

/*
   initDiagMat
   Semantique:
     Prend en paramètre une matrice M carrée d'entiers de taille size *size.
     Initialise la matrice de telle sorte que M soit diagonale avec
     pour valeur l'entier n sur la diagonale.
   Pré-conditions:
     M doit pointer vers le début d' une zone de mémoire contigue de taille (size * size) * sizeof(int).
 */
void initDiagMat(int *M, int n, int size){
	for(int i=0; i<size; i++){
		for(int j=0; j<size; j++){
            *(M + (i*size) + j) = i==j?n:0;
		}
	}
}

/*
   matrixMult
   Multiplication "naïve" de matrices
   par séquence de produits scalaires.
   Remarque:
   La multiplication que nous employons est tout sauf optimisée.
   On pourrait envisager les améliorations suivantes:

     - Utilisation de la directive openmp
      #pragma omp parallel for private(k) num_threads(nb_thread)
      pour paralléliser les calculs des produits scalaires.
     - Travailler avec une version transposée de la matrice B
       pour réduite le trashing (cache miss).

   Nous avons décidé de travailler avec la version naïve et de laisser
   ces considérations d'optimisation de côté.
   Nous pourrions comparer cette implémentation avec une implémentation
   performante (Blas).
*/
void matrixMult(int *A, int *B, int *C, int size){
    int i,j,k;
    for (k=0; k<size ; k++){
        for(i = 0; i<size; i++){
            for(j = 0; j<size ; j++){
                *(C + (i*size) + j) += (*(A + (i*size) + k)) * (*(B + (k*size) + j));
            }
        }
    }
}

/*
  frpintMat
  Sémantique:
    Écrit dans le fichier f, la matrice carrée d'entiers M de taille size *size.
  Pré-conditions:
    f doit être un desctipteur de fichier valide initialisé avec fopen,
    M doit pointer vers le début d' une zone de mémoire contiguë de taille (size * size) * sizeof(int).
*/
void fprintMat(FILE *f, int *M, int size){
	for(int i=0; i<size; i++){
		for(int j=0; j<size; j++){
          fprintf(f, "%d ", *(M + (i*size) + j));
		}
		fprintf(f, "\n");
	}
}
