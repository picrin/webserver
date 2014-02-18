#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

struct request{
  int request_ok;
  char* host_name;
  char* resource_name;
}

struct response{
  int code;
  char* html;
}

//attempts to parse request, if successful sets request_ok to 0 and resolves host_name and resource_name demanded by the request. If not, sets request_ok to 1 and leaves other fields undefined.
void parse_request(struct request* request, char* request_str);

//checks if a host in the struct matches required gethostname() or gethostname().dcs.gla.ac.uk. If so returns 0, 1 otherwise.
int correct_host(struct request* request);

//checks if resource exists and returns 0 if it does, 1 otherwise.
int resource_exists(struct request* request);

#endif //HTTP_PARSER_H
