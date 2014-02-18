#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

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

int main(int arg_count, char** args){
  descriptor = socket(AF_INET, SOCK_STREAM, 0);
  google_ip = "173.194.41.87";

  google_ip_int = inet_addr(google_ip);
  
  their_socket.sin_family = AF_INET;

  their_socket.sin_addr.s_addr = google_ip_int;
  
  their_socket.sin_port = htons(80);

  request = "GET / HTTP/1.1\r\nUser-Agent: curl/7.29.0\r\nHost: www.google.co.uk\r\nAccept: */*\r\n\r\n";

  connection = connect(descriptor, (struct sockaddr*) &their_socket, sizeof(their_socket));
  
  printf("status connection %d\n", connection);
  
  status_write = write(descriptor, request, strlen(request));
  //printf("string length %lu\n", strlen(request));
  
  printf("status write %d\n", status_write);
  
  response_size = read(descriptor, response, 1024);
  printf("status read %lu\n");
  
  printf("-------- response --------\n");
  printf("%s\n", response);
  printf("-------- end response --------\n");

  status = close(descriptor);

  //printf("%d\n", descriptor);
  printf("status descriptor %d\n", status);
  return 0;
}
