#include <stdio.h>
#include <string.h>
#include "http_parser.h"
#include <stdlib.h>
int main(){
  struct request *example_request;
  char haystack[] = "GET / HTTP/1.1\r\nsdf\r\nHost: dziaba\r\n\r\n";
  char* hstck = haystack;
  example_request = malloc_request();
  parse_request(example_request, hstck);
  print_request(example_request);
  free_request(example_request);
  return 0;
}

/* strtok example 
#include <stdio.h>
#include <string.h>

int main ()
{
  char str[] ="- This, a sample string.";
  char * pch;
  printf ("Splitting string \"%s\" into tokens:\n",str);
  pch = strtok (str," ,.-");
  while (pch != NULL)
  {
    printf ("%s\n",pch);
    pch = strtok (NULL, " ,.-");
  }
  return 0;
}*/
