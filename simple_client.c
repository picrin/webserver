
void report_error(char* message){
  printf("%s", message);
  exit(1);
}

int main(){
  int value = 0;

  int server_descriptor = socket(PF_INET, SOCK_STREAM, 0);
  if(server_descriptor == -1) report_error("socket descriptor error");
  
  struct sockaddr_in server_addrport;
  server_addrport.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addrport.sin_family = AF_INET;
  server_addrport.sin_port = htons(8080);
  
  int bind_status = bind(server_descriptor, (struct sockaddr *) &server_addrport, sizeof(struct sockaddr_in));
  if(bind_status == -1) report_error("socket couldn't be bound error");

  int listen_status;
  listen(server_descriptor, backlog);
  if(listen_status == -1) report_error("socket can't listen");

  struct sockaddr_in connection_addrport;
  socklen_t len = sizeof(connection_addrport);
  int accept_status;
  accept(server_descriptor, (struct sockaddr *) &connection_addrport, &len);
  if(accept_status == -1) report_error("socket accept error");
  
}
