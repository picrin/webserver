#include <stdio.h>
#include <string.h>
//#include "http_parser.h"
#include <stdlib.h>
#include <sys/stat.h>

int file_size(char* filename){
  struct stat st;
  stat(filename, &st);
  return st.st_size;
}


int main(){
  //char* badzia = glue_strings(4, "huja", "muja", "dziaba", "struja");
  printf("%s", read_file("index.html"));
  return 0;  
}

