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

const char* DELIMITER = "1234";
const int DELIMITER_LENGTH = 4;

void report_error(char* message){
  printf("%s\n", message);
  exit(1);
}

struct indecies{
  char* position_index;
  int delimiter_counter;
  char* big_buffer_index;
};

struct indecies* malloc_indecies(){
  return (struct indecies*) calloc(sizeof(struct indecies), 1);
}

void free_indecies(struct indecies* i){
  free(i);
}

void print_indecies(struct indecies* i){
  printf("---- begin indecies ----\n");
  printf("position_index: %lu\n", i->position_index);
  printf("delimiter_counter: %d\n", i->delimiter_counter);
  printf("big_buffer_index: %lu\n", i->big_buffer_index);
  printf("---- end indecies ----\n");
}

//sets indicies->position_index to the value just after the end of a delimiter or NULL if couldn't find one.
//TODO add security check for big_buffer_index.
//TODO test this, it's a complicated piece of code. //TODO-ING 
int read_http_message(struct indecies* indecies, const char* buffer_begin, const char* buffer_end){
  char* i;
  for(i = indecies->position_index; i < buffer_end; i++){
    *(indecies->big_buffer_index) = *i;
    (indecies->big_buffer_index)++;
    if (*i == *(DELIMITER + indecies->delimiter_counter)){
      (indecies->delimiter_counter)++;
      if (indecies->delimiter_counter == DELIMITER_LENGTH){
        indecies->position_index = i + 1;
        indecies->delimiter_counter = 0;
        return 1;
      }
    }
    else indecies->delimiter_counter = 0;
  }
  indecies->position_index = (char*) buffer_begin;
  return 0;
}


int main(){
  /*
  //test1
  const char bigbuffer[10000];
  struct indecies* i = malloc_indecies();
  char* buffer_beg = "babajagapatrzy1234rotwailery";
  char* buffer_end = buffer_beg + strlen(buffer_beg);
  i->position_index = buffer_beg;
  i->big_buffer_index = (char*) bigbuffer;
  printf("TEST 1 ---------------------------------------------\n");
  print_indecies(i);
  read_http_message(i, buffer_end);
  print_indecies(i);
  //test2
  i = malloc_indecies();
  buffer_beg = "babajagapatrzyrotwailery12";
  buffer_end = buffer_beg + strlen(buffer_beg);
  i->position_index = buffer_beg;
  i->big_buffer_index = (char*) bigbuffer;
  printf("TEST 2 ---------------------------------------------\n");
  print_indecies(i);
  read_http_message(i, buffer_end);
  print_indecies(i);

  //test3
  i = malloc_indecies();
  buffer_beg = "babajagapatrzy1234rotwailery" + 10;
  buffer_end = buffer_beg + strlen(buffer_beg);
  i->position_index = buffer_beg;
  i->big_buffer_index = (char*) bigbuffer;
  printf("TEST 3 ---------------------------------------------\n");
  print_indecies(i);
  read_http_message(i, buffer_end);
  print_indecies(i);
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
  const int big_buffer_len = 10000;
  char recv_buffer[recv_len];// = (char *)malloc(sizeof(char) * recv_len);
  memset(recv_buffer, 0, sizeof(recv_buffer));
  //recv_buffer[len] = '\0';
  int message_length = -2; // not to get confused with >0, 0 or -1, all of which have meaning.
  char received_string[big_buffer_len];
  memset(received_string, 0, sizeof(received_string));

  struct indecies* indecies_state = malloc_indecies();
  indecies_state->big_buffer_index = (char *) received_string;
  indecies_state->position_index = (char *) recv_buffer;
  int has_delimiter = 0;
  while(message_length != 0){
    message_length = read(accept_status, recv_buffer, recv_len);
    if (message_length == -1) report_error("read error");
    print_indecies(indecies_state);
    
    //printf("recv_buffer + message_length: %lu\n", recv_buffer + message_length);
    has_delimiter = read_http_message(indecies_state, recv_buffer, recv_buffer + message_length);
    while(has_delimiter != 0){
      has_delimiter = read_http_message(indecies_state, recv_buffer, recv_buffer + message_length);
    }
    //printf("string: %s, has delimiter: %d\n", recv_buffer, has_delimiter);
    //print_indecies(indecies_state);

  }

  //finished = correct_delimiter(recv_buffer + i, delimiter_len, delimiter);
  *(indecies_state->big_buffer_index + 1) = '\0';
  printf("recovered string:\n%s\n", received_string);
  free_indecies(indecies_state);
  close(server_descriptor);
  close(accept_status);
  
}
