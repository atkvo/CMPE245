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

// char payloadTest[] = "HELLO THE WORLD\n";
// char payloadTest[] = "ANDREW5295";
char payloadTest[] = "JAJAMES6912";
// char payloadTest[] = "AJANDREW5295";
// char payloadTest[] = { 0b01000010, 0b01001110 };
// char payloadTest[] = { 0b10101010, 0b00001100 };
char extracted[100];

s_buff rxBuffer;
s_buff sync;

int main() {
    /*
    for(int i = 0; i < strlen(payloadTest); i++) {
        for(int j = 7; j >= 0; j--) {
            printf("%d", ((payloadTest[i] >> j) & 0x01 )? 1 : 0);
        }
        printf(" ");
    }
    printf("\n\n");
    initBuffer(&sync);
    initBuffer(&rxBuffer);
    generateSync(&sync, SYNC_BYTES);

    clearBuffer(&rxBuffer);
    generateSync(&rxBuffer, SYNC_BYTES);
    // scrambleBits(payloadTest, strlen(payloadTest)*8, 3);
    scrambleBits(payloadTest, 11, 7);
    // scrambleElements(payloadTest, strlen(payloadTest), 7);
    // printf("%s\n", payloadTest);
    printf("SCRAMBLED: \n");
    for(int i = 0; i < 11; i++) {
        for(int j = 7; j >= 0; j--) {
            printf("%d", ((payloadTest[i] >> j) & 0x01 )? 1 : 0);
        }
        printf(" ");
    }
    printf("\nDONE\n");
    descrambleBits(payloadTest, 11, 7);
    printf("DESCRAMBLED: \n");
    for(int i = 0; i < 11; i++) {
        for(int j = 7; j >= 0; j--) {
            printf("%d", ((payloadTest[i] >> j) & 0x01 )? 1 : 0);
        }
        printf(" ");
    }
    printf("\nDONE\n");

    addPayload(&rxBuffer, payloadTest);

*/
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
            printf(" ");
        }
        printf("\n");

        bitsPrinted = 0;
        printf("  SCRAM ");
        scrambleBits(payload, 2, order);
        for(int i = 0; i < LENGTH; i++) {
            for(int j = 7; bitsPrinted < BITS && j >= 0; j--) {
                printf("%d", payload[i] & (1 << j) ? 1 : 0);
                bitsPrinted++;
            }
            printf(" ");
        }
        printf("\n");

        bitsPrinted = 0;
        printf(" DSCRAM ");
        descrambleBits(payload, 2, order);
        for(int i = 0; i < LENGTH; i++) {
            for(int j = 7; bitsPrinted < BITS && j >= 0; j--) {
                printf("%d", payload[i] & (1 << j) ? 1 : 0);
                bitsPrinted++;
            }
            printf(" ");
        }
        if(memcmp(payload, original, LENGTH) != 0) {
            printf(" -- DESCRAMBLE FAILED!");
        }
        printf("\n");
        printf("\n");
    }
    return 0;
}