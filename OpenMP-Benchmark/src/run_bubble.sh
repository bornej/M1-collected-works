#!/bin/bash

#  M1 Informatique IM2AG-UGA
#  Parallel Algorithms and Programming - TP2:
#  Borne Jonathan Isnel Maxime

echo "Algo_type;Nb_threads;Chunk_size;Nb_blocs;Bloc_size;Run_id;Time" > bubble_1_16.csv
for runID in {0..9}; do
	for blocksize in 8 16 32 64 128 256 512 1024 2048; do
		echo BLOCKSIZE $blocksize
		for nb_threads in {1..9} ; do
			echo NB_THREAD $nb_threads
			./bubble $nb_threads $blocksize $runID 2>> bubble_1_16.csv
		done
	done
done
