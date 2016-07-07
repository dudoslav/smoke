#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include "primecrack.h"
#include "packetHandler.h"

#define BUFFER_SIZE 512

static void handlePacket01(char *buffer, int socketDesc) {
    printf("Packet data: pos %d, count %d\n", buffer[1], buffer[2]);
    packet01_t packet = {0};
    crkres_t crkres = {0};
    if (crack(&buffer[3], buffer[1], buffer[2], &crkres)) {
        printf("n1: %s n2: %s\n", crkres.n1, crkres.n2);
        packet.n1 = crkres.n1;
        packet.n2 = crkres.n2;
        packet.result = 1;
    } else {
        packet.result = 0;
        printf("Factors are not in this interval\n");
    }
    sendPacket01(packet, socketDesc);
    free(crkres.n1);
    free(crkres.n2);
}


int initSocket(int *socketDesc, char *ip, char *port) {
    struct addrinfo hints = {0};
    struct addrinfo *res = NULL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo(ip, port, &hints, &res);

    //Create socket
    *socketDesc = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (*socketDesc == -1) {
        fprintf(stderr, "Failed to create socket\n");
        freeaddrinfo(res);
        return 0;
    }
    printf("Created socket\n");

    //Connect to server
    if (connect(*socketDesc, res->ai_addr, res->ai_addrlen) == -1) {
        fprintf(stderr, "Failed to connect\n");
        freeaddrinfo(res);
        return 0;
    }
    printf("Connected\n");

    freeaddrinfo(res);
    return 1;
}

int main(int argc, char **argv) {
    int socketDesc = 0;
    char buffer[BUFFER_SIZE] = {0};
    int bytes = 0;
    int running = 1;

    if(argc != 3) {
        fprintf(stderr, "Usage: %s <server ip> <server port>\n", argv[0]);
        return 1;
    }

    if (!initSocket(&socketDesc, argv[1], argv[2])) {
        fprintf(stderr, "Failed in initializing socket\n");
        return 1;
    }

    //Register packet handlers here
    registerHandler(handlePacket01, 1);

    //Main loop
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
        handlePacket(buffer[0], buffer, socketDesc);
    }

    close(socketDesc);
    return 0;
}
