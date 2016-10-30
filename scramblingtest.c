#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "engine.h"


// scramble and descrable this
char payload[] =  { 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1 };
char original[] = { 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1 };

int main() {
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