
PRJ=c204
#
C202PATH=../c202/
PROGS=$(PRJ)-test
CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic -lm -I$(C202PATH) -fcommon

all: $(PROGS)

$(PRJ)-test: $(PRJ).c $(PRJ)-test.c $(C202PATH)c202.c
	$(CC) $(CFLAGS) -o $@ $(PRJ).c $(PRJ)-test.c $(C202PATH)c202.c

clean:
	rm -f *.o *.output $(PROGS)
#