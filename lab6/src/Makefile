CC = gcc
CFLAGS = -I. -Wall -Wextra
LDFLAGS = -pthread

all: client server

client: client.c multmodulo.o
	$(CC) -o client client.c multmodulo.o $(CFLAGS) $(LDFLAGS)

server: server.c multmodulo.o
	$(CC) -o server server.c multmodulo.o $(CFLAGS) $(LDFLAGS)

multmodulo.o: multmodulo.c multmodulo.h
	$(CC) -c multmodulo.c -o multmodulo.o $(CFLAGS)

clean:
	rm -f client server multmodulo.o