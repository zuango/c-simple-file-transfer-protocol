#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "../shared/net.h"
#include "sendfile.h"

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <ip> <port> <file>\n", argv[0]);
        return 1;
    }

    const char *ip   = argv[1];
    int port         = atoi(argv[2]);
    const char *path = argv[3];

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);

    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        return 1;
    }

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return 1;
    }

    printf("[CLIENT] Connected to %s:%d\n", ip, port);

    if (send_file(sock, path) < 0) {
        fprintf(stderr, "[CLIENT] Failed to send file.\n");
        close(sock);
        return 1;
    }

    close(sock);
    return 0;
}

