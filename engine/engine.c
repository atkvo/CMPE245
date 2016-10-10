#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "engine.h"


// buffer methods
void initBuffer(s_buff *buf) {
    clearBuffer(buf);
}

void clearBuffer(s_buff *buf) {
    for(int i = 0; i < BUF_SIZE; i++) {
        buf->stream[i] = 0;
    }
    buf->bits = 0;
}

void pushToBufferBitByBit(s_buff *buf, char c) {
    if(buf->bits >= BUF_SIZE - 1) {
        clearBuffer(buf);
    }
    for(int i = 0; i < 8; i++) {
        buf->stream[buf->bits] = ((c >> (7 - i)) & 0x01);
        buf->bits++;
    }
}

// 
void pushBitToBuffer(s_buff *buf, char c) {
    pushCharToBuffer(buf, c);
}

void pushCharToBuffer(s_buff *buf, char c) {
    if(buf->bits >= BUF_SIZE - 1) {
        clearBuffer(buf);
    }
    buf->stream[buf->bits] = c;
    buf->bits++;
}

void p_generateSync(s_buff *sb, uint16_t bytes, int enableCorruption, int percentage) {
    char tmp = 0;
    int offset = sb->bits > 0 ? sb->bits : 0;
    bool corrupt = false;
    int ratio = percentage/10;
    for(int i = 0; i < bytes; i++) {
        for(int b = 7; b >= 0; b--) {
            if( ((rand() % (12 + 1)) < ratio)) {
                corrupt = true;
            }
            else { corrupt = false; }
            if(i <= ((bytes/2) - 1)) {
                tmp = 0x50 + i;
            }
            else {
                tmp = 0xa0 + (i - (bytes/2));
            }
            tmp = (0x01 & tmp >> b);
            if (enableCorruption && corrupt ) { tmp = !tmp; }
            sb->stream[offset + i*8 + (7-b)] = tmp;
            sb->bits++;
        }
    }
}
void generateSync(s_buff *sb, uint16_t bytes) {
    p_generateSync(sb, bytes, 0, 0);
}

// percentage must be >= 10 otherwise will be treated as 0
void generateCorruptedSync(s_buff *sb, uint16_t bytes, int percentage) {
    p_generateSync(sb, bytes, 1, percentage);
}

void addPayload(s_buff *sbuf, char * payload) {
    // int startIndex = sbuf->bits;
    for (int i = 0; payload[i] != '\0'; ++i)
    {
        pushToBufferBitByBit(sbuf, payload[i]);
    }
}


// returns the index of payload within data->stream
// returns -1 if minimum confidence isn't met
int scanForMatch(s_buff *window, s_buff *data, unsigned int minConfidence) {
    unsigned int bitHit = 0;
    unsigned int currentConfidence = 0;
    int lastMatchedHexRead = 0;
    int currentHex = 0;

    // shift buffer and compare  to buffer
    for(unsigned int shiftAmt = 0; shiftAmt < data->bits - window->bits; shiftAmt++) {
        // scanning window along buffer
        lastMatchedHexRead = 0;
        currentHex = 0;
        currentConfidence = 0;
        for(unsigned int scanIndex = 0; scanIndex < window->bits; scanIndex++) {
            if (window->stream[scanIndex] == data->stream[shiftAmt + scanIndex]) {
                // match
                currentHex |= (data->stream[shiftAmt + scanIndex] << (7-bitHit));
                bitHit++;
                // make sure the read byte has a valid MSNibble (0x5_ or 0xA_)
                if(bitHit > 7 && 
                    ((currentHex & 0xF0) == 0x50 || (currentHex & 0xF0) == 0xa0)
                    ) {
                    lastMatchedHexRead = currentHex;
                    currentHex = 0;
                    currentConfidence++;
                    bitHit = 0;
                }
                if(currentConfidence >= minConfidence) {
                    int byteOffset = 0;
                    if((lastMatchedHexRead & 0xF0) == 0x50) {
                        byteOffset = 0x5f - lastMatchedHexRead;
                        byteOffset += 16;
                    }
                    else if((lastMatchedHexRead & 0xF0) == 0xa0) {
                        byteOffset = 0xaf - lastMatchedHexRead;
                    }
                    else {
                        return -1;
                    }
                    // add the byte offset bits to the start of the next byte in scan
                    return (shiftAmt + scanIndex + 1) + byteOffset*8;
                }
            }
            else {
                // no match
                bitHit = 0;
                currentHex = 0;
            }
        }
        bitHit = 0;
        lastMatchedHexRead = 0;
    }
    return -1;
}

void extractPayload(s_buff *data, char *c, int startIndex, unsigned int maxLength) {
    int j = 0;
    for(int i = startIndex; data->stream[i] != '\n' && i < data->bits && j < maxLength; i += 8) {
        c[j] = 0;
        for(int b = 0; b < 8; b++) {
            c[j] |= (data->stream[i + b] << (7 - b));
        }
        j++;
    }
}

void printPacket(s_buff *b) {
    for (unsigned int i = 0; i < b->bits; i++)
    {
        #if defined(DEBUG_ALL) || defined(DEBUG_PRINT_PACKET)
        if (i % 8 == 0) { printf(" \n "); }
        #endif

        if (b->stream[i] == 0) {
            printf("0");
        }
        else if (b->stream[i] == 1) {
            printf("1");
        }
        else {
            printf("%c", b->stream[i]);
        }
    }
}

void printPacketHex(s_buff *b, enum SEPARATOR_TYPE sepType) {
    uint16_t ch = 0; 
    printf("BITS: %i\n", b->bits);
    for (unsigned int i = 0; i < b->bits; i+=8) {
        for (int j = 0; j < 8; j++) {
            if(b->stream[i + j] == 1) {
                ch |= (1 << (7 - j));
            }
        }
        printf("%2x", ch);
        switch(sepType) {
            case DASH: printf("-"); break;
            case NEWLINE: printf("\n"); break;
            case NONE:
            default: break;
        }
        ch &= 0;
    }
}
