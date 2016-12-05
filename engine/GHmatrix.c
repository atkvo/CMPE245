#include <stdlib.h>
#include <stdio.h>
#include "GHmatrix.h"


char genMatrix[__k][__n];
char idMatrix[__k][__k];
char idMatrixParity[__n - __k][__n - __k];
char hMatrix[__n][__n - __k];
char pMatrix[__k][__n - __k] = {
    { 1, 0, 0, 0 },
    { 0, 1, 0, 0 },
    { 0, 0, 1, 0 },
    { 0, 0, 0, 1 },
    { 1, 0, 1, 0 },
    { 0, 1, 1, 0 },
    { 0, 0, 0, 0 },
    { 0, 0, 1, 1 }
};

//     { 1, 1, 0, 0 },
//     { 0, 1, 1, 0 },
//     { 0, 0, 1, 1 },
//     { 1, 0, 0, 1 },
//     { 1, 0, 1, 0 },
//     { 0, 1, 0, 1 },
//     { 1, 1, 1, 0 },
//     { 0, 1, 1, 1 }
char p[__k_][__n_ - __k_];

int testLinearIndependence(int rows, int columns, char matrix[rows][columns]) {
    int tRow = 0;
    int tCol = 0;
    int match = 1;  // will stay 1 if entire rows
    int matchInv = 1;  // will stay 1 if entire rows
    for(int i = 1; i < rows; i++) {
        for(tRow = i - 1; tRow >= 0; tRow--) {
            for(tCol = 0; tCol < columns; tCol++) {
                if(matrix[tRow][tCol] == matrix[i][tCol]) { 
                    match &= 1; 
                    matchInv &= 0;
                }
                else if(matrix[tRow][tCol] == (matrix[i][tCol]) == 1 ? 0 : 1) { 
                    match &= 0; 
                    matchInv &= 1; 
                }
                else { 
                    match &= 0; 
                    matchInv &= 0;
                }
            }
            if(match == 1 || matchInv == 1) { 
                printf("err at %d\n", i);
                return DEPENDENT; 
            }
            match = 1;  // reset tester
            matchInv = 1;  // reset tester
        }
    }
    return INDEPENDENT;
}

void generateP() {
    int setIndex = 0;
    int dependency = DEPENDENT;
    int retries = 0;
    srand(0);
    // rows
    for(int i = 0; i < __k_; i++) {
        // columns. Keep flipping bits until valid
        while(dependency == DEPENDENT) {
            for(int j = 0; j < __n_ - __k_; j++) {
                p[i][j] = rand() % 2;
            }
            if(testLinearIndependence(i + 1, __n_ - __k_ , p) == DEPENDENT) {
                // printf("DEPENDENT @ %d\n", i);
                retries++;
            }
            else { dependency = INDEPENDENT; }

            // if (retries > 8000) { return; }
        }
        dependency = DEPENDENT;  // reset check
    }
}

/* 0,0 1,1, 2,2, 3,3, 4,4 */
void fillIdMatrices() {
    int stop = __k;
    for(int i = 0; i < stop; i++) {
        idMatrix[i][i] = 1;
    }
    stop = __n - __k;
    for(int i = 0; i < stop; i++) {
        idMatrixParity[i][i] = 1;
    }
}

void initLBC() {
    fillIdMatrices();
}

char accessGenMatrix(int i, int j) {
    /* G = Ik,k | Pk,n-k */
    if(j < __k) {
        // access from the identity matrix
        return idMatrix[i][j];
    } else {
        // access from the P matrix
        return pMatrix[i][j - __k];
    }
}

char accessHMatrix(int i, int j) {
    if(i < __k) {
        // access from the p matrix
        return pMatrix[i][j];
    } else {
        // access from the identity matrix
        return idMatrixParity[i - __k][j];
    }
}
/* c_1,n = d_1,k g_k,n */
/*
    for(int j = 0; j < (__n - __k); j++) {
        result[j] = 0;
        for(int k = 0; k < (__n); k++) {
            result[j] += e[k]*accessHMatrix(k, j);
        }
        result[j] = result[j]%2;
    }
    */
int encodeData(char *buf, int maxLength) {
    // if(maxLength >= (__n)) {
        int tmp = 0;
        /* multiply D * G here */
        for(int j = 0; j < __n; j++) {
            tmp = 0;
            for(int k = 0; k < __k; k++) {
                tmp += buf[k]*accessGenMatrix(k, j);
            }
            buf[j] = tmp %2;
        }
        return 0;
    // } else { return -1; }
}

int testGH() {
    int rowG = __k;
    int colG = __n;
    int colH = __n - __k;
    char result[__k][__n - __k];

    for(int i = 0; i < rowG; i++) {
        for(int j = 0; j < colH; j++) {
            result[i][j] = 0;
            for(int k = 0; k < colG; k++) {
                result[i][j] += accessGenMatrix(i, k)*accessHMatrix(k, j);
            }
            result[i][j] = result[i][j]%2;
            if(result[i][j] != 0) { return -1; }
        }
    }
    return 0;
}

int printMatrix(int rows, int cols, char matrix[rows][cols]) {
    int i = 0;
    int j = 0;
    for(i = 0; i < rows; i++) {
        printf("%3d | ", i);
        for(j = 0; j < cols; j++) {
            printf(" %d ", matrix[i][j]);
        }
        printf(" |\n");
    }
    return OK;
}

void printG() {
    for(int i = 0; i < __k; i++) {
        printf(" | ");
        for(int j = 0; j < __n; j++) {
            printf(" %d ", accessGenMatrix(i, j));
        }
        printf(" |\n");
    }
}
void printH() {
    for(int i = 0; i < __n; i++) {
        printf(" | ");
        for(int j = 0; j < __n - __k; j++) {
            printf(" %d ", accessHMatrix(i, j));
        }
        printf(" |\n");
    }
}
void printP() {
    printMatrix(__k_, __n_ - __k_, p);
}

int getSyndrome(char *e, char* result) {
    // only 1 row
    // n columns
    int returnCode = SYNDROME_OK;
    for(int j = 0; j < (__n - __k); j++) {
        result[j] = 0;
        for(int k = 0; k < (__n); k++) {
            result[j] += e[k]*accessHMatrix(k, j);
        }
        result[j] = result[j]%2;
        if(result[j] == 1) { returnCode = SYNDROME_ERR; }
    }
    return returnCode;
}