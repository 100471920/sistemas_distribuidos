CC = gcc
CFLAGS = -Wall -Werror
LDFLAGS = -L.
LIBS = -lclaves

all: cliente servidor libclaves.so

cliente: cliente.o libclaves.so
	$(CC) $(CFLAGS) -o $@ cliente.o $(LDFLAGS) $(LIBS)

servidor: servidor.o libclaves.so
	$(CC) $(CFLAGS) -o $@ servidor.o $(LDFLAGS) $(LIBS)

cliente.o: cliente.c
	$(CC) $(CFLAGS) -c -o $@ $<

servidor.o: servidor.c
	$(CC) $(CFLAGS) -c -o $@ $<

libclaves.so: claves.o
	$(CC) $(CFLAGS) -shared -o $@ $<

claves.o: claves.c
	$(CC) $(CFLAGS) -c -fPIC -o $@ $<

clean:
	rm -f cliente cliente.o servidor servidor.o libclaves.so claves.o
