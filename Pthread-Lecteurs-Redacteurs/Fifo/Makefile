# M1 Informatique IM2AG-UGA
# Introduction to Operating Systems - TP5:
# Makefile
# Borne Jonathan

CC = gcc
INCLUDE = -I $(PWD)
CFLAGS = -Wall -Werror -g -pthread -O2 $(INCLUDE)
TESTS=$(basename $(wildcard Tests/*.c))
EXECUTABLES = test_lecteurs_redacteurs test_lecteurs_redacteurs_verbose
OBJECTS = test_lecteurs_redacteurs.o lecteur_redacteur.o file.o
OBJECTS_VERBOSE = test_lecteurs_redacteurs_verbose.o lecteur_redacteur_verbose.o file.o

all: $(EXECUTABLES)

# Tests pour la structure de file
tests: $(TESTS)
	for file in $(TESTS);do ./$$file; done
$(TESTS): file.o

# Regles pour les executables
test_lecteurs_redacteurs: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(CFLAGS) $(CPPFLAGS)

test_lecteurs_redacteurs_verbose: $(OBJECTS_VERBOSE)
	$(CC) -o $@ $(OBJECTS_VERBOSE) $(CFLAGS) $(CPPFLAGS)

# Regles pour les fichiers objets
lecteur_redacteur.o : lecteur_redacteur.c lecteur_redacteur.h file.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

lecteur_redacteur_verbose.o: lecteur_redacteur_verbose.c lecteur_redacteur.h file.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

file.o: file.c file.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

# Regle générique pour la génération d'un fichier objet
%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(EXECUTABLES) $(TESTS) *.o *.dSYM
