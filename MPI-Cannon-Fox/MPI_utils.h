/*  M1 Informatique IM2AG-UGA
 *  Parallel Algorithms and Programming - TP5:
 *  Borne Jonathan Isnel Maxime
 *
 *  Sujet et sources:
 *  F.Desprez (Inria), J-F. Méhaut (UGA/Polytech),
 *  T.Ropars (UGA/IM2AG), B. Videau (CEA).
 */
void createSubGrid1DComm(int axis, MPI_Comm srcComm, MPI_Comm *comm);

void createGrid(int nbNodes, MPI_Comm *comm_cart);

void init_comm_world_paramters(int *rank, int *size);

void init_grid_lines_and_columns(int *size, MPI_Comm *gridComm, int *myGridRank,
                                 MPI_Comm *lineComm, MPI_Comm *columnComm, int *myLineRank, int *myColRank);

void init_scatter_params(int n, int N, int *sendcounts, int *displs, MPI_Datatype *myType);

void cannon_algorithm(MPI_Comm *gridComm, int *myLineRank, int *myColRank, int *shiftSourceRank, int *shiftDestRank,
                      int *A, int *B, int *C, int *n, int N, MPI_Status *status);

void fox_algorithm(MPI_Comm *gridComm, MPI_Comm *lineComm, int *myLineRank, int *myColRank, int *shiftSourceRank,
                   int *shiftDestRank, int *A_temp, int *A, int *B, int *C, int n, int N, MPI_Status *status);
