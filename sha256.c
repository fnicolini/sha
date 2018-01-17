#include "sha256.h"


const uint32_t k[64] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

uint32_t hash[8] = {
    0x6a09e667,
    0xbb67ae85,
    0x3c6ef372,
    0xa54ff53a,
    0x510e527f,
    0x9b05688c,
    0x1f83d9ab,
    0x5be0cd19
};
uint32_t a, b, c, d, e, f, g, h;

#define ROTL(a, b) ((a << b) | (a >> (32 - b)))
#define ROTR(a, b) ((a >> b) | (a << (32 - b)))

#define S0(x) ((ROTR(x, 7)) ^ (ROTR(x, 18)) ^ (x >> 3))
#define S1(x) ((ROTR(x, 17)) ^ (ROTR(x, 19)) ^ (x >> 10))

#define M0(x) ((ROTR(x, 2)) ^ (ROTR(x, 13)) ^ (ROTR(x, 22)))
#define MAJ(a, b, c) (a & b) ^ (a & c) ^ (b & c)
#define M1(x) ((ROTR(x, 6)) ^ (ROTR(x, 11)) ^ (ROTR(x, 25)))
#define CH(e, f, g) (e & f) ^ ((~e) & g)




int main(int argc, char const *argv[]) {

    if (argc == 1) {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        return 1;
    }

    uint32_t s0, s1, m0, maj, t2, m1, ch, t1;


    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        perror("Error");
        return (1);
    }

    uint64_t fileSize = 0;
    fseek(fp, 0L, SEEK_END);
    fileSize = ftell(fp);
    rewind(fp);

    bool oneMoreChunk = false;

    uint32_t w[64]; // Array that will contain the 512bit chunk extended into 64 32-bits words
    size_t elementsRead = 0; // Return value of the fread function, used to cycle through the file

    uint8_t buffer[64]; // Input file buffer, size: 64 bytes

    // main cycle
    while ((elementsRead = fread(buffer, 1, 64, fp)) == 64) { // reading 64 bytes, or until EOF

        //printf(buffer);

        initW(w, buffer, 64);

        /*
        for (int i = 0; i < 64; ++i) {
            printf("w[%d] = %"PRIu32"\n", i, w[i]);
        }
        */


        processChunk(w);

        initAlphabeth();

        updateAlphabeth(w);

        updateHash();

    }

    /* At this point we finished processing all the possible 64 Bytes chunks of the file
       The file is either fully processed (its size(bytes) was a multiple of 64) or there is still
       some information left to process.
       Either way there is still to add the final padding and process some more data.
    */ 

    initW(w, buffer, elementsRead);

    startPadding(w, elementsRead); // qui w è già a posto, non deve essere shiftata ecc

    

    int j = elementsRead / 4;
    if(elementsRead % 4)
        j++;

    // At this point j is equal to the 32-bit word in w[] that have been processed

    if ((16 - j) >= 2) { 
        //c'è spazio subito in questo chunk per la lunghezza a 64 bit


        w[14] = fileSize & 0x00000000000000ff;
        w[14] <<= 8;
        w[14] |= (fileSize >> 8) & 0x00000000000000ff;
        w[14] <<= 8;
        w[14] |= (fileSize >> 16) & 0x00000000000000ff;
        w[14] <<= 8;
        w[14] |= (fileSize >> 24) & 0x00000000000000ff;

        w[15] = (fileSize >> 32) & 0x00000000000000ff;
        w[15] <<= 8;
        w[15] |= (fileSize >> 40) & 0x00000000000000ff;
        w[15] <<= 8;
        w[15] |= (fileSize >> 48) & 0x00000000000000ff;
        w[15] <<= 8;
        w[15] |= (fileSize >> 56) & 0x00000000000000ff;

    } 
    else 
        oneMoreChunk = true;

    //extension into sixty-four 32-bit words
    processChunk(w);

    // Initialize hash value for this chunk
    initAlphabeth();

    updateAlphabeth(w);

    // update h0 h1 ... values
    updateHash();

    if (oneMoreChunk) {
        memset(w, 0x0, 256);

        w[14] = fileSize & 0x00000000000000ff;
        w[14] <<= 8;
        w[14] |= (fileSize >> 8) & 0x00000000000000ff;
        w[14] <<= 8;
        w[14] |= (fileSize >> 16) & 0x00000000000000ff;
        w[14] <<= 8;
        w[14] |= (fileSize >> 24) & 0x00000000000000ff;

        w[15] = (fileSize >> 32) & 0x00000000000000ff;
        w[15] <<= 8;
        w[15] |= (fileSize >> 40) & 0x00000000000000ff;
        w[15] <<= 8;
        w[15] |= (fileSize >> 48) & 0x00000000000000ff;
        w[15] <<= 8;
        w[15] |= (fileSize >> 56) & 0x00000000000000ff;
        
        processChunk(w);

        // Initialize hash value for this chunk
        initAlphabeth();

        updateAlphabeth(w);

        updateHash();



    }


    printf("sha256 = %x%x%x%x%x%x%x%x\n", hash[0], hash[1], hash[2], hash[3], hash[4], hash[5], hash[6], hash[7]);

    fclose(fp);

    return 0;
}

/**
 * Reads the buffer and fills w according to the Big Endian standard.
 * @param buffer the buffer, an array of char
 * @param the number of chars it contains (there may be some empty chars in the buffer)
 * @return 
 */
void initW(uint32_t *w, uint8_t *buffer, int length) {
    memset(w, 0x0, 256); // slightly faster than the loop cycle
    for (int j = 0; j < length / 4; j++) {
        for (int i = 0; i < 4; i++) {
            w[j] <<= 8;
            w[j] |= buffer[j * 4 + i];
        }
    }
}


/**
 * Processes the chunk, extending it into 64 32-bit words.
 * @param chunk
 */
void processChunk(uint32_t * w) {
    //extension into sixty-four 32-bit words
    uint32_t s0, s1;
    for (int i = 16; i < 64; ++i) {
        s0 = S0(w[i - 15]);
        s1 = S1(w[i - 2]);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }

}

/**
 * Initializes the letters a, b, c... to the current values of the hash
 */
void initAlphabeth() {
    // Initialize hash value for this chunk
    a = hash[0];
    b = hash[1];
    c = hash[2];
    d = hash[3];
    e = hash[4];
    f = hash[5];
    g = hash[6];
    h = hash[7];
}
/**
 * Updates the aphabeth.
 */
void updateAlphabeth(uint32_t *w) {
    uint32_t t1, t2;
    //main loop
    for (int i = 0; i < 64; ++i) {
        t2 = M0(a) + (MAJ(a, b, c));
        t1 = h + M1(e) + (CH(e, f, g)) + k[i] + w[i];
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

}

/**
 * Updates the hash adding it the current values of the letters a, b, c,...
 */
void updateHash() {

    hash[0] += a;
    hash[1] += b;
    hash[2] += c;
    hash[3] += d;
    hash[4] += e;
    hash[5] += f;
    hash[6] += g;
    hash[7] += h;

}

/**
 * Inserts the first byte of padding
 * @param w
 * @param nElements
 */
void startPadding(uint32_t *w, int nElements){
    int wordIndex = (nElements) / 4; // the floor of the division
    /* So that:
     * nElements = 0, 1, 2, 3 -> wordIndex =0
     * nElements = 4, 5... -> wordIndex = 1*/
    int byteIndex = 3 - (nElements % 4); 
    /* So that:
     * nElements = 0 -> byteIndex (of the padding) =0 (counted from LSB to MSB)
     * nElements = 1 -> byteIndex =1 (counted from LSB to MSB)
     */
    
    uint32_t paddingMask = 0xFFFFFF80;
    paddingMask <<= byteIndex * 8;
    /*So that:
     *switch(byteIndex){ 
     *   case 0:
     *       paddingMask = 0xFFFFFF80;
     *       break;
     *   case 1:
     *       paddingMask = 0xFFFF8000;
     *       break;
     *   case 2:
     *       paddingMask = 0xFF800000;
     *       break;
     *   case 3:
     *       paddingMask = 0x80000000;
     * 
     *}*/
    w[wordIndex]&=paddingMask;
     
}