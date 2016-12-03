#ifndef _GH_MATRIX_H_

#define DEPENDENT   1
#define INDEPENDENT 0

#define __k 8
#define __n 12
#define __k_ 12*8
#define __n_ 104
enum gh_errors {
    OK,
    GENERATOR_NOT_READY
};

extern char pMatrix[__k][__n - __k];
extern char genMatrix[__k][__n];
extern char idMatrix[__k][__k];
extern char hMatrix[__n][__n - __k];

void initLBC();

/*  Buffer must be at least size of __n + __k */
int encodeData(char *buf, int maxLength);
int testGH();
void printG();
void printH();
int getSyndrome(char *e, char* result);
int testLinearIndependence(int rows, int columns, char matrix[rows][columns]);
void generateP();
void printP();


#endif