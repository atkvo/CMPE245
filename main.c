#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "engine.h"

s_buff txBuffer;
s_buff rxBuffer;
s_buff syncBuff;

#define MIN_CONFIDENCE 5
#define SYNC_BYTES     32

s_buff syncBuff;
int main() {
    s_buff corruptedSync;
    generateSync(&syncBuff, SYNC_BYTES);

    addPayload(&rxBuffer, "G4");
    generateSync(&rxBuffer, SYNC_BYTES);
    addPayload(&rxBuffer, "HELLO THE WORLD\n");

    initBuffer(&corruptedSync);
    addPayload(&corruptedSync, "Bkj9");
    generateCorruptedSync(&corruptedSync, SYNC_BYTES, 12);
    addPayload(&corruptedSync, "HELLO THE CORRUPTION\n");

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
    payloadIndex = scanForMatch(&syncBuff, &rxBuffer, SYNC_BYTES, MIN_CONFIDENCE);
    if(payloadIndex < 0) { printf("Minimum confidence isn't met.\n"); }
    else {
        printf("Confidence met. Payload starts at: %i\n", payloadIndex);
        extractPayload(&rxBuffer, payload, payloadIndex, 1024);
        printf("\nPAYLOAD: \n\t%s\n", payload);
    }
    printf("\n\n");
    memset(payload, 0, 1024);
    payloadIndex = scanForMatch(&syncBuff, &corruptedSync, SYNC_BYTES, MIN_CONFIDENCE);
    if(payloadIndex < 0) { printf("Minimum confidence isn't met.\n"); }
    else {
        printf("Confidence met. Payload starts at: %i\n", payloadIndex);
        extractPayload(&corruptedSync, payload, payloadIndex, 1024);
        printf("\nPAYLOAD: \n\t%s\n", payload);
    }
    int corruption = 5;
    int syncbytes = 32;
    int confidence = 8;
    char * userload = NULL;
    int read;
    unsigned long len;
    while(1) {
        clearBuffer(&txBuffer);
        memset(payload, 0, 1024);
        printf("\nEnter corruption percentage (>10)");
        printf("\nEnter number of sync bytes: ");
        generateCorruptedSync(&txBuffer, syncbytes, corruption);
        printf("\nEnter minimum confidence: ");
        printf("\nEnter payload: ");
        read = getline(&userload, &len, stdin);
        addPayload(&txBuffer, userload);
        int matchIndex = scanForMatch(&syncBuff, &txBuffer, 32, confidence);
        if(matchIndex > 0) {
            extractPayload(&txBuffer, payload, matchIndex, 1024);
            printf("\nPAYLOAD @ %i: %s", matchIndex, payload);
        }
        else {
            printf("\nCan't find payload");
        }
    }
    return 0;
}
