#include <stdio.h>
#include <stdlib.h>
#include "GHmatrix.h"

int main(int argc, char const *argv[])
{
    char c[__n] = { 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0 };
    char e[__n] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    char s[__n - __k] = { 0, 0, 0, 0 };
    initLBC();

    /* Prints the data  */
    for(int i = 0; i < __k; i++) {
        if(i == __k) printf(" ");
        printf("%d", c[i]);
    }

    /* Tests if p matrix is linearly independent*/
    if(testLinearIndependence(__k, __n - __k, pMatrix) == INDEPENDENT) {
        printf("P is ok\n");
    } else {
        printf("ERR\n");
    }
    printf("\n");

    // if( testGH() == 0 ) {
    //     printf("GH is ok\n");
    // }

    /* ask user for noise bit positions */
    int noiseBit = -1;
    printf("Select noise bit position 1: ");
    scanf("%d", &noiseBit);
    if(noiseBit >= 0 && noiseBit < __k) {
        printf("noise bit @ %d", noiseBit);
        e[noiseBit] = 1;
    }

    /* Get second noise bit */
    printf("\nSelect noise bit position 2: ");
    scanf("%d", &noiseBit);
    if(noiseBit >= 0 && noiseBit < __k) {
        printf("noise bit @ %d", noiseBit);
        e[noiseBit] = 1;
    }

    /* Encode payload and print the encoding */
    printf("Encoded payload: ");
    encodeData(c, __n);
    for(int i = 0; i < __n; i++) {
        if(i == __k) printf(" ");
        printf("%d", c[i]);
    }
    printf("\n");


    /* Add noise array to packet */
    for(int i = 0; i < __k; i++) {
        c[i] ^= e[i];
    }
    printf("Altered payload: ");
    for(int i = 0; i < __n; i++) {
        if(i == __k) printf(" ");
        printf("%d", c[i]);
    }
    printf("\n");

    /* Extract syndrome from encoded packet + noise */
    getSyndrome(c, s);
    printf("\nSYNDROME: ");
    for(int i = 0; i < __n - __k; i++) {
        printf("%d", s[i]);
    }
    printf("\n");
    return 0;
}