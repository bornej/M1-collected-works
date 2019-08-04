CC=gcc

# uncomment to compile in 32bits mode (require gcc-*-multilib packages
# on Debian/Ubuntu)
#HOST32= -m32

INCLUDE= -I $(PWD)
CFLAGS= $(HOST32) -Wall -Werror -std=c99 -g -DMEMORY_SIZE=128000 $(INCLUDE)
CFLAGS+= -DDEBUG
LDFLAGS= $(HOST32)
TESTS=$(basename $(wildcard Tests/*.c))
PROGRAMS=memshell $(TESTS)

.PHONY: clean all test_ls

all: $(PROGRAMS)

# Tests
tests: $(TESTS)
	for file in $(TESTS);do ./$$file; done

# dépendences des binaires
$(PROGRAMS): %: mem.o common.o

# dépendances des fichiers objects
$(patsubst %.c,%.o,$(wildcard *.c)): %.o: .%.deps

.%.deps: %.c
	$(CC) $(CPPFLAGS) -MM $< | sed -e 's/\(.*\).o: /.\1.deps \1.o: /' > $@

-include $(wildcard .*.deps)

# seconde partie du sujet
libmalloc.so: malloc_stub.o mem.o
	$(CC) -shared -Wl,$@ $^ -o $@

test_ls: libmalloc.so
	LD_PRELOAD=./libmalloc.so ls

# nettoyage
clean:
	rm -f *.o $(PROGRAMS) libmalloc.so .*.deps
