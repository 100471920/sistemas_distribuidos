CC = gcc
CFLAGS = -Wall -Werror -fPIC
LDFLAGS = -L. -lclaves -pthread

all: servidor cliente

servidor: servidor.o libclaves.so
	$(CC) $(CFLAGS) -o $@ servidor.o $(LDFLAGS)

cliente: cliente.o libclaves.so
	$(CC) $(CFLAGS) -o $@ cliente.o $(LDFLAGS) -Wl,-rpath,.

libclaves.so: claves.c
	$(CC) $(CFLAGS) -shared -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.so servidor cliente

