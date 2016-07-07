#include "packetHandler.h"

#include <sys/socket.h>
#include <string.h>

static void (*handlers[256])(char *buffer, int socketDesc) = {0};

void registerHandler(void (*handler)(char *buffer, int socketDesc), int pos) {
    handlers[pos] = handler;
}

void handlePacket(int num, char *buffer, int socketDesc) {
    if (handlers[num]) {
        handlers[num](buffer, socketDesc);
    }
}

int sendPacket01(packet01_t packet, int socketDesc) {
    size_t len = 2;
    if (packet.result) {
        len = strlen(packet.n1) + strlen(packet.n2) + 4;
    }
    char buffer[len];
    buffer[0] = 1;
    buffer[1] = packet.result;
    if (packet.result) {
        strcpy(&buffer[2], packet.n1);
        strcpy(&buffer[3 + strlen(packet.n1)], packet.n2);
    }
    return send(socketDesc, buffer, len, 0);
}


