#ifndef SHA512_H
#define SHA512_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h> //memset
#include <inttypes.h> //PRIu32

void sha512(const char *filePath);
void sha512_initW(uint64_t *w, uint8_t *buffer, int length);
void sha512_processChunk(uint64_t *w);
void sha512_initAlphabeth();
void sha512_updateAlphabeth(uint64_t *w);
void sha512_updateHash();


#endif