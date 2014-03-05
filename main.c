#include <stdio.h>
#include <string.h>
#include "http_parser.h"
#include <stdlib.h>
#include <sys/stat.h>

int main(){
  struct stat st;
  stat("index.html", &st);
  int size = st.st_size;
  printf("%d", size);
  return 0;  
}

