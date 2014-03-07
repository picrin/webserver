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

#include "http_parser.h"
#include <pthread.h>

const int thread_no = 20;
const int backlog = 20;

const char* DELIMITER = "\r\n\r\n";
const int DELIMITER_LENGTH = 4;

int server_descriptor;


void report_error(char* message){
  printf("%s\n", message);
  pthread_exit(0);
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
  printf("position_index: %lu\n", (unsigned long) i->position_index);
  printf("delimiter_counter: %d\n", i->delimiter_counter);
  printf("big_buffer_index: %lu\n", (unsigned long) i->big_buffer_index);
  printf("---- end indecies ----\n");
}

//sets indicies->position_index to the value just after the end of a delimiter or NULL if couldn't find one.
//DONE -- in the big loop of the accepting code. Add security check for big_buffer_index.
int find_delimiter(struct indecies* indecies, const char* buffer_begin, const char* buffer_end){
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

void dispatch_request(char* from, char* to, int accept_socket, int* keep_alive){
  char* req_str = (char*) malloc(sizeof(char) * (to - from + 1));
  char* char_i;
  int int_i;
  for(char_i = from, int_i = 0; char_i < to; char_i++, int_i++){
    *(req_str + int_i) = *(char_i);
  }
  *(req_str + int_i) = '\0';
  struct request* req = malloc_request();
  parse_request(req, req_str);
  struct response* resp = malloc_response(req);
  //free(req_str);
  write_request(resp, accept_socket);
  if(resp->html == 0){
    read_file(req->resource_name, accept_socket);
  } else{
    int magia = write(accept_socket, resp->html, strlen(resp->html));
  }
  *keep_alive = req->keep_alive;
  free(req_str);
  free_request(req);
  print_response(resp);
  free_response(resp); 
}

void forever_accept(){
  struct sockaddr_in connection_addrport;
  socklen_t len = sizeof(connection_addrport);
  int accept_status;
  while(1){
  accept_status = accept(server_descriptor,
      (struct sockaddr *) &connection_addrport,
      &len);
  if(accept_status == -1) report_error("socket accept error");
  
  const int recv_len = 1500;
  const int big_buffer_len = 10000;
  char recv_buffer[recv_len];// = (char *)malloc(sizeof(char) * recv_len);
  memset(recv_buffer, 0, sizeof(recv_buffer));
  
  // not to get confused with >0, 0 or -1, all of which have meaning.
  int message_length = -2;
  char received_string[big_buffer_len];
  //char dispatch_request[big_buffer_len];
  memset(received_string, 0, sizeof(received_string));

  struct indecies* indecies_state = malloc_indecies();
  indecies_state->big_buffer_index = (char *) received_string;
  indecies_state->position_index = (char *) recv_buffer;
  int has_delimiter = 0;
  char* end;
  int keep_alive = 1;
  while(message_length != 0 && keep_alive == 1){
    message_length = read(accept_status, recv_buffer, recv_len);
    //if (message_length == -1)report_error("read error");
    if (message_length + (indecies_state->big_buffer_index) >=
        (char *) received_string + big_buffer_len) report_error("GET request to"
        "o big, or too many GET requests in too short time. Support for up to 1"
        "0 kB/read");
    //print_indecies(indecies_state);
    
    //printf("recv_buffer + message_length: %lu\n", recv_buffer + message_length);
    //printf("ttuuuuuuuuuuuuuuuuuuuuuuuuuuuheasfglkeurhglksjfb%d", message_length);
    //dispatch request
    //zero the big buffer
    //do it recursively until finished 
    while((has_delimiter = find_delimiter(indecies_state,
        recv_buffer, recv_buffer + message_length)) != 0){

      end = indecies_state->big_buffer_index;
      
      indecies_state->big_buffer_index = (char*) received_string;
      dispatch_request((char*) received_string, end, accept_status, &keep_alive);
      //zero the big buffer; //could be done, but doesn't have to.  
    }
  }
  free_indecies(indecies_state);
  printf("closing\n");
  close(accept_status);
  }
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
  
  server_descriptor = socket(PF_INET, SOCK_STREAM, 0);
    
  int optval = 1;
  int opts_status;
  opts_status = setsockopt(server_descriptor, SOL_SOCKET, SO_REUSEADDR,
      &optval, sizeof(optval));
  if (opts_status == -1) report_error("couldn't set SO_REUSEADDR flag");

  if(server_descriptor == -1) report_error("socket descriptor error");
  
  struct sockaddr_in server_addrport;
  server_addrport.sin_addr.s_addr = inet_addr("127.0.0.1");
  server_addrport.sin_family = AF_INET;
  server_addrport.sin_port = htons(8080);
  
  int bind_status = bind(server_descriptor,
      (struct sockaddr *) &server_addrport,
      sizeof(struct sockaddr_in));
  if(bind_status == -1) report_error("socket couldn't be bound error");

  int listen_status;
  listen_status = listen(server_descriptor, backlog);
  if(listen_status == -1) report_error("socket can't listen");
  
  pthread_t ids[thread_no];
  //pthread_t id0;
  //pthread_t id1;
  int i;
  for(i = 0; i<thread_no; i++){
    pthread_create (
        &ids[i],
        NULL,
        (void*(*)())&forever_accept,
        NULL); 
  }
  //pthread_join(id1, NULL);


  forever_accept();
  
  close(server_descriptor);
  return 0;
}
