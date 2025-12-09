#include "net.h"


int send_all(int sock, const void *buf, size_t len)
{
  size_t sent = 0;
  const uint8_t *p = buf;

  while(sent < len){
    ssize_t n = send(sock, p + sent, len - sent, 0);
    if(n <= 0){
      return -1;
    }
    sent += n;
  }
  return 0;
}

int recv_all(int sock, void *buf, size_t len)
{
  size_t recvd = 0;
  uint8_t *p = buf;

  while(recvd < len) {
    ssize_t n = recv(sock, p + recvd, len - recvd, 0);
    if(n <= 0 )
      return -1;
    recvd += n;
  }
  return 0;
}

