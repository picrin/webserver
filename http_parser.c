#include "http_parser.h"
#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdarg.h>

/*
struct request{
  int request_ok;
  char* host_name;
  char* resource_name;
  int keep_alive;
};

struct response{
  char* code;
  int content_length;
  char* content_type;
  int response_descriptor;
};
*/


char* stralloc(const char* string){
  char* malloced = (char*) malloc(sizeof(char) * (strlen(string) + 1));
  strcpy(malloced, string);
  return malloced;
}

char* glue_strings(int no, ...){
  va_list ap;
  int j;
  char* result;
  int total_len = 0;
  
  va_start(ap, no); /* Requires the last fixed parameter (to get the address) */
  for(j = 0; j < no; j++)
    total_len += strlen(va_arg(ap, char*)); /* Increments ap to the next argument. */
  va_end(ap);
  
  result = (char*) malloc(sizeof(char) * (total_len + 1));
  char* next_add = result;
  char* next_str;
  va_start(ap, no); /* Requires the last fixed parameter (to get the address) */
  for(j = 0; j < no; j++){
    next_str = va_arg(ap, char*);
    strcpy(next_add, next_str);
    next_add += strlen(next_str);
  } /* Increments ap to the next argument. */
  va_end(ap);
  return result; 
}

char* check_extension(char* string){
  char* last_dot = string;
  char* next_dot = last_dot;
  while((next_dot = strstr(next_dot + 1, ".")) != NULL){
    last_dot = next_dot;
  }
  last_dot = last_dot + 1;
  if(strcmp(last_dot, "html") == 0 || strcmp(last_dot, "htm") == 0) return stralloc("text/html");
  else if(strcmp(last_dot, "txt") == 0) return stralloc("text/plain");
  else if(strcmp(last_dot, "jpeg") == 0 || strcmp(last_dot, "jpg") == 0) return stralloc("text/jpeg");
  else if(strcmp(last_dot, "gif") == 0) return stralloc("image/gif");
  else if(strcmp(last_dot, "png") == 0) return stralloc("image/png");

  else return stralloc("application/octet-stream");
}

/*
404 Not FoundThe requested file cannot be found.
*/

char* request_template(char* message, char* long_message){
  char* left = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" 3\"http://www.w3.org/TR/html4/strict.dtd\">\n"
               "<html>\n"
               "<head>\n"
               "<title> ";
  char* right = " </title>\n</head>\n<body>\n<p> ";
  
  char* far_right =  " </p>\n</body>\n</html>";
  return glue_strings(5, left, message, right, long_message, far_right);
}

int file_size(char* filename){
  struct stat st;
  stat(filename, &st);
  return st.st_size;
}
/*
struct request{
  int request_ok;
  char* host_name;
  char* resource_name;
  int keep_alive;
};*/

/*
struct response{
  char* code;
  int content_length;
  int keep_alive;
  char* content_type;
  char* html;
};
*/
void free_response(struct response* resp){
  free(resp->content_type);
  //free(resp->resource_name);
  free(resp->html);
  free(resp->code);
  free(resp);
}



void read_file(char* name, int fd){
  //int size = file_size(name);
  //char* buffer = (char*) malloc(sizeof(char) * (size + 1));
    //memset(buffer, 50, 10000);
  //char* index = buffer;
  FILE *fp = fopen(name, "rb");
 
  int rc;
  while((rc = getc(fp)) != EOF){
    char hujamuja = rc;
    write(fd, &hujamuja, 1);
    //*index = (char) rc;
    //index++;
  }
  //*index = '\0';

  fclose(fp);
  //return buffer;
}

void print_response(const struct response* resp){
  printf("-------- BEGIN struct response --------\n");
  printf("code: %s\n", resp->code);
  printf("content_length: %d\n", resp->content_length);
  printf("keep_alive: %d\n", resp->keep_alive);
  printf("content_type: %s\n", resp->content_type);
  printf("html: %s\n", resp->html);
  printf("-------- END struct response --------\n");
}


struct response* malloc_response(struct request* req){
  struct response* resp = (struct response*) malloc(sizeof(struct response));
  resp->keep_alive = req->keep_alive;
  resp->content_length = 0;
  resp->content_type = stralloc("text/html");
  if(req->request_ok == -1){
    resp->code = stralloc("500 Internal Server Error");
    resp->html = request_template("500", "500 -- internal server error. "
    "There's nothing you can do about it, we have screwed up aka \"a team"
    "of highly trained monkeys has been sent to deal with this problem\"");
    resp->content_length = strlen(resp->html);

  }
  else if(req->request_ok == 404){
    resp->code = stralloc("404 Not Found");
    resp->html = request_template("404", "404 -- couldn't find the re"
    "source. Are you sure you typed the URL correctly?");
    resp->content_length = strlen(resp->html);

  }
  else if(req->request_ok != 0){
    resp->code = stralloc("400 Bad Request");
    resp->html = request_template("400", "server says: 400 It seems your browser screwed up.");
    resp->content_length = strlen(resp->html);
  }
  else{
    resp->code = stralloc("200 OK");
    resp -> content_length = file_size(req->resource_name);
    free(resp->content_type);
    resp -> content_type = check_extension(req->resource_name);
    resp -> html = NULL;
    //resp -> html = read_file(req->resource_name);
  }
  //free_request(req);
  return resp;
}
/*
HTTP/1.1 200 OK
Content-Type: text/html
Connection: close
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN"
"http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>

struct response{
  char* code;
  int content_length;
  int keep_alive;
  char* content_type;
  char* html;
};
*/

void wrt(int fd, char* str){
  if (str==NULL) return;
  printf("%s", str);
  int amount = write(fd, str, strlen(str));
  if (amount == -1) printf("error writing!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
}

void write_request(struct response* resp, int fd){
  wrt(fd, "HTTP/1.1 ");
  wrt(fd, resp->code);
  wrt(fd, "\r\n");
  
  wrt(fd, "Content-Type: ");
  wrt(fd, resp->content_type);
  wrt(fd, "\r\n");

  wrt(fd, "Connection: ");
  if(resp->keep_alive){
    wrt(fd, "keep-alive");
  } else {
    wrt(fd, "close");
  }
  wrt(fd, "\r\n");

  wrt(fd, "Content-Length: ");
  char huja[30];
  sprintf (huja,"%d", (resp->content_length));
  wrt(fd, huja);
  wrt(fd, "\r\n\r\n");
  
  //int status_write = write(server_descriptor, request, strlen(request));
}

char* resource_normalise(char* resource_string){
  char* malloced;
  if (strcmp(resource_string, "/") == 0){
    const char* defaul = "index.html";
    malloced = (char*) malloc(sizeof(char) * (strlen(defaul) + 1));
    strcpy(malloced, defaul);
    //printf("hello%s", malloced);

    free(resource_string);
    return malloced;
  }
  else if (strstr(resource_string, "/") == resource_string){
    int size_to_malloc = strlen(resource_string);
    malloced = (char*) malloc(sizeof(char) * size_to_malloc);
    //strcpy(malloced, (const char *) ".");
    strcpy(malloced, resource_string + 1);
    //strcpy(malloced, defaul);
    
    free(resource_string);
    return malloced;
  }
  else{
    return resource_string;
  }
}
/*
int correct_host(struct request* request){
  size_t large = 1000;
  char jebany[large];
  gethostname(jebany, large);
  
  if (strcmp(request->host_name, jebany) == 0) return 1;
  else if((strstr(request->host_name, jebany) == request->host_name) && strstr(request->host_name, ".dcs.gla.ac.uk") == request->host_name + strlen(jebany)) return 1;
  else return 0;
}*/

int correct_host(struct request* request){
  size_t large = 1000;
  char jebany[large];
  gethostname(jebany, large);
  if (strstr(request->host_name, jebany) == request->host_name) return 1;
  else return 0;
}

int resource_exists(struct request* request){
    FILE* fp = fopen(request -> resource_name, "rb");
    int returned;
    if (fp != NULL){
      returned = 1;
      fclose(fp);
    } else {
      returned = 0;
      request -> request_ok = 404;
    }
    
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
  printf("ffffffffffffffffffffffffff\n");
  printf("%s", request_str);
  printf("ffffffffffffffffffffffffff\n");

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
  
  char* host_begin = parse_header("host: ", saveptr, 7, i, parse_to);
  char* connection = parse_header("connection: ", saveptr, 13, i, parse_to);
  if(connection != NULL && strcasestr(connection, "keep-alive") != NULL) parse_to->keep_alive = 1;
  else if(connection != NULL && strcasestr(connection, "close") != NULL) parse_to->keep_alive = 0;
  else parse_to->keep_alive = 0;

  if(host_begin == NULL) return;
  //printf("magia\n");
  //for(int k = 1; k <= 20; k++){
  //  printf("%lu\n", *(host_begin + k));
  //}
  //printf("end_magia\n");
  //printf("hello");
  //done with checking, allocate new string for memory 
  char* resource_req = (char*) malloc(sizeof(char) * (resource_end - resource_begin) + 1);

  char* host_req = (char*) malloc(sizeof(char) * strlen(host_begin) + 1);
  strcpy(host_req, host_begin);
  
  *resource_end = '\0';
  //printf("%lu, %lu\n", saveptr[0], resource_end - resource_begin);
  
  strcpy(host_req, host_begin + 1);
  strcpy(resource_req, resource_begin);

  resource_req = resource_normalise(resource_req);
  
  parse_to->resource_name = resource_req;
  parse_to->host_name = host_req;
  
  if(!resource_exists(parse_to)) return;
  if(!correct_host(parse_to)){
    parse_to -> request_ok = 9;
    printf("parse error wrong hostname");
    return;
  }
  
  parse_to->request_ok = 0;
  //printf("%s\n", host_req);
  //printf("%s\n", resource_req);
}

