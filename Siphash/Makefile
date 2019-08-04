# M1 Informatique IM2AG-UGA
# Tp SipHash:
# Borne Jonathan - Duverney Thomas

CC=gcc
INCLUDE= -I $(PWD)
CFLAGS= -Wall -Werror -O3 -std=c99 -g $(INCLUDE)
COMPILOPTS= $(CFLAGS)
TESTS=$(basename $(wildcard Tests/*.c))
PROGRAMS= $(TESTS)

.PHONY: clean all

all: $(PROGRAMS)

# Tests
tests: $(TESTS)
	for file in $(TESTS);do ./$$file; done

# dépendences des binaires
$(PROGRAMS): %: sipHash.o

# nettoyage
clean:
	rm -f *.o $(PROGRAMS) .*.deps
