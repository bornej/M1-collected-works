/*  M1 Informatique IM2AG-UGA
 *  Parallel Algorithms and Programming - TP5:
 *  Borne Jonathan Isnel Maxime
 *
 *  Sujet et sources:
 *  F.Desprez (Inria), J-F. Méhaut (UGA/Polytech),
 *  T.Ropars (UGA/IM2AG), B. Videau (CEA).
 */
#include <stdio.h>

void initDiagMat(int *M, int n, int size);

void matrixMult(int *A, int *B, int *C, int size);

void fprintMat(FILE *f, int *M, int size);
