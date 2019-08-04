#!/bin/bash

#  M1 Informatique IM2AG-UGA
#  Parallel Algorithms and Programming - TP2:
#  Borne Jonathan Isnel Maxime

rm -f result_mergeSort.csv

echo "Algotype;Taille_Tableau;Nb_Thread;Threshold;Run_id;Time" > result_mergeSort.csv

# Algotype 0: mergeSort sequentiel
# Algotype 1: mergeSort parallele


for runID in {0..9}; do
	for taille_tableau in {13..18}; do # taille réelle tableau = 2^$taille_tableau
		echo TAILLE $taille_tableau
		for nb_threads in {1..18} ; do
            for threshold in 1 2 4 8 16 32 ; do
			    echo NB_THREAD $nb_threads
			    ./mergesort $taille_tableau $nb_threads $threshold $runID 2>> result_mergeSort.csv
            done
		done
	done
done
