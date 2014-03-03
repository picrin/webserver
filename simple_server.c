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

const char* DELIMITER = "\r\n\r\n";
const int DELIMITER_LENGTH = 4;

void report_error(char* message){
  printf("%s\n", message);
  exit(1);
}

struct indicies{
  char* position_index = 0;
  int delimiter_index = 0;
  char* big_buffer_index = 0;
}
//sets indicies->position_index to the value just after the end of a delimiter or NULL if couldn't find one.
//TODO add security check for big_buffer_index.
//TODO test this, it's a complicated piece of code.
void read_http_message(char* buffer_end, int delimiter_length, struct indicies* indecies){
  char* i;
  for(i = pd->position_index; i <= buffer_end; i++){
    *(indecies->big_buffer_index) = *i;
    (indecies->big_buffer_index)++;
    if (*i == *(DELIMITER + indecies->delimiter_index)){
      (indecies->delimiter_index)++;
      if (delimiter_index == DELIMITER_LENGTH){
        indecies->position_index = i + 1;
        indecies->delimiter_index = 0;
        return;
      }
    }
    else indecies->delimiter_index = 0;
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
  const int recv_len = 13; 
  int message_counter = 0;
  int message_length = -2; // not to get confused with >0, 0 or -1, all of which have meaning.
  char recv_buffer[recv_len];// = (char *)malloc(sizeof(char) * recv_len);
  char received_string[10000];
  const char* delimiter = "abcd";
  int delimiter_len = strlen(delimiter);
  int delim_char = 0;
  char *finished == 0;
  errno = 0;
  while(1)
  {
    int i;
    int k;
    message_length = read(accept_status, recv_buffer, recv_len);
    switch (message_length){
      case 0:
        printf("client closed socket\n");
        goto end_soc_read;
      case -1:
        report_error("read error");
        break;
      default:
        break;
    }
    
    for(i = 0; i <= message_length; i++){
    }
    for(k = 0; k < i - 1; k++){
      received_string[k + message_counter] = recv_buffer[k];
    }
    message_counter += (i - 1);
    //printf("message_counter %d\n", message_counter);
  }

  //finished = correct_delimiter(recv_buffer + i, delimiter_len, delimiter);

  received_string[message_counter + 1] = '\0';
  printf("heeeeeeeeeeeeeeeeeeeeej:\n%s\n", received_string);
  close(server_descriptor);
  close(accept_status);
}
