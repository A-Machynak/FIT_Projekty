CC = gcc
CFLAGS = -g -O0 -std=c11 -pedantic -Wall -Wextra -fPIC
TARGETS = libhtab.a libhtab.so tail wordcount wordcount-dynamic

all: $(TARGETS)

tail: tail.o
	$(CC) $^ -o $@

tail.o: tail.c
	$(CC) -g -O0 -std=c11 -pedantic -Wall -Wextra -c $^ -o $@

wordcount.o: wordcount.c htab.h io.h

wordcount: wordcount.o io.o libhtab.a
	$(CC) -static $^ -o $@

# On Linux:
#	export LD_LIBRARY_PATH="."
wordcount-dynamic: wordcount.o io.o libhtab.so
	$(CC) $^ -o $@

libhtab.a: htab_bucket_count.o htab_clear.o htab_erase.o htab_find.o htab_for_each.o htab_free.o htab_hash_function.o htab_init.o htab_lookup_add.o htab_move.o htab_size.o
	ar rcs $@ $^

libhtab.so: htab_bucket_count.o htab_clear.o htab_erase.o htab_find.o htab_for_each.o htab_free.o htab_hash_function.o htab_init.o htab_lookup_add.o htab_move.o htab_size.o
	$(CC) -shared $^ -o $@


htab_bucket_count.o: htab_bucket_count.c htab.h htab_priv.h

htab_clear.o: htab_clear.c htab.h htab_priv.h

htab_erase.o: htab_erase.c htab.h

htab_find.o: htab_find.c htab.h htab_priv.h

htab_for_each.o: htab_for_each.c htab.h htab_priv.h

htab_free.o: htab_free.c htab.h htab_priv.h

htab_hash_function.o: htab_hash_function.c htab.h

htab_init.o: htab_init.c htab.h htab_priv.h

htab_lookup_add.o: htab_lookup_add.c htab.h htab_priv.h

htab_move.o: htab_move.c htab.h htab_priv.h

htab_size.o: htab_size.c htab.h htab_priv.h

io.o: io.c io.h


clean:
	rm -f *.o $(TARGETS) xmachy02.zip

zip: Makefile *.c *.h
	zip xmachy02.zip *.c *.h Makefile
