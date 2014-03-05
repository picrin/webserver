#I prefer to use this set of flags and clang as compiler. 
COMP=clang
#CFLAGS=-std=c99 -Wall -Wextra -pedantic -Wfatal-errors -g
CFLAGS= -Wall -Wextra -Wfatal-errors -g

#These are reommended flags, and a recommended compiler.  
#COMP=gcc
#CFLAGS=-Wall -Wextra -O2

test: main.c http_parser.o
	$(COMP) $(CFLAGS) main.c http_parser.o -o test

http_parser.o: http_parser.h http_parser.c
	$(COMP) $(CFLAGS) -c http_parser.c -o http_parser.o

client: simple_client.c
	$(COMP) $(CFLAGS) simple_client.c -o client

server.o: simple_server.c
	$(COMP) $(CFLAGS) -c simple_server.c -o server.o

server: server.o http_parser.o
	$(COMP) $(CFLAGS) http_parser.o server.o -o server
