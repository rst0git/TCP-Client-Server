CC=cc

all: server client

server:
	$(CC) -o server server.c

client:
	$(CC) -o client client.c
