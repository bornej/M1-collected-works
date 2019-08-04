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
#define N 2

/*
  Test de l'algorithme de fox sur l'exemple suivant:
     A             B         C
  0 0 1 0       1 0 1 0   1 0 1 0
  0 0 0 1       0 1 0 1   0 1 0 1
  2 0 3 0   X   1 0 1 0 = 5 0 5 0
  0 2 0 3       0 1 0 1   0 5 0 5

  On considère une grille de 4 processus, P0, P1, P2, P3.
  On affecte à chaque processus les matrices Ai, Bi, Ci.
  avec  A0   A1       B0   B1       C0   C1
       0 0  1 0      1 0  1 0      0 0  0 0
       0 0  0 1      0 1  0 1      0 0  0 0
        A2   A3       B2   B3       C2   C3
       2 0  3 0      1 0  1 0      0 0  0 0
       0 2  0 3      0 1  0 1      0 0  0 0

  On déroule l'algorithme de fox:
  Etape 1:
  On broadcaste la première diagonale de A et on effectue les multiplcations locales Ci=Ai*Bi
  On obtient
        A0   A1       B0   B1       C0   C1
       0 0  0 0      1 0  1 0      0 0  0 0
       0 0  0 0      0 1  0 1      0 0  0 0
        A2   A3       B2   B3       C2   C3
       3 0  3 0      1 0  1 0      3 0  3 0
       0 3  0 3      0 1  0 1      0 3  0 3

  Etape 2
  On shifte B, on broadcast la seconde diagonale de A et on fait les multiplications locales
  on obtient
        A0   A1       B0   B1       C0   C1
       1 0  1 0      1 0  1 0      1 0  1 0
       0 1  0 1      0 1  0 1      0 1  0 1
        A2   A3       B2   B3       C2   C3
       2 0  2 0      1 0  1 0      5 0  5 0
       0 2  0 2      0 1  0 1      0 5  0 5
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
        printf("---      Fox Small Matrix                             ---\n");
        printf("---------------------------------------------------------\n");
    }
    MPI_Barrier(gridComm);

    /* initialisation des matrices A, A_temp et B */
    initDiagMat(A_temp, myGridRank, N);
    initDiagMat(A, myGridRank, N);
    initDiagMat(B, 1, N);

    snprintf(filename, 256, "%s%d%c%d", "./Results/SmallMatrixFox/A_", myLineRank, '_', myColRank);
    f[rank] = fopen(filename, "w");
    fprintMat(f[rank], A, N);
    fclose(f[rank]);

    snprintf(filename, 256, "%s%d%c%d", "./Results/SmallMatrixFox/B_", myLineRank, '_', myColRank);
    f[rank] = fopen(filename, "w");
    fprintMat(f[rank], B, N);
    fclose(f[rank]);

    /* n étapes de l'algorithme de Fox */
    fox_algorithm(&gridComm, &lineComm, &myLineRank, &myColRank, &shiftSourceRank, &shiftDestRank,
                  A_temp, A, B, C, n, N, &status);

    snprintf(filename, 256, "%s%d%c%d", "./Results/SmallMatrixFox/C_", myLineRank, '_', myColRank);
    f[rank] = fopen(filename, "w");
    fprintMat(f[rank], C, N);
    fclose(f[rank]);

    MPI_Finalize();
	return 0;
}
