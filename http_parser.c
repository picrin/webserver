#include "http_parser.h"
#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int write_responce(const char* filename, int stream, ){
    const int bufsize = 5;
    char buffer[bufsize];
    //memset(buffer, 50, 10000);
    int i;

    FILE *fp = fopen("something.c", "rb");
 
    if (fp == NULL) {
        perror("there's no file");
        return 1;
    }
 
    /* be sure to never read more than 5 char */
    int rc = getc(fp);
    while(rc != EOF){
        
        if (rc == EOF) {
            break;
        }
        buffer[i] = rc;
    }
    buffer[i] = '\0';
    printf("%s", buffer);
    fclose(fp);
    return 0;
}

int resource_exists(struct request* request){
    FILE* fp = fopen(request -> resource_name, "rb");
    int returned = (int) fp;
    fclose(fp);
    return returned;    
}


char* parse_header(char* host, char** headers, unsigned int minimum_length, int size, struct request* parse_to){
  int i_host;
  char* from_request;
  for(i_host = 1; i_host < size - 1; i_host++){
    from_request = headers[i_host];
    //printf("string checked: %s\n", from_request);
    if(strcasestr(from_request, host) == from_request) break;
    //printf("i_host, %d, %d\n", i_host, i - 1);
  }
    
  if(i_host == (size - 1)){
    printf("7: parse error, no %s header\n", host);
    parse_to->request_ok = 7;
    return NULL;
  }
  
  if(strlen(headers[i_host]) < minimum_length){
    printf("8: parse error, %s header too short\n", host);
  }

  return strstr(headers[i_host], " ");
}

struct request* malloc_request(){
  struct request* return_this;
  return_this = (struct request*) malloc(sizeof(struct request));
  return_this->request_ok = -1;
  return_this->host_name = NULL;
  return_this->resource_name = NULL;
  return return_this;
}

void print_request(const struct request* req){
  printf("-------- BEGIN struct request --------\n");
  printf("request code: %d\n", req->request_ok);
  printf("kepp-alive: %d\n", req->keep_alive);
  printf("hostname: %s\n", req->host_name);
  printf("resource path: %s\n", req->resource_name);
  printf("-------- END struct request --------\n");
}


void free_request(struct request* free_request){
  free(free_request->host_name);
  free(free_request->resource_name);
  free(free_request);
}

void parse_request(struct request *parse_to, char *request_str){
  const int max_no_headers = 128;
  //const int max_length_header = 1024;
  //const char** stable;
  char* saveptr[max_no_headers];
  char* delimiter;
  
  int i = 0;
  delimiter = request_str;

  /*
   * this modifies the string passed to include '\0'
   * character right after every header.     
   */
  while(1){
    saveptr[i] = delimiter;
    delimiter = strstr(delimiter, "\r\n");
    i++;
    if(delimiter == NULL) break;
    *delimiter = '\0';
    delimiter += 2;
  }
  /*
   * 1st sanity check. Is the request long enough?
   * 0 -- get header, 1 -- hostname, 2 -- empty 
   * TODO better error reporting, i.e. stderr
   */
  if(i < 2){
    printf("1: parse error, short request\n");
    parse_to->request_ok = 1;
    return;
  }
  
  /*
   * 2nd sanity check. Was the last header empty?
   * i.e. two consecutive "\r\n"s in a row?
   */
  if(saveptr[i-1] - saveptr[i-2]  != 2){
    printf("2: parse error, no empty header in the end\n");
    parse_to->request_ok = 2;
    return;
  }
  
  /*
   * 3rd and 4th sanity check. Exactly two spaces in the first header. 
   * First header long enough.
   */
  char* char_i;
  char* resource_begin;
  char* resource_end;
  int space_counter = 0;
  char_i = saveptr[0];  
  while(*char_i != '\0'){
    if (*char_i == ' ' && space_counter == 0) resource_begin = char_i + 1;
    if (*char_i == ' ' && space_counter == 1) resource_end = char_i;
    if (*char_i == ' ') space_counter++;

    char_i++;
  }
  
  if(char_i - saveptr[0] < 14){
    printf("3: parse error, GET header too short\n");
    parse_to->request_ok = 3;
    return;
  }

  if(space_counter != 2){
    printf("4: parse error, "
        "GET header has a wrong number of spaces\n");
    parse_to->request_ok = 4;
    return;
  }

  char* get_str = "GET";
  int get_i;
  /*
   * 5th and 6th: Does the first header start with "GET" and ends with "HTTP/1.1"?
   */
  for(get_i = 0; get_i < 3; get_i++){
    if(*(get_str + get_i) != *(saveptr[0] + get_i)){
      printf("5: parse error, GET header has no GET!\n");
      parse_to->request_ok = 5;
      return; 
    }
  }
  
  char* http_ver = "HTTP/1.1";
  char* http_i = char_i - 8;
  while(http_i != char_i){
    if(*http_i != *http_ver){
      printf("6: parse error in GET header or unsuporrted HTTP version\n");
      parse_to->request_ok = 6;
      return;
    }
    http_i++;
    http_ver++;
  }
  
  char* host_begin = parse_header("HoSt: ", saveptr, 7, i, parse_to);
  char* connection = parse_header("connection: ", saveptr, 13, i, parse_to);
  if(connection != NULL && strcasestr(connection, "keep-alive") != NULL) parse_to->keep_alive = 1;
  else if(connection != NULL && strcasestr(connection, "close") != NULL) parse_to->keep_alive = 0;
  else parse_to->keep_alive = 0;

  if(host_begin == 0) return;
  //printf("magia\n");
  //for(int k = 1; k <= 20; k++){
  //  printf("%lu\n", *(host_begin + k));
  //}
  //printf("end_magia\n");
  //printf("hello");
  //done with checking, allocate new string for memory 
  char* resource_req = (char*) malloc(sizeof(char) * (resource_end - resource_begin));
  char* host_req = (char*) malloc(sizeof(char) * strlen(host_begin + 1));
  strcpy(host_req, host_begin);
  
  *resource_end = '\0';
  //printf("%lu, %lu\n", saveptr[0], resource_end - resource_begin);
  
  strcpy(host_req, host_begin + 1);
  strcpy(resource_req, resource_begin);
  
  parse_to->host_name = host_req;
  parse_to->resource_name = resource_req;
  parse_to->request_ok = 0;
  //printf("%s\n", host_req);
  //printf("%s\n", resource_req);
}

