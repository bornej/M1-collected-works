/*  M1 Informatique IM2AG-UGA
 *  Parallel Algorithms and Programming - TP5:
 *  Borne Jonathan Isnel Maxime
 *
 *  Sujet et sources:
 *  F.Desprez (Inria), J-F. Méhaut (UGA/Polytech),
 *  T.Ropars (UGA/IM2AG), B. Videau (CEA).
 */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix_operations.h"
#include "MPI_utils.h"
#define DIM 2
#define N 1024
/*
  Test de l'algorithme de fox sur l'exemple suivant:
     A             B         C
  0 0 1 0       1 0 1 0   1 0 1 0
  0 0 0 1       0 1 0 1   0 1 0 1
  2 0 3 0   X   1 0 1 0 = 5 0 5 0
  0 2 0 3       0 1 0 1   0 5 0 5
*/
int main(int argc, char **argv){
/* nombre de colonnes/lignes de la grille de processus */
    int n = 2;
    /* Communicateurs */
    MPI_Comm gridComm, lineComm, columnComm;
    MPI_Status status;
    /* Rangs du procesus courant */
    int rank, myGridRank, myLineRank, myColRank;

    /* Shift parameters */
    int shiftSourceRank, shiftDestRank, size;
    /* Fichier résultat*/
    FILE *f[n*n];
    char *filename = (char *) malloc(256 * sizeof(char));
    /* Matrices */
    int *A_temp = (int *)malloc(N * N * sizeof(int));
    int *A = (int *)malloc(N * N * sizeof(int));
    int *B = (int *)malloc(N * N * sizeof(int));
    int *C = (int *)malloc(N * N * sizeof(int));
    initDiagMat(C, 0, N);

    /* Initialisation des communicateurs (world, Grid, lines, columns) MPI*/
    MPI_Init(&argc, &argv);
    init_comm_world_paramters(&rank, &size);
    init_grid_lines_and_columns(&size, &gridComm, &myGridRank,
                                &lineComm, &columnComm, &myLineRank, &myColRank);

    if (rank == 0){
        printf("---------------------------------------------------------\n");
        printf("---      Fox Large Matrix                             ---\n");
        printf("---------------------------------------------------------\n");
    }
    MPI_Barrier(gridComm);

    /* initialisation des matrices A, A_temp et B */
    initDiagMat(A_temp, myGridRank, N);
    initDiagMat(A, myGridRank, N);
    initDiagMat(B, 1, N);

    /* n étapes de l'algorithme de Fox */
    fox_algorithm(&gridComm, &lineComm, &myLineRank, &myColRank, &shiftSourceRank, &shiftDestRank,
                  A_temp, A, B, C, n, N, &status);

    snprintf(filename, 256, "%s%d%c%d", "./Results/LargeMatrixFox/C_", myLineRank, '_', myColRank);
    f[rank] = fopen(filename, "w");
    fprintMat(f[rank], C, N);
    fclose(f[rank]);


    /* Vérifie que la diagonale de C vaut 1 pour les processus de rangs 0 et 1
       et vaut 5 pour les processus de rang 2 et 3 */
    if(myGridRank == 0 || myGridRank == 1){
        int verif_diag_1 = 1;
        for (int i = 0; i < N; i++){
            verif_diag_1 *= (*(C + i*N +i)) == 1;
        }
        if(verif_diag_1) printf("rank:%d OK\n", myGridRank);
        else printf("rank:%d Erreur multiplication\n", myGridRank);

    }

    if(myGridRank == 2 || myGridRank == 3){
        int verif_diag_5 = 1;
        for (int i = 0; i < N; i++){
            verif_diag_5 *= (*(C + i*N +i)) == 5;
        }
        if(verif_diag_5) printf("rank:%d OK\n", myGridRank);
        else printf("rank:%d Erreur multiplication\n", myGridRank);
    }

    MPI_Finalize();
	return 0;
}
