CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic -pthread
#CFLAGS += -g
TARGETS = proj2

all: $(TARGETS)

proj2.o: proj2.c

proj2: proj2.o actors.o
	$(CC) $(CFLAGS) $^ -o $@

actors.o: actors.c actors.h

clean:
	rm -f *.o $(TARGETS) proj2.zip

zip: Makefile *.c *.h
	zip proj2.zip *.c *.h Makefile