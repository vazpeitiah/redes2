CC = /usr/bin/gcc

all: client server

client: chat_client.c 
	$(CC) chat_client.c  -o $@ -lpthread

server: chat_server.c
	$(CC) chat_server.c  -o $@ -lpthread

clean: 
	rm chat_client chat_server *.txt
