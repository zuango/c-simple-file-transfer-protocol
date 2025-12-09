/* protocol.h */
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

// Packet types
#define FILE_INFO   1
#define FILE_DATA   2
#define FILE_DONE   3
#define FILE_ACK    4
#define FILE_ERROR  5

// The fixed header sent before each payload

typedef struct {
  uint32_t type; 
  uint32_t size; 
  uint32_t seq; 
} packet_header_t;

#endif
