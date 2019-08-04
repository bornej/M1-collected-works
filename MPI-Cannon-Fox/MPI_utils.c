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

/*
  void createSubGrid1DComm(int axis, MPI_Comm srcComm, MPI_Comm *comm)
  Sémantique:
    Soit srcComm un communicateur pour une topologie en grille.
	createSubGrid1DComm permet l'initialisation d'un communicateur comm pour les sous-grilles
    à une dimension de srcComm définies selon l'axe (axis) choisi.
    Pour une ligne: (axis == 1).
    Pour une colonne: (axis == 0).

  Exemple:
    Si srcComm correspond au communicateur d'une grille 2D
	createSubGrid1DComm(1, srcComm, comm) initialise un communicateur comm pour une ligne de la grille srcComm.
    createSubGrid1DComm(0, srcComm, comm) initialise un communicateur comm pour une colonne de la grille srcComm.

    Soit le communicateur de grille srcComm associé
    à la grille de processus:

    0  1  2  3
    4  5  6  7
    8  9  10 11
    12 13 14 15

    CreateSubGrid1dcomm(1, srcComm, comm) engendre les
    Communicateurs, comm, de lignes suivants:
    --------------
    | 0  1  2  3 |
    --------------
    --------------
    | 4  5  6  7 |
    --------------
    ...
*/
void createSubGrid1DComm(int axis, MPI_Comm srcComm, MPI_Comm *comm){
  /*
    (axis == 1) => remainDim = {0, 1}
    (axis == 0) => remainDim = {1, 0}.
  */
    int remainDim[DIM] = {0};
	remainDim[axis] = 1;
    MPI_Cart_sub(srcComm, remainDim , comm);
}

void createGrid(int nbNodes, MPI_Comm *comm_cart){
	int arrayNode[DIM] = {0};
	int periodic[DIM];
	int reorder = 1;

	for(int i=0; i<DIM; i++){
		periodic[i] = 1;
	}

	MPI_Dims_create(nbNodes, DIM, arrayNode);
	MPI_Cart_create(MPI_COMM_WORLD, DIM, arrayNode, periodic, reorder, comm_cart);
}

void init_comm_world_paramters(int *rank, int *size){
    /* rank <- rang du processus dans le channel */
    MPI_Comm_rank(MPI_COMM_WORLD, rank);
    /* size <- nombre de processus connectés dans MPI_COMM_WORLD */
    MPI_Comm_size(MPI_COMM_WORLD, size);
}

void init_grid_lines_and_columns(int *size, MPI_Comm *gridComm, int *myGridRank,
                                 MPI_Comm *lineComm, MPI_Comm *columnComm, int *myLineRank, int *myColRank){
    int coords[2];
    /* création de la grille de processus */
    createGrid(*size, gridComm);
    /* myGridrank <- rang du processus courant dans la grille */
    MPI_Comm_rank(*gridComm, myGridRank);
    /* creation d'un communicateur pour la ligne du processus courant */
    createSubGrid1DComm(1, *gridComm, lineComm);
    /* creation d'un communicateur pour la colonne du processus courant */
    createSubGrid1DComm(0, *gridComm, columnComm);
    /* MPI_Cart_coords on récupère le rang de sa propre ligne et colonne */
    MPI_Cart_coords(*gridComm, *myGridRank, 2, coords);
    *myLineRank = coords[0];
    *myColRank = coords[1];
}


/*
  init_scatter_params:
  Sémantique:
    Création d'un type MPI pour l'envoi de sous-blocs de matrices de taille N*N
    En partant d'une matrice de taille N*n * N*n.
    Initialisation des valeurs des paramètres pour l'opération de scatter (sendcounts, displacements).

  Inspiré de:
    https://stackoverflow.com/questions/9269399/sending-blocks-of-2d-array-in-c-using-mpi/9271753#9271753

  Nous avons explicité le calcul des décalages pas forcément très intuitif.
*/
void init_scatter_params(int n, int N, int *sendcounts, int *displs, MPI_Datatype *myType){
    int i,j;
    MPI_Datatype type;
    /* Dimensions des lignes et colonnes de la matrice principale*/
    int sizes[2]    = {(N*n),(N*n)};
    /* Dimension d'un sous-bloc à envoyer */
    int subsizes[2] = {N,N};
    /* Indices de l'élément de départ */
    int starts[2]   = {0,0};

    /* Initialisation d'un nouveau type "myType" correspondant à un sous-bloc */
    MPI_Type_create_subarray(2, sizes, subsizes, starts, MPI_ORDER_C, MPI_INT, &type);
    /* change la largeur du type*/
    MPI_Type_create_resized(type, 0, 1*sizeof(int), myType);
    MPI_Type_commit(myType);

    /* On veut envoyer un bloc N*N à chacun des n*n processus */
    for (i = 0; i<(n*n); i++) sendcounts[i] = 1;

    /* Calcul du décalage */
    /* Exemple:
       Soit la matrice M carrée 6x6:
       0 0 0|1 1 1
       0 0 0|1 1 1
       0 0 0|1 1 1       A0|A1
       -----------    =  --|--
       2 2 2|3 3 3       A2|A3
       2 2 2|3 3 3
       2 2 2|3 3 3

       On souhaite envoyer les sous-blocs A0, A1, A2, A3.
       On a défini un sous-type correspondant à un bloc 3x3 dont case de départ
       est la case M(0,0):

       starts
       |
       v
       0 0 0
       0 0 0 = A0
       0 0 0


       On rappelle qu'en mémoire la matrice est stoquée de la manière suivante.
       M = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, ...}
            ^
            |
         starts


      - Afin d'envoyer A1,
      On veut que starts pointe vers la l'indice de la première case de A1 = M(0,3) = M[0*6 + 3].
      M = {0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, ...}
                    ^
                    |
                  starts
           starts
             |
             v
       0 0 0|1 1 1
       0 0 0|1 1 1
       0 0 0|1 1 1

      Avec M(i,j) la notation mathématique usuelle et M[k] la représentation mémoire de M.
      Remarque:
        De manière générale on a, soit M une matrice carrée NxN
        M(i,j) = M[i*N + j].


      - Afin d'envoyer A2,
      On veut que starts pointe vers l'indice de la première case de A2 = M(3,0) = M[3*6 + 0] = M[18]
      - Afin d'envoyer A3,
      On veut que starts pointe vers l'indice de la première case de A3 = M(3,3) = M[3*6 + 3] = M[21]

      Pour résumer on doit calculer les décalages pour trouver les indices des cases
      M(0,0), M(0,3), M(3,0), M(3,3).
      Ainsi on à les décalages displs = {0, 3, 18, 21} à effectuer en partant de M[0] pour envoyer successivement les
      blocs A0, A1, A2, A3.

      De manière générale si on a une matrice M de taille (N*n * N*n)
      et que l'on veut envoyer des sous blocs de taille (N*N) à n*n processus, on doit calculer
      les décalages pour les cases M(i*N, j*N) avec i,j dans [0, n-1].

      On rappelle qu'on a M(i,j) = M[i*N + j] pour une matrice de taille N*N
      Donc pour une matrice de taille (N*n)*(N*n) on a
      M(i,j) = M[i*(N*n) + j]
      Ainsi on remplit le tableau displs de la manière suivante.
    */
    for (i = 0; i<n; i++){
        for(j = 0; j<n ; j++){
            displs[i*n+j] = (i*N)*(N*n) + (j*N) ;
            //printf("displ[%d]:%d\n", (i*n+j), displs[i*n+j]);
        }
    }
}

void cannon_algorithm(MPI_Comm *gridComm, int *myLineRank, int *myColRank, int *shiftSourceRank, int *shiftDestRank,
                      int *A, int *B, int *C, int *n, int N, MPI_Status *status){
    int i;
    /* Preskewing */
    /* Shift de A vers la gauche */
    /* On shifte la ligne i de i rangs à gauche */
    MPI_Cart_shift(*gridComm, 1, -(*myLineRank), shiftSourceRank, shiftDestRank);
    MPI_Sendrecv_replace(A, N*N, MPI_INT, *shiftDestRank, 0, *shiftSourceRank , 0, *gridComm, status);

    /* Shift de B vers le haut */
    /* On shifte la colonne i de i rangs vers le haut */
    MPI_Cart_shift(*gridComm, 0, -(*myColRank), shiftSourceRank, shiftDestRank);
    MPI_Sendrecv_replace(B, N*N, MPI_INT, *shiftDestRank, 0, *shiftSourceRank , 0, *gridComm, status);

    /* n étapes de l'algorithme de Cannon */
    for(i=0; i<*n; i++){
        matrixMult(A, B, C, N);
        /* Shift horizontal de A */
        MPI_Cart_shift(*gridComm, 1, -1, shiftSourceRank, shiftDestRank);
        MPI_Sendrecv_replace(A, N*N, MPI_INT, *shiftDestRank, 0, *shiftSourceRank , 0, *gridComm, status);
        /* Shift vertical de B */
        MPI_Cart_shift(*gridComm, 0, -1, shiftSourceRank, shiftDestRank);
        MPI_Sendrecv_replace(B, N*N, MPI_INT, *shiftDestRank, 0, *shiftSourceRank , 0, *gridComm, status);
    }

    /* Postskewing */
    /* Shift de A vers la droite */
    /* On shifte la ligne i de i rangs à droite */
    MPI_Cart_shift(*gridComm, 1, *myLineRank, shiftSourceRank, shiftDestRank);
    MPI_Sendrecv_replace(A, N*N, MPI_INT, *shiftDestRank, 0, *shiftSourceRank , 0, *gridComm, status);

    /* Shift de B vers le bas */
    /* On shifte la colonne i de i rangs vers le bas */
    MPI_Cart_shift(*gridComm, 0, *myColRank, shiftSourceRank, shiftDestRank);
    MPI_Sendrecv_replace(B, N*N, MPI_INT, *shiftDestRank, 0, *shiftSourceRank , 0, *gridComm, status);
}

void fox_algorithm(MPI_Comm *gridComm, MPI_Comm *lineComm, int *myLineRank, int *myColRank, int *shiftSourceRank, int *shiftDestRank,
                   int *A_temp, int *A, int *B, int *C, int n, int N, MPI_Status *status){
    int i, root;
    for(i=0; i<n; i++){
        /* On shifte B d'un rang vers le haut */
        if (i > 0) {
            MPI_Cart_shift(*gridComm, 0, 1, shiftSourceRank, shiftDestRank);
            MPI_Sendrecv_replace(B, N*N, MPI_INT, *shiftDestRank, 0, *shiftSourceRank , 0, *gridComm, status);
        }
        /* On broadcaste la i-ème diagonale de la matrice A originale*/
        root = (*myLineRank + i)%n;
        if (root == *myColRank){
            MPI_Bcast(A, N*N, MPI_INT, root, *lineComm);
            matrixMult(A, B, C, N);
        } else {
            /* On reçoit de la part du root la i-éme diagonale de A dans A_temp */
            MPI_Bcast(A_temp, N*N, MPI_INT, root, *lineComm);
            matrixMult(A_temp, B, C, N);
        }
    }
}
