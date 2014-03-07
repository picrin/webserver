#These are recommended flags, and a recommended compiler.  
COMP=gcc
CFLAGS=-Wall -Wextra -O2

server: server.o http_parser.o
	$(COMP) $(CFLAGS) http_parser.o server.o -lpthread -o server

http_parser.o: http_parser.h http_parser.c
	$(COMP) $(CFLAGS) -c http_parser.c -o http_parser.o

server.o: simple_server.c
	$(COMP) $(CFLAGS) -c simple_server.c -o server.o

#client: simple_client.c
#	$(COMP) $(CFLAGS) simple_client.c -o client
