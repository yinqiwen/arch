/*
 * rc4.c
 *
 *  Created on: 2013-1-15
 *      Author: wqy
 */
#include "rc4.h"
#include <stdint.h>

#define swap_int(_a, _b) int _t = _a; _a = _b; _b = _t;

void rc4_cipher(char* data, int datalen, const char* ikey, int keylen) {
	uint32_t sbox_size = 256;
	unsigned char sbox[sbox_size];
	unsigned const char * key = (unsigned const char *) ikey;

	// Sbox Initilisieren
	unsigned char j = 0;
	uint32_t i = 0;
	for (i = 0; i < sbox_size; i++)
		sbox[i] = i;
	// Sbox randomisieren
	for (i = 0; i < sbox_size; i++) {
		j += sbox[i] + key[i % keylen];
		swap_int(sbox[i], sbox[j]);
	}

	j = 0;
	i = 0;
	int n = 0;

	unsigned char* content = (unsigned char*) data;
	for (n = 0; n <= datalen; n++) {
		i++;
		uint32_t index = i % sbox_size;
		j += sbox[index];
		swap_int(sbox[index], sbox[j]);
		content[n] = content[n] ^ (sbox[(sbox[index] + sbox[j]) & 0xFF]);
	}
}

