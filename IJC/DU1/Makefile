CC = gcc
CFLAGS = -g -O0 -std=c11 -pedantic -Wall -Wextra
LDLIBS = -lm
TARGETS = primes primes-i steg-decode
SHELL = /bin/bash

#CFLAGS += -fsanitize=address
#LDFLAGS += -fsanitize=address

all: $(TARGETS)

%-i.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(LDLIBS) -DUSE_INLINE

primes-i: primes-i.o error.o erastothenes.o bitset-i.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)

primes: primes.o error.o erastothenes.o

steg-decode: steg-decode.o error.o ppm.o erastothenes.o

run: primes primes-i
	ulimit -s 65535 && time ./primes
	ulimit -s 65535 && time ./primes-i

clean:
	rm -f *.o primes primes-i steg-decode xmachy02.zip

zip: Makefile *.c *.h
	zip xmachy02.zip *.c *.h Makefile
