#  M1 Informatique IM2AG-UGA
#  Parallel Algorithms and Programming - TP5:
#  Borne Jonathan Isnel Maxime

# /!\ Attention:
# Les tests sont exécutés avec mpirun -n (NB_PROCESS). avec NB_PROCESS = 4
# Ainsi pour nos test, 4 processus sont exécutés sur un tore de 2*2 processeurs.
#
# Nous n'avons pas réussi à récupérer la valeur de l'argument -n dans nos programmes de test depuis argv.
# (Même en essayant de consulter argv avant MPI_init la valeur reçue est 0.)
#
# Dans nos Tests les dimensions du tore sont donc codées "en dur" par la variable int n (on a n = 2).
# Si vous décidez de changer le nombre de processus passé en argument il faudra adapter
# la taille du tore manuellement dans le code
# Pour pouvoir exécuter les test fournis il ne faut donc pas modifier ce paramètre.

CC=mpicc
INCLUDE= -I $(PWD)
CFLAGS= -Wall -Werror -O3 -std=c99 -g $(INCLUDE)
COMPILOPTS= $(CFLAGS)
TESTS=$(basename $(wildcard Tests/*.c))

RUN=mpirun
SRC=(wildcard *.c)

NB_PROCESS= 4

EXEC= $(TESTS)

all: $(EXEC)

tests: $(EXEC)
	for exec in $(EXEC);do $(RUN) -n $(NB_PROCESS) ./$$exec; done

# dépendences des binaires
$(EXEC): %: MPI_utils.o matrix_operations.o

clean:
	rm -f $(EXEC) *.o ./Results/LargeMatrixCannon_DataMgmt/* ./Results/LargeMatrixFox_DataMgmt/* \
	./Results/SmallMatrixCannon_DataMgmt/* ./Results/SmallMatrixFox_DataMgmt/* \
	./Results/LargeMatrixFox/* ./Results/LargeMatrixCannon/* ./Results/SmallMatrixFox/*
