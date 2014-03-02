#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

const int backlog = 20;

void report_error(char* message){
  printf("%s\n", message);
  exit(1);
}

int correct_delimiter(char* buffer, int buffer_length, const char* delimiter){
  int i;
  for(i = 0; i < buffer_length; i++){
    if(*(buffer + i) != *(delimiter + i)) return 0;
  }
  return 1;
}

int main(){
  /*
    char* buffer = "\r\n\r\n";
    char* delimiter = "\r\n\r\n"; 
    printf("%d", correct_delimiter(buffer, 4, delimiter));
  */
  
  int server_descriptor = socket(PF_INET, SOCK_STREAM, 0);
    
  int optval = 1;
  int opts_status;
  opts_status = setsockopt(server_descriptor, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
  if (opts_status == -1) report_error("couldn't set SO_REUSEADDR flag");

  if(server_descriptor == -1) report_error("socket descriptor error");
  
  struct sockaddr_in server_addrport;
  server_addrport.sin_addr.s_addr = inet_addr("192.168.0.6");
  server_addrport.sin_family = AF_INET;
  server_addrport.sin_port = htons(8080);
  
  int bind_status = bind(server_descriptor, (struct sockaddr *) &server_addrport, sizeof(struct sockaddr_in));
  if(bind_status == -1) report_error("socket couldn't be bound error");

  int listen_status;
  listen_status = listen(server_descriptor, backlog);
  if(listen_status == -1) report_error("socket can't listen");

  struct sockaddr_in connection_addrport;
  socklen_t len = sizeof(connection_addrport);
  int accept_status;
  accept_status = accept(server_descriptor, (struct sockaddr *) &connection_addrport, &len);
  if(accept_status == -1) report_error("socket accept error");
  const int recv_len = 150; 
  int message_counter = 0;
  int message_length = 0;
  char* recv_buffer = (char *)malloc(sizeof(char) * recv_len);
  char received_string[10000];
  int finished = 0;
  const char* delimiter = "\r\n\r\n";
  int delimiter_len = strlen(delimiter);
  errno = 0;
  //message_length = read(accept_status, recv_buffer, recv_len);
  //printf("message_length: %d\n", message_length);
  //printf("sockerr_no %s: ", strerror(errno));
  //printf("message: %s\n", recv_buffer);
  while(/*!finished && message_length != -1*/ 1){
    //int i;
    //int k;
    //int inner_finished;
    message_length = read(accept_status, recv_buffer, recv_len);
    printf("message_length: %d\n", message_length);
    printf("message: %s\n", recv_buffer);
    /*for(i = 0; i < message_length - delimiter_len; i++){
      printf("recv_buffer %c\n", recv_buffer[i]);
      inner_finished = correct_delimiter(recv_buffer, delimiter_len, delimiter);
      if(inner_finished){
        finished = 1;
      }
    }
    for(k = 0; k < i; k++){
      received_string[k + message_counter] = recv_buffer[k];
    }
    message_counter += i;*/
  }
  received_string[message_counter + 1] = '\0';
  printf("%s\n", received_string);
  close(server_descriptor);
  close(accept_status);
}
