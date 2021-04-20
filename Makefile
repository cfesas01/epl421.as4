CC = gcc
CFLAGS = 
# CFLAGS = -Wall

all: server client

server:
	$(CC) $(CFLAGS) -pthread -o server server.c tpool.c

client:
	$(CC) $(CFLAGS) -o client client.c

clean:
	$(RM) client server