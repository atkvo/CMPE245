#ifndef ENGINE_H
#define ENGINE_H
#endif

#define BUF_SIZE 1024
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    char stream[BUF_SIZE];
    unsigned int bits;
} s_buff;

enum SEPARATOR_TYPE {
    NONE = 0,
    DASH = 1,
    NEWLINE = 2
};

// buffer methods
void initBuffer(s_buff *buf);
void clearBuffer(s_buff *buf);
void pushToBufferBitByBit(s_buff *buf, char c);
void pushBitToBuffer(s_buff *buf, char c);
void pushCharToBuffer(s_buff *buf, char c);

void generateSync(s_buff *sb, uint16_t bytes);
void generateCorruptedSync(s_buff *sb, uint16_t bytes, int seed);

void addPayload(s_buff *sbuf, char * payload);

int scanForMatch(s_buff *window, s_buff *data, unsigned int minConfidence);
void extractPayload(s_buff *data, char *c, int startIndex, unsigned int maxLength);

void printPacket(s_buff *b);
void printPacketHex(s_buff *b, enum SEPARATOR_TYPE sepType);
