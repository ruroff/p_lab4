# Makefile 

server: server.o 
	gcc -o server server.c -lrt -lpthread

client: client.o 
	gcc -o client client.c

all: client server
	
clean:
	rm -f *.o client server

clean-server:
	rm -f *.o server

clean-client:
	rm -f *.o client
