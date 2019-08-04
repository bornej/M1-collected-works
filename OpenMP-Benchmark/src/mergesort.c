/*  M1 Informatique IM2AG-UGA
 *  Parallel Algorithms and Programming - TP2:
 *  Borne Jonathan Isnel Maxime
 *
 *  Sujet et sources:
 *  F.Desprez (Inria), J-F. Méhaut (UGA/Polytech),
 *  T.Ropars (UGA/IM2AG), B. Videau (CEA).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<omp.h>
#include <x86intrin.h>

#define NBEXPERIMENTS   10
static long long unsigned int experiments[NBEXPERIMENTS];
static unsigned int N;
static int nb_thread;
static int threshold;
typedef int *array_int;
static array_int X;

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

    for (i = 0; i < N; i++) {
        printf("%d ", T[i]);
    }
    printf("\n");
}

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

long long unsigned int average(long long unsigned int *exps)
{
    unsigned int i;
    long long unsigned int s = 0;

    for (i = 2; i < (NBEXPERIMENTS - 2); i++) {
        s = s + exps[i];
    }

    return s / (NBEXPERIMENTS - 2);
}

/* Fusion des deux parties du tableau */
void merge(int *T, unsigned int debut, unsigned int milieu,
           unsigned int fin)
{
    unsigned int i = debut, j = milieu + 1;
    int tmp;
    int size = fin - debut + 1;
    unsigned int indice;
    int *merged = malloc(size * sizeof(int));

    for (indice = 0; indice < size; indice++) {
        /*  si j dépasse */
        if (j > fin) {
            merged[indice] = T[i];
            i++;
            continue;
        }

        /* si i dépasse de sa partie */
        if (i > milieu) {
            merged[indice] = T[j];
            j++;
            continue;
        }

        if (T[i] < T[j]) {
            merged[indice] = T[i];
            i++;
        } else {
            merged[indice] = T[j];
            j++;
        }
    }

    // #pragma omp parallel for schedule(dynamic) private(i)
    for (i = debut; i <= fin; i++) {
        T[i] = merged[i - debut];
    }
    free(merged);
}

void merge_sort(int *T, unsigned int debut, unsigned int fin)
{
    if (debut < fin) {
        unsigned int milieu = (fin + debut) / 2;
        // printf("Debut: %u Milieu: %u Fin: %u\n", debut, milieu, fin);

        /* partie gauche */
        merge_sort(T, debut, milieu);

        /* partie droite */
        merge_sort(T, milieu + 1, fin);

        /* merge des deux parties du tableau */
        merge(T, debut, milieu, fin);
    }

    return;
}

void parallel_merge_sort(int *T, unsigned int debut, unsigned int fin)
{
    if (debut < fin) {
        /* si le tableau à trier est petit on le trie avec merge_sort */
        if ((fin - debut) <= threshold) {
            merge_sort(T, debut, fin);
        } else {
            unsigned int milieu = (fin + debut) / 2;

            /* partie gauche */
            #pragma omp task
            parallel_merge_sort(T, debut, milieu);

            /* partie droite */
            #pragma omp task
            parallel_merge_sort(T, milieu + 1, fin);

            /* merge des deux parties du tableau */
            #pragma omp taskwait
            merge(T, debut, milieu, fin);
        }
    }

    return;
}

void sort(int *T, unsigned int debut, unsigned int fin){
    #pragma omp parallel num_threads(nb_thread)
    #pragma omp master
    {
        parallel_merge_sort(T, debut, fin);
    }
}
int main(int argc, char **argv)
{
    unsigned long long int start, end, residu;
    unsigned long long int av;
    unsigned int exp;

    if (argc != 5) {
        fprintf(stderr, "mergesort N nb_thread threshold runID\n");
        exit(-1);
    }

    N = 1 << (atoi(argv[1]));
    nb_thread = atoi(argv[2]);
    threshold = atoi(argv[3]);
    int runID = atoi(argv[4]);
    X = (int *) malloc(N * sizeof(int));

    printf("--> Sorting an array of size %u\n", N);

    start = _rdtsc();
    end = _rdtsc();
    residu = end - start;

    // print_array (X) ;

    printf("sequential sorting ...\n");

    for (exp = 0; exp < NBEXPERIMENTS; exp++) {
        init_array(X);

        start = _rdtsc();

        merge_sort(X, 0, N - 1);

        end = _rdtsc();
        experiments[exp] = end - start;

        if (!is_sorted(X)) {
            fprintf(stdout, "ERROR: the array is not properly sorted\n");
            exit(-1);
        }
    }

    av = average(experiments);
    fprintf(stderr, "0;%d;%d;%d;%d;%llu\n", N, nb_thread, threshold, runID, av - residu);
    printf("\n merge sort serial\t\t %Ld cycles\n\n", av - residu);

    printf("parallel sorting ...\n");

    /* autorise le parallélisme imbriqué */
    //omp_set_nested(1);
    for (exp = 0; exp < NBEXPERIMENTS; exp++) {
        init_array(X);

        start = _rdtsc();

        sort(X, 0, N - 1);

        end = _rdtsc();
        experiments[exp] = end - start;

        if (!is_sorted(X)) {
            fprintf(stdout, "ERROR: the array is not properly sorted\n");
            exit(-1);
        }
    }

    av = average(experiments);
    fprintf(stderr, "1;%d;%d;%d;%d;%llu\n", N, nb_thread, threshold, runID, av - residu);
    printf("\n merge sort parallel with tasks\t %Ld cycles\n\n",
           av - residu);
}
