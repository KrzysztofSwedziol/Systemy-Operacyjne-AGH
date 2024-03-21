CC = gcc
CFLAGS = -std=c11 -Wall -O0
LDFLAGS = -lm

.PHONY: all static_lib shared_lib

all: static_lib shared_lib dynamic_lib client_static_lib client_shared_lib client_dynamic_lib

static_lib: collatz.c
	$(CC) $(CFLAGS) -c collatz.c -o collatz.o
	ar rcs libcollatz.a collatz.o

shared_lib: collatz.c
	$(CC) $(CFLAGS) -fPIC -shared -o libcollatz.so collatz.c $(LDFLAGS)

dynamic_lib: collatz.c
	$(CC) $(CFLAGS) -fPIC -shared -o libcollatz.so collatz.c $(LDFLAGS)

client_static_lib: client.c static_lib
	$(CC) $(CFLAGS) -c client.c -o client.o && $(CC) $(CFLAGS) -o client client.o -L. libcollatz.a

client_shared_lib: client.c shared_lib
	$(CC) $(CFLAGS) -c client.c -o client.o  && $(CC) $(CFLAGS) -o clientShare client.o -lcollatz -L. -Wl,-rpath=$(PWD)

client_dynamic_lib: client_dynamic.c dynamic_lib
	$(CC) $(CFLAGS) -o client_dynamic_lib client_dynamic.c -ldl -L.

clean:
	rm -f *.o *.a *.so client_static_lib client_shared_lib client_dynamic_lib