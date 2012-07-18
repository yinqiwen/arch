/*
 * crc32.h
 *
 *  Created on: 2011-8-15
 *      Author: qiyingwang
 */

#ifndef CRC32_H_
#define CRC32_H_
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
uint32_t crc32 (uint32_t crc, unsigned char *buf, size_t len);
#ifdef __cplusplus
}
#endif
#endif /* CRC32_H_ */
