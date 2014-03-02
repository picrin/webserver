#I prefer to use this set of flags and clang as compiler. 
COMP=clang
CFLAGS=-std=c99 -Wall -Wextra -pedantic -Wfatal-errors -g

#These are reommended flags, and a recommended compiler.  
#COMP=gcc
#CFLAGS=-Wall -Wextra -O2

main: main.o http_parser.o
	$(COMP) $(CFLAGS) main.o http_parser.o -o main

main.o: http_parser.h main.c
	$(COMP) $(CFLAGS) -c main.c -o main.o

http_parser.o: http_parser.h http_parser.c
	$(COMP) $(CFLAGS) -c http_parser.c -o http_parser.o

client: simple_client.c
	$(COMP) $(CFLAGS) simple_client.c -o client
server: simple_server.c
	$(COMP) $(CFLAGS) simple_server.c -o server
