#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "../shared/net.h"
#include "handler.h"

#define CHUNK_SIZE 4096

// Helper: send ACK back to client
static void send_ack(int sock, uint32_t seq)
{
  packet_header_t ack;
  ack.type = htonl(FILE_ACK);
  ack.size = htonl(0);
  ack.seq  = htonl(seq);

  send_all(sock, &ack, sizeof(ack));
}

void handle_client(int client_sock)
{
  packet_header_t header;
  char filename[256];
  FILE *fp = NULL;

  while (1) {
    if(recv_all(client_sock, &header, sizeof(header)) < 0){
      perror("recv header");
      break;
    }

    // convert from network byte order
    uint32_t type = ntohl(header.type);
    uint32_t size = ntohl(header.size);
    uint32_t seq = ntohl(header.seq);

    if (type == FILE_INFO){
      if(size >= sizeof(filename)){
        fprintf(stderr, "Filename too large\n");
        break;
      }

      // Receive filename
      if(recv_all(client_sock, filename, size) < 0){
        perror("recv filename");
        break;
      }

      filename[size] = 0;

      fp = fopen(filename, "wb");
      if(!fp){
        perror("fopen");
        break;
      }
      printf("[SERVER] Receiving file: %s\n", filename);
    }

    else if (type == FILE_DATA){
      uint8_t buf[CHUNK_SIZE];

      if(size > CHUNK_SIZE){
        fprintf(stderr, "Chunk too large\n");
        break;
      }

      if(recv_all(client_sock, buf, size) < 0){
        perror("recv data");
        break;
      }

      fwrite(buf, 1, size, fp);

      // ACK the Chunk
      send_ack(client_sock, seq);
    }

    else if (type == FILE_DONE) {
      printf("[SERVER] Trasfer complete\n");
      break;
    }

    else {
      fprintf(stderr, "Unkown packet type: %u\n", type);
      break;
    }
  }

  if (fp)
    fclose(fp);

  close(client_sock);
}
 
