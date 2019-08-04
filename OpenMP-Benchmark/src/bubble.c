/*  M1 Informatique IM2AG-UGA
 *  Parallel Algorithms and Programming - TP2:
 *  Borne Jonathan Isnel Maxime
 *
 *  Sujet et sources:
 *  F.Desprez (Inria), J-F. Méhaut (UGA/Polytech),
 *  T.Ropars (UGA/IM2AG), B. Videau (CEA).
 */

#include <stdio.h>
#include<omp.h>
#include <math.h>
#include <x86intrin.h>

#define NBEXPERIMENTS    10
static long long unsigned int experiments[NBEXPERIMENTS];
int nb_thread;
int blocksize;
int chunksize;
static unsigned int N = 4096;
typedef int *array_int;

/* the X array to be sorted */
static array_int X;

long long unsigned int average(long long unsigned int *exps)
{
    unsigned int i;
    long long unsigned int s = 0;

    for (i = 2; i < (NBEXPERIMENTS - 2); i++) {
        s = s + exps[i];
    }

    return s / (NBEXPERIMENTS - 2);
}

void init_array(array_int T)
{
    register int i;

    for (i = 0; i < N; i++) {
        T[i] = N - i;
    }
}

void print_array(array_int T)
{
    register int i;
    printf("{");
    for (i = 0; i < N; i++) {
        printf("%d, ", T[i]);
    }
    printf("}\n");
}

/*
  test if T is properly sorted
*/
int is_sorted(array_int T)
{
    register int i;

    for (i = 1; i < N; i++) {
        /* test designed specifically for our usecase */
        if (T[i - 1] + 1 != T[i])
            return 0;
    }
    return 1;
}

void sequential_bubble_sort(int *T, const int size)
{
    unsigned int swapped;
    unsigned int i;
    int tmp;

    do {
        swapped = 0;

        for (i = 0; i < size - 1; i++) {
            if (T[i] > T[i + 1]) {
                tmp = T[i];
                T[i] = T[i + 1];
                T[i + 1] = tmp;
                swapped = 1;
            }
        }
    } while (swapped);

    return;
}

/*
 * int bloc_bubble_pass(int *T, const int size)
 * Description:
 * Fonction utilitaire permettant d'effectuer une passe de bubble sort dans
 * un tableau T de taille size
 * Pré-conditions:
 *   - T est de taille 2^N avec N > 0
 *   - size = 2^N
 * Valeur de retour:
 *   Si l'état du tableau à été modifié la fonction retourne 1.
 */

int bloc_bubble_pass(int *T, const int size)
{
    unsigned int swapped;
    unsigned int i;
    int tmp;

    swapped = 0;

    for (i = 0; i < size - 1; i++) {
        if (T[i] > T[i + 1]) {
            tmp = T[i];
            T[i] = T[i + 1];
            T[i + 1] = tmp;
            swapped = 1;
        }
    }

    return swapped;
}

/*
 * void bloc_bubble_sort(int *T, const int size, const int blocsize)
 *
 * Description: Tri à bulles par blocs (version parallèle).
 * On considère un tableau d'entiers T de taille 2^N.
 * On divise T en sous blocs de taille s = 2^k avec k<N.
 * Pour chacun des (N-k) sous-blocs on fait une passe de bubble sort
 * dans un thread séparé.
 * A l'issue des passes parallèles on effectue une passe
 * de bubble-sort sur les cases adjacentes des différents sous-blocs.
 * On répète l'opération tant que l'état du tableau T à été modifié
 * lors d'une des étapes précédentes.
 *
 *           T[s]     T[2s]
 *  T[0] T[s-1]| T[2s-1]|        T[(N-k-1)s]  T[(N-k)*s-1]
 *    |      | |      | |      ...        |     |
 *   +v------v+v------v+v-------+--------+v-----v+
 * T |        |        |        |        |       |
 *   +--------+--------+--------+--------+-------+
 *
 *   +-------+ +-------+ +-------+ +-------+ +-------+
 *   |       | |       | |       | |       | |       |
 *   +-------+ +-------+ +-------+ +-------+ +-------+
 *    ~~~~~~~   ~~~~~~~   ~~~~~~~             ~~~~~~~
 *    thread0   thread1   thread2     ...    thread (N-k)-1
 *
 *   +--------+--------+--------+--------+-------+
 * T |        |        |        |        |       |
 *   +-------^+^------^+^------^+^------^+^------+
 *           | |      | |      | |      | |
             \ /      \ /      \ /      \ /
 *          bubble   bubble   bubble  bubble
 *
 * Spécifications: Prends en paramètres:
 *   - Un tableau d'entiers T
 *   - Un entier size correspondant à la taille du sous-tableau
 *     de T à trier (on prendra size = T.length pour trier T en entier).
 *   - Un entier blocsize définissant la taille des sous-blocs pour
 *     le tri parallèle.
 * Pré-conditions:
 *   - T est de taille 2^N avec N entier > 0
 *   - size vaut 2^N.
 *   - blocsize est de taille 2^k avec 0 < K < N.
 * Post-conditions:
 *   Le tableau T est trié.
 */
void bloc_bubble_sort_static(int *T, const int size, const int blocsize)
{
    register int i;
    int swapped;
    int tmp;
    int j, k, l;
    int q = size / blocsize;
    do {
        swapped = 0;
        /* Une passe de Bubble dans chaque sous bloc du tableau T */
#pragma omp parallel for schedule(static, chunksize) private(i) num_threads(nb_thread)
        for (i = 0; i < q; i++) {
            if(bloc_bubble_pass(T+(i * blocsize), blocsize) == 1) swapped = 1;
        }
        /* Bubble sur les cases adjacentes des blocs */
        for (j = 0; j < q; j++) {
            k = j * blocsize - 1;
            l = k + 1;
            if (T[k] > T[l]) {
                tmp = T[k];
                T[k] = T[l];
                T[l] = tmp;
                swapped = 1;
            }
        }
    } while (swapped);
    return;
}

void bloc_bubble_sort_dynamic(int *T, const int size, const int blocsize)
{
    register int i;
    int swapped;
    int tmp;
    int j, k, l;
    int q = size / blocsize;
    do {
        swapped = 0;
        /* Une passe de Bubble dans chaque sous bloc du tableau T */
#pragma omp parallel for schedule(dynamic, chunksize) private(i) num_threads(nb_thread)
        for (i = 0; i < q; i++) {
            if(bloc_bubble_pass(T+(i * blocsize), blocsize) == 1) swapped = 1;
        }
        /* Bubble sur les cases adjacentes des blocs */
        for (j = 0; j < q; j++) {
            k = j * blocsize - 1;
            l = k + 1;
            if (T[k] > T[l]) {
                tmp = T[k];
                T[k] = T[l];
                T[l] = tmp;
                swapped = 1;
            }
        }
    } while (swapped);
    return;
}

/* Principe:
   On effectue un bubble sort séparément sur les indices pairs et impairs du tableau.

   ex:
   taille tableau = 2^2
   indice: 0 1 2 3
   valeur: 4 3 2 1

   première étape:
   0 1 2 3
   4 2 3 1
   \ / \ /
   0 1 2 3
   2 4 1 3

   deuxième étape:
   0 1 2 3
   2 4 1 3
   \ /
   0 1 2 3
   2 1 4 3

   Grâce à cette méthode les boucles qui échangent les valeurs ne sont plus dépendantes
   de l'itération précédente donc on peut faire du parallélisme sur la boucle
   (Fonctionne mais à clarifier...)

*/

void parity_bubble_sort(int *T, const int size)
{
    unsigned int swapped;
    unsigned int i, j;
    int tmp;


    do {
        swapped = 0;


        for (i = 2; i < size; i += 2) {
            if (T[i] < T[i - 1]) {
                tmp = T[i];
                T[i] = T[i - 1];
                T[i - 1] = tmp;
                swapped = 1;
            }
        }


        for (i = 1; i < size; i += 2) {
            if (T[i] < T[i - 1]) {
                tmp = T[i];
                T[i] = T[i - 1];
                T[i - 1] = tmp;
                swapped = 1;
            }
        }

    } while (swapped);


    return;
}


int main(int argc, char **argv)
{
    unsigned long long int start, end, residu;
    unsigned long long int av;
    unsigned int exp;

    /* the program takes one parameter N which is the size of the array to
       be sorted. The array will have size 2^N */
    /*
    if (argc != 2) {
        fprintf(stderr, "bubble N \n");
        exit(-1);
        }*/

    /*N = 1 << (atoi(argv[1]));*/

    if (argc != 4) {
        printf("Entrée attendue: nombre threads; chunksize; runID\n");
        exit(1);
    }

    nb_thread = atoi(argv[1]);
    blocksize = atoi(argv[2]);
    int runID = atoi(argv[3]);

    /* -----------------------------------------------------------------------*/
    /* --- Calcul de la valeur du paramètre chunksize pour les directives --- */
    /* --- openMP:                                                        --- */
    /* --- Le nombre d'itérations dans le tri par bloc est                --- */
    /* --- taille_tableau/taille_bloc                                     --- */
    /* --- Si on a moins de threads disponibles que d'itérations           --- */
    /* --- on veut pouvoir assigner plusieurs itérations à un thread      --- */
    /* -----------------------------------------------------------------------*/
    int nb_iterations = N/blocksize;
    if (nb_thread < nb_iterations) {
        chunksize = nb_iterations / nb_thread;
    } else {
        chunksize = 1;
    }

    printf("chunksize: %d, nb_iterations: %d\n", chunksize, nb_iterations);

    X = (int *) malloc(N * sizeof(int));

    printf("--> Sorting an array of size %u\n", N);

    start = _rdtsc();
    end = _rdtsc();
    residu = end - start;

    /* ---------------------------- */
    /* --- Version séquentielle --- */
    /* ---------------------------- */

    for (exp = 0; exp < NBEXPERIMENTS; exp++) {
        init_array(X);

        start = _rdtsc();

        sequential_bubble_sort(X, N);

        end = _rdtsc();
        experiments[exp] = end - start;

        /* verifying that X is properly sorted */
        if (!is_sorted(X)) {
            fprintf(stderr, "ERROR: the array is not properly sorted\n");
            exit(-1);
        }
    }

    av = average(experiments);
    fprintf(stderr, "0;%d;%d;%d;%d;%d;%llu\n", nb_thread, chunksize, N, blocksize, runID,
            av - residu);
    printf("\n bubble serial \t\t\t %Ld cycles\n\n", av - residu);

    /* for (exp = 0; exp < NBEXPERIMENTS; exp++) { */
    /*     init_array(X); */
    /*     start = _rdtsc(); */

    /*     parity_bubble_sort(X, N); */

    /*     end = _rdtsc(); */
    /*     experiments[exp] = end - start; */

    /*     /\* verifying that X is properly sorted *\/ */
    /*     if (!is_sorted(X)) { */
    /*         fprintf(stderr, "ERROR: the array is not properly sorted\n"); */
    /*         exit(-1); */
    /*     } */
    /* } */

    /* av = average(experiments); */
    /* printf("\n parity bubble \t %Ld cycles\n\n", av - residu); */

    /* ------------------------------------------------ */
    /* --- Version par bloc: ordonancement statique --- */
    /* ------------------------------------------------ */

    for (exp = 0; exp < NBEXPERIMENTS; exp++) {
        init_array(X);
        start = _rdtsc();
        bloc_bubble_sort_static(X, N, blocksize);

        end = _rdtsc();
        experiments[exp] = end - start;

        /* verifying that X is properly sorted */
        if (!is_sorted(X)) {
            fprintf(stderr, "ERROR: the array is not properly sorted\n");
            exit(-1);
        }
    }

    av = average(experiments);
    fprintf(stderr, "1;%d;%d;%d;%d;%d;%llu\n", nb_thread, chunksize, nb_iterations, blocksize, runID,
            av - residu);
    printf("\n bloc bubble static \t %Ld cycles\n\n", av - residu);

    /* ------------------------------------------------- */
    /* --- Version par bloc: ordonancement dynamique --- */
    /* ------------------------------------------------- */

    for (exp = 0; exp < NBEXPERIMENTS; exp++) {
        init_array(X);
        start = _rdtsc();
        bloc_bubble_sort_dynamic(X, N, blocksize);

        end = _rdtsc();
        experiments[exp] = end - start;

        /* verifying that X is properly sorted */
        if (!is_sorted(X)) {
            fprintf(stderr, "ERROR: the array is not properly sorted\n");
            exit(-1);
        }
    }

    av = average(experiments);
    fprintf(stderr, "2;%d;%d;%d;%d;%d;%llu\n", nb_thread, chunksize, nb_iterations, blocksize, runID,
            av - residu);
    printf("\n bloc bubble dynamic \t %Ld cycles\n\n", av - residu);
}
