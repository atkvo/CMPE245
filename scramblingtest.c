#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "engine.h"

#define SYNC_BYTES 32
#define LENGTH 2
#define BITS 14
// #define LENGTH 14

// scramble and descrable this
// char payload[] =     { 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1 };
char payload[] = { 0b10101010, 0b00001100 };
// char original[] = { 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1 };
char original[] = { 0b10101010, 0b00001100 };

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
    scrambleBits(payloadTest, 15*8, 7);
    addPayload(&rxBuffer, payloadTest);

    int start = 0;
    int len = 0;
    start = scanForMatch(&sync, &rxBuffer, SYNC_BYTES, 5);
    len = extractPayload(&rxBuffer, extracted, start, 1024);
    printf("len: %d\n", len);
    // descrambleBits(extracted, 15*8, 7);
    descrambleBits(extracted, len*8, 7);
    printf("extracted: %s\n", extracted);

    int bitsPrinted = 0;
    for(int order = 3; order <= 13; order += 2) {
        printf("ORDER: %i\n", order);
        printf("----------------------\n");

        bitsPrinted = 0;
        printf("   ORIG ");
        for(int i = 0; i < LENGTH; i++) {
            for(int j = 7; bitsPrinted < BITS && j >= 0; j--) {
                printf("%d", payload[i] & (1 << j) ? 1 : 0);
                bitsPrinted++;
            }
        }
        printf("\n");

        bitsPrinted = 0;
        printf("  SCRAM ");
        scrambleBits(payload, 14, order);
        for(int i = 0; i < LENGTH; i++) {
            for(int j = 7; bitsPrinted < BITS && j >= 0; j--) {
                printf("%d", payload[i] & (1 << j) ? 1 : 0);
                bitsPrinted++;
            }
        }
        printf("\n");

        bitsPrinted = 0;
        printf(" DSCRAM ");
        descrambleBits(payload, 14, order);
        for(int i = 0; i < LENGTH; i++) {
            for(int j = 7; bitsPrinted < BITS && j >= 0; j--) {
                printf("%d", payload[i] & (1 << j) ? 1 : 0);
                bitsPrinted++;
            }
        }
        if(memcmp(payload, original, LENGTH) != 0) {
            printf(" -- DESCRAMBLE FAILED!");
        }
        printf("\n");
        printf("\n");
    }
    return 0;
}