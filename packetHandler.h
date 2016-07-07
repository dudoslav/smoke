#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H

void registerHandler(void (*handler)(char *buffer, int socketDesc), int pos);
void handlePacket(int num, char *buffer, int socketDesc);

typedef struct {
    char result;
    char *n1;
    char *n2;
} packet01_t;

int sendPacket01(packet01_t packet, int socketDesc);

#endif
