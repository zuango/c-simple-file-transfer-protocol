#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "../shared/net.h"
#include "../shared/protocol.h"
#include "sendfile.h"

#define CHUNK_SIZE 4096

int send_file(int sock, const char *path) 
{
  FILE *fp = fopen(path, "rb");
  if(!fp) {
    perror("fopen");
    return -1;
  }

  // Extract filename (strip directory)
  const char *filename = strrchr(path, '/');
  filename = filename ? filename + 1 : path;

  size_t name_len = strlen(filename);

  /* 1) Send FILE_INFO header */

  packet_header_t info;
  info.type = htonl(FILE_INFO);
  info.size = htonl(name_len);
  info.seq  = htonl(0);

  if(send_all(sock, &info, sizeof(info)) < 0) {
    perror("send FILE_INFO header");
    fclose(fp);
    return -1;
  }

  if(send_all(sock, filename, name_len) < 0) {
    perror("send filename");
    fclose(fp);
    return -1;
  }

  /* 2) Send file Chunks */
  uint8_t buf[CHUNK_SIZE];
  size_t n;
  uint32_t seq = 1;
  
  while((n = fread(buf, 1, CHUNK_SIZE, fp)) > 0) {
    packet_header_t h;
    h.type = htonl(FILE_DATA);
    h.size = htonl(n);
    h.seq  = htonl(seq);

    if(send_all(sock, &h, sizeof(h)) < 0) {
      perror("send chunk header");
      fclose(fp);
      return -1;
    }

    if(send_all(sock, buf, n) < 0) {
      perror("send chunk");
      fclose(fp);
      return -1;
    }

    /* Wait for ACK */
    packet_header_t ack;
    if(recv_all(sock, &ack, sizeof(ack)) < 0 ) {
      perror("recv ack");
      fclose(fp);
      return -1;
    }

    seq++;
  }

  /* 3) send FILE_DONE */
  packet_header_t done;
  done.type = htonl(FILE_DONE);
  done.size = htonl(0);
  done.size = htonl(seq);

  if (send_all(sock, &done, sizeof(done)) < 0){
    perror("send FILE_DONE");
    fclose(fp);
    return -1;
  }

  printf("[CLIENT] File sent successfully.\n");

  fclose(fp);
  return 0;
}
