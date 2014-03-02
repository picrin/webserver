#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

void report_error(char* message){
  printf("%s\n", message);
  exit(1);
}


int main(){
  int server_descriptor = socket(PF_INET, SOCK_STREAM, 0);
  if(server_descriptor == -1) report_error("client socket descriptor error");

  char* server_ip = "127.0.0.1"; // you can run nmap on 192.168.0.*, this should tell you all local computers conected to local network.

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;

  server_addr.sin_addr.s_addr = inet_addr(server_ip);
  
  server_addr.sin_port = htons(8080);

  int status_connection = connect(server_descriptor,
      (struct sockaddr*) &server_addr,
      sizeof(server_addr));
  if (status_connection == -1)
    report_error("client connection error");
  
  char* request = "czesc gosia";

  int status_write = write(server_descriptor, request, strlen(request));
  //printf("string length %lu\n", strlen(request));
  if(status_write == -1){
    report_error("status write error");
  }
  
}

