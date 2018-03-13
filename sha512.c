#include "sha512.h"

static const uint64_t k[80] = {
  0x428A2F98D728AE22, 0x7137449123EF65CD, 0xB5C0FBCFEC4D3B2F, 0xE9B5DBA58189DBBC, 0x3956C25BF348B538,
  0x59F111F1B605D019, 0x923F82A4AF194F9B, 0xAB1C5ED5DA6D8118, 0xD807AA98A3030242, 0x12835B0145706FBE,
  0x243185BE4EE4B28C, 0x550C7DC3D5FFB4E2, 0x72BE5D74F27B896F, 0x80DEB1FE3B1696B1, 0x9BDC06A725C71235,
  0xC19BF174CF692694, 0xE49B69C19EF14AD2, 0xEFBE4786384F25E3, 0x0FC19DC68B8CD5B5, 0x240CA1CC77AC9C65,
  0x2DE92C6F592B0275, 0x4A7484AA6EA6E483, 0x5CB0A9DCBD41FBD4, 0x76F988DA831153B5, 0x983E5152EE66DFAB,
  0xA831C66D2DB43210, 0xB00327C898FB213F, 0xBF597FC7BEEF0EE4, 0xC6E00BF33DA88FC2, 0xD5A79147930AA725,
  0x06CA6351E003826F, 0x142929670A0E6E70, 0x27B70A8546D22FFC, 0x2E1B21385C26C926, 0x4D2C6DFC5AC42AED,
  0x53380D139D95B3DF, 0x650A73548BAF63DE, 0x766A0ABB3C77B2A8, 0x81C2C92E47EDAEE6, 0x92722C851482353B,
  0xA2BFE8A14CF10364, 0xA81A664BBC423001, 0xC24B8B70D0F89791, 0xC76C51A30654BE30, 0xD192E819D6EF5218, 
  0xD69906245565A910, 0xF40E35855771202A, 0x106AA07032BBD1B8, 0x19A4C116B8D2D0C8, 0x1E376C085141AB53,
  0x2748774CDF8EEB99, 0x34B0BCB5E19B48A8, 0x391C0CB3C5C95A63, 0x4ED8AA4AE3418ACB, 0x5B9CCA4F7763E373,
  0x682E6FF3D6B2B8A3, 0x748F82EE5DEFB2FC, 0x78A5636F43172F60, 0x84C87814A1F0AB72, 0x8CC702081A6439EC,
  0x90BEFFFA23631E28, 0xA4506CEBDE82BDE9, 0xBEF9A3F7B2C67915, 0xC67178F2E372532B, 0xCA273ECEEA26619C,
  0xD186B8C721C0C207, 0xEADA7DD6CDE0EB1E, 0xF57D4F7FEE6ED178, 0x06F067AA72176FBA, 0x0A637DC5A2C898A6,
  0x113F9804BEF90DAE, 0x1B710B35131C471B, 0x28DB77F523047D84, 0x32CAAB7B40C72493, 0x3C9EBE0A15C9BEBC,
  0x431D67C49C100D4C, 0x4CC5D4BECB3E42B6, 0x597F299CFC657E2A, 0x5FCB6FAB3AD6FAEC, 0x6C44198C4A475817
};

static const uint64_t starting_hash[8] = {
  0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1, 0x510e527fade682d1,
  0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179 };

static uint64_t hash[8];

static uint64_t a, b, c, d, e, f, g, h;


#define ROTL(a, b) ((a << b) | (a >> (64 - b)))
#define ROTR(a, b) ((a >> b) | (a << (64 - b)))

#define S0(x) ((ROTR(x, 1)) ^ (ROTR(x, 8)) ^ (x >> 7))
#define S1(x) ((ROTR(x, 19)) ^ (ROTR(x, 61)) ^ (x >> 6))

#define M0(x) ((ROTR(x, 28)) ^ (ROTR(x, 34)) ^ (ROTR(x, 39)))
#define MAJ(a, b, c) (a & b) ^ (a & c) ^ (b & c)
#define M1(x) ((ROTR(x, 14)) ^ (ROTR(x, 18)) ^ (ROTR(x, 41)))
#define CH(e, f, g) (e & f) ^ ((~e) & g)  



void sha512(const char *filePath) {

	for (int i = 0; i < 8; ++i)
        hash[i] = starting_hash[i];

	FILE *fp = fopen(filePath, "rb");
	if (fp == NULL) {
		perror("Error");
		return;
	}
	//
	uint64_t fileSize[] = {0, 0};
	fseek(fp, 0L, SEEK_END);
	fileSize[1] = ftell(fp); // for now, only supports 2^64 Bit file length
	rewind(fp);

	// Need file size in bits
	fileSize[1] *= 8;

	uint64_t w[80];
	size_t elementsRead;
	uint8_t buffer[128]; // Input buffer, size: 128 Bytes
	bool oneMoreChunk = false;


	//main cycle
	while ((elementsRead = fread(buffer, 1, 128, fp)) == 128) {
		
		sha512_initW(w, buffer, 128);

		sha512_processChunk(w);
		sha512_initAlphabeth();
		sha512_updateAlphabeth(w);
		sha512_updateHash();

	}

	fclose(fp);

	buffer[elementsRead] = 0x80;
	elementsRead++;
	
	sha512_initW(w, buffer, 128);

	// At max full 16 words of 80 filled.

	if(elementsRead / 8 < 15) {

		// There is space for the 128-bit length in this chunk
		w[14] = 0; // for now only 2^64 bit length supported (~2,3 exabytes)
		w[15] = fileSize[1];
	}
	else
		oneMoreChunk = true;

	sha512_processChunk(w);

	sha512_initAlphabeth();

	sha512_updateAlphabeth(w);

	sha512_updateHash();

	if (oneMoreChunk) {
		memset(w, 0x0, 640);

		w[14] = 0;
		w[15] = fileSize[1];

		sha512_processChunk(w);

		sha512_initAlphabeth();

		sha512_updateAlphabeth(w);

		sha512_updateHash();
	}

    char actualpath [100];
    char *ptr;
    ptr = realpath(filePath, actualpath);

    printf("sha512 = %lx%lx%lx%lx%lx%lx%lx%lx    %s\n", hash[0], hash[1], hash[2], hash[3], hash[4], hash[5], hash[6], hash[7], actualpath);

}

/**
 * Reads the buffer and fills w. It is done single char by char, so it should be Endian independent.
 * @param buffer the buffer, an array of char
 * @param the number of chars it contains (there may be some empty chars in the buffer)
 * @return 
 */
void sha512_initW(uint64_t *w, uint8_t *buffer, int length) {
	memset(w, 0x0, 640);
	for (int j = 0; j < length/8; ++j) {
		for (int i = 0; i < 8; ++i) {
			w[j] <<= 8;
			w[j] |= buffer[j * 8 + i];		
		}
	}
	memset(buffer, 0x0, 128);
}

/**
 * Processes the chunk, extending it into 80 64-bit words.
 * @param chunk
 */
void sha512_processChunk(uint64_t *w) {
	uint64_t s0, s1;

	for (int i = 16; i < 80; ++i) {
		s0 = S0(w[i-15]);
		s1 = S1(w[i-2]);
		w[i] = s1 + w[i-7] + s0 + w[i-16];
	}

}

/**
 * Initializes the letters a, b, c... to the current values of the hash
 */
void sha512_initAlphabeth() {
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
void sha512_updateAlphabeth(uint64_t *w) {
    uint64_t t1, t2;
    //main loop
    for (int i = 0; i < 80; ++i) {
        t1 = h + M1(e) + (CH(e, f, g)) + k[i] + w[i]; 
        t2 = M0(a) + (MAJ(a, b, c));
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
void sha512_updateHash() {

    hash[0] += a;
    hash[1] += b;
    hash[2] += c;
    hash[3] += d;
    hash[4] += e;
    hash[5] += f;
    hash[6] += g;
    hash[7] += h;

}
