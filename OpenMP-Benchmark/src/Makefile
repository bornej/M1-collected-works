#  M1 Informatique IM2AG-UGA
#  Parallel Algorithms and Programming - TP2:
#  Borne Jonathan Isnel Maxime

CC=gcc
EXEC=triangular_matrix bubble mergesort
LDFLAGS=-fopenmp
CFLAGS=-O2 -fopenmp

all: $(EXEC)

triangular_matrix.o: triangular_matrix.c

triangular_matrix: triangular_matrix.o
	$(CC) $(LDFLAGS) -o $@ $^

bubble.o: bubble.c

bubble: bubble.o
	$(CC) $(LDFLAGS) -o $@ $^

mergesort.o: mergesort.c

mergesort: mergesort.o
	$(CC) $(LDFLAGS) -o $@ $^

%.o : %.c
	$(CC) -c $(CFLAGS) $^ 


clean:
	rm -f $(EXEC) *.o
