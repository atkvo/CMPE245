#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "engine.h"

#define SYNC_BYTES 32

// scramble and descrable this
char payload[] =  { 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1 };
char original[] = { 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1 };

char payloadTest[] = "HELLO THE WORLD\n";
char extracted[100];

s_buff rxBuffer;
s_buff sync;

int main() {
    initBuffer(&sync);
    initBuffer(&rxBuffer);
    generateSync(&sync, SYNC_BYTES);

    clearBuffer(&rxBuffer);
    generateSync(&rxBuffer, SYNC_BYTES);
    scramble(payloadTest, 15*8, 7);
    addPayload(&rxBuffer, payloadTest);

    int start = 0;
    start = scanForMatch(&sync, &rxBuffer, SYNC_BYTES, 5);
    extractPayload(&rxBuffer, extracted, start, 1024);
    descramble(extracted, 15*8, 7);
    printf("extracted: %s\n", extracted);

    for(int order = 3; order <= 13; order += 2) {
        printf("ORDER: %i\n", order);
        printf("----------------------\n");

        printf("   ORIG ");
        for(int i = 0; i < 14; i++) {
            printf("%d", payload[i]);
        }
        printf("\n");

        printf("  SCRAM ");
        scramble(payload, 14, order);
        for(int i = 0; i < 14; i++) {
            printf("%d", payload[i]);
        }
        printf("\n");

        printf(" DSCRAM ");
        descramble(payload, 14, order);
        for(int i = 0; i < 14; i++) {
            printf("%d", payload[i]);
        }
        if(memcmp(payload, original, 14) != 0) {
            printf(" -- DESCRAMBLE FAILED!");
        }
        printf("\n");
        printf("\n");
    }
    return 0;
}