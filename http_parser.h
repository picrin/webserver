#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

//import data structure with a set of self-explanatory functions.
struct request{
  int request_ok;
  char* host_name;
  char* resource_name;
  int keep_alive;
};
struct request* malloc_request();
void free_request(struct request* free_request);
void print_request(const struct request* req);

//attempts to parse request, if successful sets request_ok to 0 and resolves host_name and resource_name to values demanded by the request. If not, sets request_ok to code error and quits.
void parse_request(struct request* request, char* request_str);

//checks if a host in the struct matches required gethostname() or gethostname().dcs.gla.ac.uk. If so returns 0, 1 otherwise.
int correct_host(struct request* request);

//checks if resource exists and returns 0 if it does, 1 otherwise.
int resource_exists(struct request* request);

struct response{
  int code;
  int content_length;
  char* content_type;
  int response_descriptor;
};

struct response* malloc_response(struct request*);
void free_response(struct response* free_response);
void print_response(const struct response* req);


#endif //HTTP_PARSER_H
