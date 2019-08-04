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
#define N 16

void init_global_matrices(int n, int **Global_A, int **Global_C){
    *Global_A = (int *)malloc(N * N * sizeof(int) * n * n);
    *Global_C = (int *)malloc(N * N * sizeof(int) * n * n);
    initDiagMat(*Global_A, 1, N*n); /* Identité */
}

int main(int argc, char **argv){
    int n = 2;/* nombre de colonnes/lignes de la grille de processus */
    /* Communicateurs */
    MPI_Comm gridComm, lineComm, columnComm;
    /* Rangs du procesus courant */
    int rank, myGridRank, myLineRank, myColRank;
    /* Scatter and Gather parameters*/
    MPI_Datatype myType;
    int *sendcounts = (int*) malloc( n*n*sizeof(int));
    int *displs = (int*) malloc(n*n*sizeof(int));
    /* Shift parameters */
    int size;
    /* Fichier résultat*/
    FILE *f[n*n];
    char *filename = (char *) malloc(256 * sizeof(char));
    /* Matrices */
    int *Global_A = NULL; /* Initialisée seulement dans le processus 0 */
    int *Global_C = NULL; /* Initialisée seulement dans le processus 0 */
    int *A = (int *)malloc(N * N * sizeof(int));
    int *C = (int *)malloc(N * N * sizeof(int));
    initDiagMat(C, 0, N);

    /* Initialisation des communicateurs (world, Grid, lines, columns) MPI*/
    MPI_Init(&argc, &argv);
    init_comm_world_paramters(&rank, &size);
    init_grid_lines_and_columns(&size, &gridComm, &myGridRank,
                                &lineComm, &columnComm, &myLineRank, &myColRank);

    if (rank == 0){
        printf("---------------------------------------------------------\n");
        printf("---              Scatter & Gather test                ---\n");
        printf("---        Scattering d'une matrice identité:         ---\n");
        printf("---        La matrice est envoyée par bloc            ---\n");
        printf("---        depuis le processus 0 vers quatres         ---\n");
        printf("---        processus. Les blocs sont ensuites         ---\n");
        printf("---        renvoyés pour être réassemblés             ---\n");
        printf("---        par le processus 0.                        ---\n");
        printf("---        Voir ./Results/scatter_gather/A_i_j        ---\n");
        printf("---        Pour le résultat de scattering.            ---\n");
        printf("---        Voir ./Results/scatter_gather/C            ---\n");
        printf("---        Pour le résultat du gathering              ---\n");
        printf("---------------------------------------------------------\n");
    }
    MPI_Barrier(gridComm);

    /* Initialisation par le processus 0 des matrices globales Global_A et Global_B */
    if(rank == 0) {
        init_global_matrices(n, &Global_A, &Global_C);
    }

    /* Initialisation des paramètres pour les opération scatter et gather */
    init_scatter_params(n, N, sendcounts, displs, &myType);
    /* Envoi des sous blocs de A_Global aux processus */
    MPI_Scatterv(Global_A, sendcounts, displs, myType, A, N*N, MPI_INT, 0, gridComm);

    /* Vérification que l'operation de Scattering a bien fonctionné sur A */
    snprintf(filename, 256, "%s%d%c%d", "./Results/scatter_gather/A_", myLineRank, '_', myColRank);
    f[rank] = fopen(filename, "w");
    fprintMat(f[rank], A, N);
    fclose(f[rank]);

    /* Reconstruction de la matrice Global_A originale à partir des blocs A */
    MPI_Gatherv(A, N*N, MPI_INT, Global_C, sendcounts, displs, myType, 0, gridComm);

    if(rank == 0){
        snprintf(filename, 256, "%s", "./Results/scatter_gather/C");
        f[rank] = fopen(filename, "w");
        fprintMat(f[rank], Global_C, N*n);
        fclose(f[rank]);
    }

    MPI_Finalize();
	return 0;
}
