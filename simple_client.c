#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

int descriptor;
int status;
int status_write;
const char *google_ip;

const char *request;

char response[1024];
size_t response_size;


in_addr_t google_ip_int;
struct in_addr google_sockaddr; 
struct sockaddr_in their_socket;


int connection;


void report_error(char* message){
  printf("%s", message);
  exit(1);
}


int main(int arg_count, char** args){
  descriptor = socket(PF_INET, SOCK_STREAM, 0);
  if(descriptor == -1) report_error("client socket descriptor error");

  google_ip = "127.0.0.1";

  google_ip_int = inet_addr(google_ip);
  
  their_socket.sin_family = AF_INET;

  their_socket.sin_addr.s_addr = google_ip_int;
  
  their_socket.sin_port = htons(8080);

  request = "GET / HTTP/1.1\r\nUser-Agent: curl/7.29.0\r\nHost: www.google.co.uk\r\nAccept: */*\r\n\r\n";

  connection = connect(descriptor, (struct sockaddr*) &their_socket, sizeof(their_socket));
  
  printf("status connection %d\n", connection);

}

