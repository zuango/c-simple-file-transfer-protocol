#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "handler.h"

#define BACKLOG 10

static int create_server(int port)
{
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock < 0){
    perror("socket");
    exit(1);
  }

  // allow reuse of port/address
  int opt = 1;
  if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("setsockopt");
    close(sock);
    exit(1);
  }

  struct sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;

  if(bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    perror("bind");
    close(sock);
    exit(1);
  }

  if(listen(sock, BACKLOG) < 0) {
    perror("listen");
    close(sock);
    exit(1);
  }

  return sock;
}

int main(int argc, char **argv) 
{
  int port = 9000;

  if(argc == 2){
    port = atoi(argv[1]);
  }

  printf("[SERVER] Starting server on port %d...\n", port);

  // prevent zombie children
  signal(SIGCHLD, SIG_IGN);

  int server_sock = create_server(port);

  while (1) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);
    if (client_sock < 0) {
      perror("accept");
      continue;
    }

    // print connection
    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, ipstr, sizeof(ipstr));
    printf("[SERVER] Connection from %s:%d\n", ipstr, ntohs(client_addr.sin_port));
    
    pid_t pid = fork();

    if(pid < 0) {
      perror("fork");
      close(client_sock);
      continue;
    }

    if(pid == 0) {
      // child process
      close(server_sock); //child dsnt need the listener
      handle_client(client_sock);
      exit(0);
    }

    // Parent process
    close(client_sock); // parent dsnt need the client sock
  }

  close(server_sock);
  return 0;
} 
