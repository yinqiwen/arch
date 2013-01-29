/*
 * rc4.h
 *
 *  Created on: 2013-1-15
 *      Author: wqy
 */

#ifndef RC4_H_
#define RC4_H_
#ifdef __cplusplus
extern "C" {
#endif
void rc4_cipher(char* data, int datalen, const char* ikey, int keylen);

#ifdef __cplusplus
}
#endif
#endif /* RC4_H_ */
