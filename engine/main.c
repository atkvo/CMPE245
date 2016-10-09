#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "engine.h"

s_buff txBuffer;
s_buff rxBuffer;
s_buff syncBuff;

#define MIN_CONFIDENCE 2

int main() {
    s_buff corruptedSync;
    generateSync(&syncBuff, 32);

    addPayload(&rxBuffer, "G");
    generateSync(&rxBuffer, 32);
    addPayload(&rxBuffer, "HELLO THE WORLD");

    initBuffer(&corruptedSync);
    addPayload(&corruptedSync, "GBA");
    generateCorruptedSync(&corruptedSync, 32, 10);
    addPayload(&corruptedSync, "HELLO THE CORRUPTION");

    printf("\nClean packet: ");
    printPacketHex(&rxBuffer, DASH);
    printf("\n");
    printPacket(&rxBuffer);

    printf("\nMessy packet: ");
    printPacketHex(&corruptedSync, DASH);
    printf("\n");
    printPacket(&corruptedSync);
    printf("\n");

    char payload[1024];
    int payloadIndex = 0;
    payloadIndex = scanForMatch(&syncBuff, &rxBuffer, MIN_CONFIDENCE);
    if(payloadIndex < 0) { printf("Minimum confidence isn't met.\n"); }
    else {
        printf("Confidence met. Payload starts at: %i\n", payloadIndex);
        extractPayload(&rxBuffer, payload, payloadIndex, 1024);
        printf("\nPAYLOAD: \n\t%s\n", payload);
    }
    printf("\n\n");
    memset(payload, 0, 1024);
    payloadIndex = scanForMatch(&syncBuff, &corruptedSync, MIN_CONFIDENCE);
    if(payloadIndex < 0) { printf("Minimum confidence isn't met.\n"); }
    else {
        printf("Confidence met. Payload starts at: %i\n", payloadIndex);
        extractPayload(&corruptedSync, payload, payloadIndex, 1024);
        printf("\nPAYLOAD: \n\t%s\n", payload);
    }
    return 0;
}
