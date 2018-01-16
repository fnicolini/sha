#include "sha256.h"

const uint32_t k[64] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
   0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
   0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
   0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
   0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
   0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
   0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
   0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};


#define ROTL(a, b) ((a << b) | (a >> (32 - b)))
#define ROTR(a, b) ((a >> b) | (a << (32 - b)))

#define S0(x) (ROTR(x, 7) ^ ROTR(x, 18) ^ (x >> 3))
#define S1(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ (x >> 10))

#define M0(x) (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define MAJ(a, b, c) (a & b) ^ (a & c) ^ (b & c)
#define M1(x) (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define CH(e, f, g) (e & f) ^ ((~e) & g)





int main(int argc, char const *argv[]) {

	if (argc == 1) {
		fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
		return 1;
	}


	// variables initialization

	uint32_t h0 = 0x6a09e667;
	uint32_t h1 = 0xbb67ae85;
	uint32_t h2 = 0x3c6ef372;
	uint32_t h3 = 0xa54ff53a;
	uint32_t h4 = 0x510e527f;
	uint32_t h5 = 0x9b05688c;
	uint32_t h6 = 0x1f83d9ab;
	uint32_t h7 = 0x5be0cd19;

	uint32_t s0, s1, a, b, c, d, e, f, g, h, m0, maj, t2, m1, ch, t1;


	FILE *fp = fopen(argv[1], "rb");
	if (fp == NULL) {
		perror("Error");
		return(1);
	}

	uint64_t fileSize = 0;
	fseek(fp, 0L, SEEK_END);
	fileSize = ftell(fp);
	rewind(fp);

	int oneMoreChunk = 0;

	uint32_t w[64]; // 512 bits chunk buffer + space for extension to 64 32-bits words
	size_t elementsRead = 0;

	uint8_t buffer[64]; // array of 64 8-bits elements

	// main cycle
	while(elementsRead = fread(buffer, 1, 64, fp) == 64) {   // reading 512 bits

		int j = 0;
		for (int i = 0; i < 64; i += 4) {
			w[j] = buffer[i+3];
			w[j] << 8;
			w[j] |= buffer[i+2];
			w[j] << 8;
			w[j] |= buffer[i+1];
			w[j] << 8;
			w[j] |= buffer[i];
			j++;

		}

		/* transform to big endian
		for (int i = 0; i < 16; ++i) {
			w[i] = bswap_32(w[i]);
		}
		*/

		//extension into sixty-four 32-bit words

		for (int i = 16; i < 64; ++i) {
			s0 = S0(w[i-15]);
			s1 = S1(w[i-2]);
			w[i] = w[i-16] + s0 + w[i-7] + s1;
		}

		// Initialize hash value for this chunk

		a = h0;
		b = h1;
		c = h2;
		d = h3;
		e = h4;
		f = h5;
		g = h6;
		h = h7;

		//main loop
		for (int i = 0; i < 64; ++i) {
			t2 = M0(a) + MAJ(a, b, c);
			t1 = h + M1(e) + CH(e, f, g) + k[i] + w[i];
			h = g;
			g = f;
			f = e;
			e = d + t1;
			d = c;
			c = b;
			b = a;
			a = t1 + t2;
		}

		// update h0 h1 ... values

		h0 += a;
		h1 += b;
		h2 += c;
		h3 += d;
		h4 += e;
		h5 += f;
		h6 += g;
		h7 += h;

	}
	fclose(fp);

	uint8_t wordsFilled = elementsRead / 4;
	uint8_t rest = elementsRead % 4;

	int j = 0;
	for (int i = 0; i < elementsRead - rest; i += 4) {
		w[j] = buffer[i+3];
		w[j] << 8;
		w[j] |= buffer[i+2];
		w[j] << 8;
		w[j] |= buffer[i+1];
		w[j] << 8;
		w[j] |= buffer[i];
		j++;
	}

	if(rest > 0) { // && < 4
		for (int i = elementsRead - rest; i < elementsRead; ++i) {
			w[j] |= buffer[i];
			w[j] << 8;
		}

		w[j] != 0x80;
		w[j] << 8;

		if(4 - (rest + 1) == 1)
			w[j] &= 0xffffff00;

		else if (4 - (rest + 1) == 2) {
			w[j] &= 0xffffff00;
			w[j] << 8;
			w[j] &= 0xffffff00;
		}
		j++;

	}

	if((16 - j) >= 2) { // numero di word da 32bit liberi rimanenti nel chunk
		//c'è spazio subito in questo chunk per la lunghezza a 64 bit
		for (j; j < 14; ++j) {
			w[j] == 0;
		}

		printf("FILESIZE HEX = %llx\n", fileSize);

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

		printf("W14 HEX = %lx\n", w[14]);
		printf("W15 HEX = %lx\n", w[15]);
	}
	else {
		for (j; j < 16; ++j) { // executed max once
			w[j] = 0;
		}
		oneMoreChunk = 1;
	}

	//extension into sixty-four 32-bit words

	for (int i = 16; i < 64; ++i) {
		s0 = S0(w[i-15]);
		s1 = S1(w[i-2]);
		w[i] = w[i-16] + s0 + w[i-7] + s1;
	}

	// Initialize hash value for this chunk

	a = h0;
	b = h1;
	c = h2;
	d = h3;
	e = h4;
	f = h5;
	g = h6;
	h = h7;

	//main loop
	for (int i = 0; i < 64; ++i) {
		t2 = M0(a) + MAJ(a, b, c);
		t1 = h + M1(e) + CH(e, f, g) + k[i] + w[i];
		h = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + t2;
	}

	// update h0 h1 ... values

	h0 += a;
	h1 += b;
	h2 += c;
	h3 += d;
	h4 += e;
	h5 += f;
	h6 += g;
	h7 += h;

	if(oneMoreChunk) {
		for (int i = 0; i < 14; ++i) {
			w[i] = 0;
		}

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

		for (int i = 16; i < 64; ++i) {
			s0 = S0(w[i-15]);
			s1 = S1(w[i-2]);
			w[i] = w[i-16] + s0 + w[i-7] + s1;
		}

		// Initialize hash value for this chunk

		a = h0;
		b = h1;
		c = h2;
		d = h3;
		e = h4;
		f = h5;
		g = h6;
		h = h7;

		//main loop
		for (int i = 0; i < 64; ++i) {
			t2 = M0(a) + MAJ(a, b, c);
			t1 = h + M1(e) + CH(e, f, g) + k[i] + w[i];
			h = g;
			g = f;
			f = e;
			e = d + t1;
			d = c;
			c = b;
			b = a;
			a = t1 + t2;
		}

		// update h0 h1 ... values

		h0 += a;
		h1 += b;
		h2 += c;
		h3 += d;
		h4 += e;
		h5 += f;
		h6 += g;
		h7 += h;


	}

	printf("se 1 introia = %d\n", oneMoreChunk);
	printf("fileSize(bytes) = %d\n", fileSize);

	printf("sha256 = %x%x%x%x%x%x%x\n", h0, h1, h2, h3, h4, h5, h6, h7);



	
	return 0;
}