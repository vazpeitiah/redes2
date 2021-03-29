CC = /usr/bin/gcc

all: client server

client: client.c handleFile.c handleFile.h
	$(CC) client.c  handleFile.c -o $@ -lpthread

server: server.c handleFile.c handleFile.h
	$(CC) server.c handleFile.c -o $@ -lpthread

clean: 
	rm client server && rm -r dsk_client* dsk_server
	