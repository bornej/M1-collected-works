#!/bin/bash

#  M1 Informatique IM2AG-UGA
#  Parallel Algorithms and Programming - TP2:
#  Borne Jonathan Isnel Maxime

RESULT_FILE_NAME= result_1_16.csv
rm -f $(RESULT_FILE_NAME)

echo "Algotype;Nb_Thread;Chunksize;Run_id;Time" > $(RESULT_FILE_NAME)

for runID in {0..9}; do
	for chunksize in 1 2 4 10 20 50 100 500; do
		echo CHUNKSIZE $chunksize
		for nb_threads in {1..16} ; do
			echo NB_THREAD $nb_threads
			./triangular_matrix $nb_threads $chunksize $runID 2>> $(RESULT_FILE_NAME)
		done
	done
done
