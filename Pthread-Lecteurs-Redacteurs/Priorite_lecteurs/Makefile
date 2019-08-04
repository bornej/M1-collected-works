# M1 Informatique IM2AG-UGA
# Introduction to Operating Systems - TP5:
# Makefile
# Borne Jonathan

CC = gcc
INCLUDE = -I $(PWD)
CFLAGS = -Wall -Werror -g -pthread -O2 $(INCLUDE)
EXECUTABLES = test_lecteurs_redacteurs
OBJECTS = test_lecteurs_redacteurs.o lecteur_redacteur.o

all: $(EXECUTABLES)

# Regles pour les executables
test_lecteurs_redacteurs: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(CFLAGS) $(CPPFLAGS)

# Regles pour les fichiers objets
lecteur_redacteur.o : lecteur_redacteur.c lecteur_redacteur.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

# Regle générique pour la génération d'un fichier objet
%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(EXECUTABLES) $(TESTS) *.o *.dSYM
