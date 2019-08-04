# M1 Informatique IM2AG-UGA
# Introduction to Operating Systems - TP8:
# Borne Jonathan

CC=gcc
INCLUDE= -I $(PWD)
CFLAGS= $(HOST32) -Wall -Werror -std=c99 -g $(INCLUDE)
CFLAGS+= -DDEBUG
COMPILOPTS= $(CFLAGS)
TESTS=$(basename $(wildcard Tests/*.c))
PROGRAMS= $(TESTS)

.PHONY: clean all

all: $(PROGRAMS)

# Tests
tests: $(TESTS)
	for file in $(TESTS);do ./$$file; done

# dépendences des binaires
$(PROGRAMS): %: stdes.o

# nettoyage
clean:
	rm -f *.o $(PROGRAMS) .*.deps
