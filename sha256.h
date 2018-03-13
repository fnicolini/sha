#ifndef SHA256_H
#define SHA256_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h> //memset
#include <inttypes.h> //PRIu32

void sha256(char const *filePath);
void sha256_initW(uint32_t *w, uint8_t *buffer, int length);
void sha256_processChunk(uint32_t * w);
void sha256_initAlphabeth();
void sha256_updateAlphabeth(uint32_t *w);
void sha256_updateHash();



#endif