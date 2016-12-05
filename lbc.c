#include <stdio.h>
#include <stdlib.h>
#include "GHmatrix.h"

char c_saved[__n] = { 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0 };
char e_saved[__n] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
char s_saved[__n - __k] = { 0, 0, 0, 0 };

void restoreValues(char *c, char *e, char *s) {
    for(int i = 0; i < __n; i++) {
        c[i] = c_saved[i];
        e[i] = e_saved[i];
        if(i < (__n - __k)) { s[i] = s_saved[i]; }
    }
}

int main(int argc, char const *argv[])
{
    // char c[__n] = { 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0 };
    char c[__n] = { 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 };
    char e[__n] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    char s[__n - __k] = { 0, 0, 0, 0 };
    initLBC();
    while(1) {
        /* Prints the data  */
        // for(int i = 0; i < __k; i++) {
        //     if(i == __k) printf(" ");
        //     printf("%d", c[i]);
        // }

        /* Tests if p matrix is linearly independent*/
        // if(testLinearIndependence(__k, __n - __k, pMatrix) == INDEPENDENT) {
        //     printf("\nP is ok\n");
        // } else {
        //     printf("ERR\n");
        // }
        // printf("\n");

        // if( testGH() == 0 ) {
        //     printf("GH is ok\n");
        // }

        /* ask user for noise bit positions */
        int noiseBit = -1;
        printf("\nSelect noise bit position 1: ");
        scanf("%d", &noiseBit);
        if(noiseBit >= 0 && noiseBit < __k) {
            // printf("noise bit @ %d", noiseBit);
            e[noiseBit] = 1;
        }

        /* Get second noise bit */
        printf("\nSelect noise bit position 2: ");
        scanf("%d", &noiseBit);
        if(noiseBit >= 0 && noiseBit < __k) {
            // printf("noise bit @ %d", noiseBit);
            e[noiseBit] = 1;
        }

        /* Encode payload and print the encoding */
        printf("\nEncoded payload: ");
        encodeData(c, __n);
        for(int i = 0; i < __n; i++) {
            if(i == __k) printf(" ");
            printf("%d", c[i]);
        }


        /* Add noise array to packet */
        for(int i = 0; i < __k; i++) {
            c[i] ^= e[i];
        }
        printf("\nAltered payload: ");
        for(int i = 0; i < __n; i++) {
            if(i == __k) printf(" ");
            printf("%d", c[i]);
        }

        /* Extract syndrome from encoded packet + noise */
        getSyndrome(c, s);
        printf("\nSYNDROME       : ");
        for(int i = 0; i < __n - __k; i++) {
            printf("%d", s[i]);
        }
        printf("\n");
        restoreValues(c, e, s);
    }
    return 0;
}