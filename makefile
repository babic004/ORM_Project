# To "make"       run in terminal: "make", "make client" or "make server"
# To "make clean" run in terminal: "make clean", "make client_clean" or "make server_clean"

all: client server

make clean: client_clean server_clean


client: client.c
	make -f makefile.client

client_clean:
	make clean -f makefile.client


server: server.c
	make -f makefile.server

server_clean:
	make clean -f makefile.server