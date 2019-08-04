/*  M1 Informatique IM2AG-UGA
 *  Parallel Algorithms and Programming - TP2:
 *  Borne Jonathan Isnel Maxime
 *
 *  Sujet et sources:
 *  F.Desprez (Inria), J-F. Méhaut (UGA/Polytech),
 *  T.Ropars (UGA/IM2AG), B. Videau (CEA).
 */

#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <stdlib.h>
#include <x86intrin.h>

#define NBEXPERIMENTS    100
#define N              1024

static long long unsigned int experiments[NBEXPERIMENTS];
typedef double vector[N];
typedef double matrix[N][N];

unsigned int nb_thread;
unsigned int chunksize;

matrix M;
vector v1;
vector v2;

long long unsigned int average(long long unsigned int *exps)
{
    unsigned int i;
    long long unsigned int s = 0;

    for (i = 2; i < (NBEXPERIMENTS - 2); i++) {
        s = s + exps[i];
    }

    return s / (NBEXPERIMENTS - 2);
}

void init_vector(vector X, const double val)
{
    register unsigned int i;

    for (i = 0; i < N; i++) {
        X[i] = val;
    }

    return;
}

void init_matrix_inf(matrix X, double val)
{
    register unsigned int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < i; j++) {
            X[i][j] = val;
            X[j][i] = 0.0;
        }
        X[i][i] = val;
    }
}

void print_vector(vector X)
{
    register unsigned int i;

    for (i = 0; i < N; i++) {
        printf(" %3.2f", X[i]);
    }
    printf("\n\n");
    fflush(stdout);

    return;
}

void print_matrix(matrix M)
{
    register unsigned int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            printf(" %3.2f ", M[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    return;
}


void mult_mat_vector(matrix M, vector b, vector c)
{
    register unsigned int i;
    register unsigned int j;
    register double r;

    for (i = 0; i < N; i = i + 1) {
        r = 0.0;
        for (j = 0; j < N; j = j + 1) {
            r += M[i][j] * b[j];
        }
        c[i] = r;
    }
    return;
}
/*
 * void mult_mat_vector_tri_inf(matrix M, vector b, vector c)
 *
 * Description: Multiplication d'une matrice triangulaire inférieure
 *   et d'un vecteur. Afin de réduire le nombre d'opérations,
 *   on se sert de l'hypothèse que la matrice est triangulaire
 *   pour ignorer les valeurs au dessus de sa diagonale.
 * Spécification: Prends en paramètres:
 *   - Une matrice M, première opérande de la multiplication
 *   - Un vecteur b, seconde opérande
 *   - Un vecteur c, résultat de l'opération.
 * Préconditions:
 *   - La matrice M est carrée de taille N et est triangulaire inférieure.
 *   - Les vecteurs b et c sont de taille N.
 */
void mult_mat_vector_tri_inf(matrix M, vector b, vector c)
{
    register unsigned int i;
    register unsigned int j;
    register double r;

    for (i = 0; i < N; i = i + 1) {
        r = 0.0;
        for (j = 0; j <= i; j = j + 1) {
            r += M[i][j] * b[j];
        }
        c[i] = r;
    }
    return;
}

/*
 * void mult_mat_vector_tri_inf1(matrix M, vector b, vector c)
 *
 * Description: Multiplication d'une matrice triangulaire inférieure
 *   et d'un vecteur (version parallèle, ordonancement statique).
 * Spécification: Prends en paramètres:
 *   - Une matrice M, première opérande de la multiplication
 *   - Un vecteur b, seconde opérande
 *   - Un vecteur c, résultat de l'opération.
 * Préconditions:
 *   - La matrice M est carrée de taille N et est triangulaire inférieure.
 *   - Les vecteurs b et c sont de taille N.
 */
void mult_mat_vector_tri_inf1(matrix M, vector b, vector c)
{
    register int i;
    register int j;

#pragma omp parallel for schedule(static, chunksize) private(i, j) num_threads(nb_thread)
    for (i = 0; i < N; i++) {
        for (j = N - 1; j >= 0; j--) {
            c[i] += b[j] * M[i][j];
        }
    }
    return;
}

/*
 * void mult_mat_vector_tri_inf1(matrix M, vector b, vector c)
 *
 * Description: Multiplication d'une matrice triangulaire inférieure
 *   et d'un vecteur (version parallèle, ordonancement dynamique).
 * Spécification: Prends en paramètres:
 *   - Une matrice M, première opérande de la multiplication
 *   - Un vecteur b, seconde opérande
 *   - Un vecteur c, résultat de l'opération.
 * Préconditions:
 *   - La matrice M est carrée de taille N et est triangulaire inférieure.
 *   - Les vecteurs b et c sont de taille N.
 */
void mult_mat_vector_tri_inf2(matrix M, vector b, vector c)
{
    register int i;
    register int j;

#pragma omp parallel for schedule(dynamic, chunksize) private(i, j) num_threads(nb_thread)
    for (i = 0; i < N; i++) {
        for (j = N - 1; j >= 0; j--) {
            c[i] += b[j] * M[i][j];
        }
    }
    return;
}

/*
 * void mult_mat_vector_tri_inf1(matrix M, vector b, vector c)
 *
 * Description: Multiplication d'une matrice triangulaire inférieure
 *   et d'un vecteur (version parallèle, ordonancement guidé).
 * Spécification: Prends en paramètres:
 *   - Une matrice M, première opérande de la multiplication
 *   - Un vecteur b, seconde opérande
 *   - Un vecteur c, résultat de l'opération.
 * Préconditions:
 *   - La matrice M est carrée de taille N et est triangulaire inférieure.
 *   - Les vecteurs b et c sont de taille N.
 */
void mult_mat_vector_tri_inf3(matrix M, vector b, vector c)
{
    register int i;
    register int j;

#pragma omp parallel for schedule(guided, chunksize) private(i, j) num_threads(nb_thread)
    for (i = 0; i < N; i++) {
        for (j = N - 1; j >= 0; j--) {
            c[i] += b[j] * M[i][j];
        }
    }

    return;
}

void mult_mat_vector_tri_inf4(matrix M, vector b, vector c)
{
    /*
      this function is parallel (with OpenMP directive, runtime scheduling)
      Computes the Multiplication between the vector b and the Triangular Lower Matrix
    */
    register int i;
    register int j;

#pragma omp parallel for schedule(runtime) private(i, j) num_threads(nb_thread)
    for (i = 0; i < N; i++) {
        for (j = N - 1; j >= 0; j--) {
            c[i] += b[j] * M[i][j];
        }
    }
    return;
}

int main(int argc, char **argv)
{
    int nthreads;
    unsigned long long start, end;
    unsigned long long residu;
    unsigned long long av;
    unsigned int exp;
    double r;

    if (argc != 4) {
        printf("Entrée attendue: nombre threads; chunksize; runID\n");
        exit(1);
    }

    nb_thread = atoi(argv[1]);
    chunksize = atoi(argv[2]);
    int runID = atoi(argv[3]);


    printf("number of threads %d\n", nb_thread);

    start = _rdtsc();
    end = _rdtsc();
    residu = end - start;

    /* ---------------------------- */
    /* --- Version séquentielle --- */
    /* ---------------------------- */

    init_vector(v1, 1.0);
    init_matrix_inf(M, 2.0);

    for (exp = 0; exp < NBEXPERIMENTS; exp++) {
        start = _rdtsc();

        mult_mat_vector(M, v1, v2);

        end = _rdtsc();
        experiments[exp] = end - start;
    }

    av = average(experiments);

    fprintf(stderr, "0;%d;%d;%d;%llu\n", nb_thread, chunksize, runID,
            av - residu);
    printf("Full matrix multiplication vector \t\t  %Ld cycles\n",
           av - residu);


    /* ----------------------------------------------------- */
    /* --- Version séquentielle: triangulaire inférieure --- */
    /* ----------------------------------------------------- */

    init_vector(v1, 1.0);
    init_matrix_inf(M, 2.0);


    for (exp = 0; exp < NBEXPERIMENTS; exp++) {
        start = _rdtsc();

        mult_mat_vector_tri_inf(M, v1, v2);
        end = _rdtsc();
        experiments[exp] = end - start;
    }

    av = average(experiments);

    fprintf(stderr, "1;%d;%d;%d;%llu\n", nb_thread, chunksize, runID,
            av - residu);
    printf("Triangular Matrix multiplication vector\t\t  %Ld cycles\n",
           av - residu);

    /* ------------------------------------------------- */
    /* --- Version parallèle: ordonancement statique --- */
    /* ------------------------------------------------- */

    init_vector(v1, 1.0);
    init_matrix_inf(M, 2.0);

    for (exp = 0; exp < NBEXPERIMENTS; exp++) {
        start = _rdtsc();

        mult_mat_vector_tri_inf1(M, v1, v2);

        end = _rdtsc();
        experiments[exp] = end - start;
    }

    av = average(experiments);

    fprintf(stderr, "2;%d;%d;%d;%llu\n", nb_thread, chunksize, runID,
            av - residu);
    printf("Parallel Loop Static Scheduling \t\t  %Ld cycles\n",
           av - residu);

    /* -------------------------------------------------- */
    /* --- Version parallèle: ordonancement dynamique --- */
    /* -------------------------------------------------- */

    init_vector(v1, 1.0);
    init_matrix_inf(M, 2.0);
    for (exp = 0; exp < NBEXPERIMENTS; exp++) {
        start = _rdtsc();

        mult_mat_vector_tri_inf2(M, v1, v2);
        end = _rdtsc();
        experiments[exp] = end - start;
    }

    av = average(experiments);
    fprintf(stderr, "3;%d;%d;%d;%llu\n", nb_thread, chunksize, runID,
            av - residu);
    printf("Parallel Loop Dynamic Scheduling \t\t  %Ld cycles\n",
           av - residu);

    /* ---------------------------------------------- */
    /* --- Version parallèle: ordonancement guidé --- */
    /* ---------------------------------------------- */

    init_vector(v1, 1.0);
    init_matrix_inf(M, 2.0);
    for (exp = 0; exp < NBEXPERIMENTS; exp++) {
        start = _rdtsc();

        mult_mat_vector_tri_inf3(M, v1, v2);

        end = _rdtsc();
        experiments[exp] = end - start;
    }

    av = average(experiments);

    fprintf(stderr, "4;%d;%d;%d;%llu\n", nb_thread, chunksize, runID,
            av - residu);
    printf("Parallel Loop Guided Scheduling \t\t  %Ld cycles\n",
           av - residu);

    /* ------------------------------------------------------------ */
    /* ---                 Version parallèle:                   --- */
    /* --- choix de la politique d'ordonancement à l'execution  --- */
    /* ------------------------------------------------------------ */

    init_vector(v1, 1.0);
    init_matrix_inf(M, 2.0);

    for (exp = 0; exp < NBEXPERIMENTS; exp++) {
        start = _rdtsc();
        mult_mat_vector_tri_inf4(M, v1, v2);
        end = _rdtsc();
        experiments[exp] = end - start;
    }

    av = average(experiments);
    printf("Parallel Loop Runtime Scheduling \t\t  %Ld cycles\n",
           av - residu);
    return 0;
}
