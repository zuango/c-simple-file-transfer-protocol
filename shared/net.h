#ifndef NET_H
#define NET_H

#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>

int send_all(int sock, const void *buf, size_t len);
int recv_all(int sock, void *buf, size_t len);

#endif
