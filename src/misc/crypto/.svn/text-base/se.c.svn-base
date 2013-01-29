/*
 * se.c
 *
 *  Created on: 2013-1-16
 *      Author: wqy
 */
#include "se.h"
#include <stdint.h>

void se1_encrypt(char* data, int datalen) {
	unsigned char* content = (unsigned char*) data;
	int i = 0;
	for (i = 0; i <= datalen; i++) {
		int32_t v = content[i];
		v = v - 1;
		if (v < 0) {
			v += 256;
		}
		content[i] = v;
	}
}
void se1_decrypt(char* data, int datalen) {
	unsigned char* content = (unsigned char*) data;
	int i = 0;
	for (i = 0; i <= datalen; i++) {
		int32_t v = content[i];
		v = v + 1;
		if (v >= 256) {
			v -= 256;
		}
		content[i] = v;
	}
}

