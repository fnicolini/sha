#ifndef SHA256_H
#define SHA256_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h> //memset
#include <inttypes.h> //PRIu32

void initW(uint32_t *w, uint8_t *buffer, int length);
void processChunk(uint32_t * w);
void initAlphabeth();
void updateAlphabeth(uint32_t *w);
void updateHash();
void startPadding(uint32_t *w, uint8_t *buffer, int nElements);


#endif