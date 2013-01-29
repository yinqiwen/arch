/*
 * stringsearch.h
 *
 *  Created on: 2012-1-9
 *      Author: qiyingwang
 */

#ifndef STRINGSEARCH_H_
#define STRINGSEARCH_H_
#include <limits.h>
#ifdef __cplusplus
extern "C"
{
#endif

    int boyer_moore(const unsigned char *string, ssize_t stringlen,
            const unsigned char *pat, ssize_t patlen);
    int
    boyermoore_horspool(const unsigned char* haystack, ssize_t hlen,
            const unsigned char* needle, ssize_t nlen);

    int bitap_search(const unsigned char *text, size_t len, const unsigned char *pattern,
            size_t patlen);

    int sunday_search(const unsigned char *text, size_t text_len, const unsigned char *find,
            size_t find_len);
    int robin_karp_search(const unsigned char *text, size_t text_len, const unsigned char *find,
            size_t find_len);

    int railgun_quadruplet_search(const unsigned char * pbTarget,
            size_t cbTarget, const unsigned char * pbPattern, size_t cbPattern);
#ifdef __cplusplus
}
#endif
#endif /* STRINGSEARCH_H_ */
