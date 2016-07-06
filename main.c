#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include "primecrack.h"

#define BUFFER_SIZE 512

int main(int argc, char **argv) {
    int socketDesc;
    struct addrinfo hints, *res;
    char buffer[BUFFER_SIZE] = {0};
    int bytes;
    int running = 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo(argv[1], argv[2], &hints, &res);

    socketDesc = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (socketDesc == -1) {
        fprintf(stderr, "Failed to create socket\n");
        return 1;
    }
    printf("Created socket\n");


    if (connect(socketDesc, res->ai_addr, res->ai_addrlen) == -1) {
        fprintf(stderr, "Failed to connect\n");
        return 2;
    }
    printf("Connected\n");

    /*len = strlen(buffer);
    bytes = send(socketDesc, buffer, len, 0);
    printf("Sending %d bytes\n", bytes);*/

    while (running) {
        bytes = recv(socketDesc, buffer, BUFFER_SIZE, 0);

        if (bytes == 0) {
            printf("Connection closed\n");
            break;
        }

        if (bytes < 0) {
            fprintf(stderr, "Error receiving data");
        }

        printf("Incoming packet %d\n", buffer[0]);
        switch (buffer[0]) {
            case 1:
                printf("Packet data: pos %d, count %d\n", buffer[1], buffer[2]);
                crack_result_t result = {0};
                if (crack(&buffer[3], 10, buffer[1], buffer[2], &result)) {
                    printf("n1: %s n2: %s\n", result.n1, result.n2);
                    size_t len = strlen(result.n1) + strlen(result.n2) + 4;
                    char *buffer = malloc(len);
                    buffer[0] = 1;
                    buffer[1] = 1;
                    strcpy(&buffer[2], result.n1);
                    strcpy(&buffer[3 + strlen(result.n1)], result.n2);
                    send(socketDesc, buffer, len, 0);
                    free(result.n1);
                    free(result.n2);
                } else {
                    size_t len = 2;
                    char *buffer = malloc(len);
                    buffer[0] = 1;
                    buffer[1] = 0;
                    send(socketDesc, buffer, len, 0);
                    printf("Factors are not in this interval\n");
                }
                break;
        }
    }

    freeaddrinfo(res);
    close(socketDesc);

    return 0;
}
