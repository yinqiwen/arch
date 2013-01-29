/*
 * stringsearch.c
 *
 *  Created on: 2012-1-9
 *      Author: qiyingwang
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "util/stringsearch.h"

#define ALPHABET_LEN 255
#define NOT_FOUND patlen
#define max(a, b) ((a < b) ? b : a)

// delta1 table: delta1[c] contains the distance between the last
// character of pat and the rightmost occurence of c in pat.
// If c does not occur in pat, then delta1[c] = patlen.
// If c is at string[i] and c != pat[patlen-1], we can
// safely shift i over by delta1[c], which is the minimum distance
// needed to shift pat forward to get string[i] lined up
// with some character in pat.
// this algorithm runs in alphabet_len+patlen time.
static inline void make_delta1(int *delta1, const unsigned char *pat, ssize_t patlen)
{
    int i;
    for (i = 0; i < ALPHABET_LEN; i++)
    {
        delta1[i] = NOT_FOUND;
    }
    for (i = 0; i < patlen - 1; i++)
    {
        delta1[pat[i]] = patlen - 1 - i;
    }
}

// true if the suffix of word starting from word[pos] is a prefix
// of word
static inline int is_prefix(const unsigned char *word, int wordlen, int pos)
{
    int i;
    int suffixlen = wordlen - pos;
    // could also use the strncmp() library function here
    for (i = 0; i < suffixlen; i++)
    {
        if (word[i] != word[pos + i])
        {
            return 0;
        }
    }
    return 1;
}

// length of the longest suffix of word ending on word[pos].
// suffix_length("dddbcabc", 8, 4) = 2
static inline int suffix_length(const unsigned char *word, int wordlen, int pos)
{
    int i;
    // increment suffix length i to the first mismatch or beginning
    // of the word
    for (i = 0; (word[pos - i] == word[wordlen - 1 - i]) && (i < pos); i++)
        ;
    return i;
}

// delta2 table: given a mismatch at pat[pos], we want to align
// with the next possible full match could be based on what we
// know about pat[pos+1] to pat[patlen-1].
//
// In case 1:
// pat[pos+1] to pat[patlen-1] does not occur elsewhere in pat,
// the next plausible match starts at or after the mismatch.
// If, within the substring pat[pos+1 .. patlen-1], lies a prefix
// of pat, the next plausible match is here (if there are multiple
// prefixes in the substring, pick the longest). Otherwise, the
// next plausible match starts past the character aligned with
// pat[patlen-1].
//
// In case 2:
// pat[pos+1] to pat[patlen-1] does occur elsewhere in pat. The
// mismatch tells us that we are not looking at the end of a match.
// We may, however, be looking at the middle of a match.
//
// The first loop, which takes care of case 1, is analogous to
// the KMP table, adapted for a 'backwards' scan order with the
// additional restriction that the substrings it considers as
// potential prefixes are all suffixes. In the worst case scenario
// pat consists of the same letter repeated, so every suffix is
// a prefix. This loop alone is not sufficient, however:
// Suppose that pat is "ABYXCDEYX", and text is ".....ABYXCDEYX".
// We will match X, Y, and find B != E. There is no prefix of pat
// in the suffix "YX", so the first loop tells us to skip forward
// by 9 characters.
// Although superficially similar to the KMP table, the KMP table
// relies on information about the beginning of the partial match
// that the BM algorithm does not have.
//
// The second loop addresses case 2. Since suffix_length may not be
// unique, we want to take the minimum value, which will tell us
// how far away the closest potential match is.
static inline void make_delta2(int *delta2, const unsigned char *pat, ssize_t patlen)
{
    int p;
    int last_prefix_index = patlen - 1;

    // first loop
    for (p = patlen - 1; p >= 0; p--)
    {
        if (is_prefix(pat, patlen, p + 1))
        {
            last_prefix_index = p + 1;
        }
        delta2[p] = last_prefix_index + (patlen - 1 - p);
    }

    // second loop
    for (p = 0; p < patlen - 1; p++)
    {
        int slen = suffix_length(pat, patlen, p);
        if (pat[p - slen] != pat[patlen - 1 - slen])
        {
            delta2[patlen - 1 - slen] = patlen - 1 - p + slen;
        }
    }
}

int boyer_moore(const unsigned char *string, ssize_t stringlen,
        const unsigned char *pat, ssize_t patlen)
{
    int i;
    int delta1[ALPHABET_LEN];
    //int *delta2 = malloc(patlen * sizeof(int));
    int delta2[patlen];
    //const uint8_t* pat = (const uint8_t*)patstr;
    //const uint8_t* string = (const uint8_t*)str;
    make_delta1(delta1, pat, patlen);
    make_delta2(delta2, pat, patlen);

    i = patlen - 1;
    while (i < stringlen)
    {
        int j = patlen - 1;
        while (j >= 0 && (string[i] == pat[j]))
        {
            --i;
            --j;
        }
        if (j < 0)
        {
            //free(delta2);
            return (i + 1);
        }

        i += max(delta1[string[i]], delta2[j]);
    }
    //free(delta2);
    return -1;
}

/* The constant UCHAR_MAX is assumed to contain the maximum
 * value of the input character type. Typically it's 255.
 * size_t is an unsigned type for representing sizes.
 * If your system doesn't have it, replace with
 * unsigned int.
 */

/* Returns a pointer to the first occurrence of "needle"
 * within "haystack", or NULL if not found. Works like
 * memmem().
 */

/* Note: In this example needle is a Cstring. The ending
 * 0x00 will be cut off, so you could call this example with
 * boyermoore_horspool_memmem(haystack, hlen, "abc", sizeof("abc"))
 */
int boyermoore_horspool(const unsigned char* haystack, ssize_t hlen,
        const unsigned char* needle, ssize_t nlen)
{
    size_t scan = 0;
    size_t bad_char_skip[UCHAR_MAX + 1]; /* Officially called:
     * bad character shift */
    const unsigned char* orignal = haystack;
    /* Sanity checks on the parameters */
    if (nlen <= 0 || !haystack || !needle)
        return -1;

    /* ---- Preprocess ---- */
    /* Initialize the table to default value */
    /* When a character is encountered that does not occur
     * in the needle, we can safely skip ahead for the whole
     * length of the needle.
     */
    for (scan = 0; scan <= UCHAR_MAX; scan = scan + 1)
        bad_char_skip[scan] = nlen;

    /* C arrays have the first byte at [0], therefore:
     * [nlen - 1] is the last byte of the array. */
    size_t last = nlen - 1;

    /* Then populate it with the analysis of the needle */
    for (scan = 0; scan < last; scan = scan + 1)
        bad_char_skip[needle[scan]] = last - scan;

    /* ---- Do the matching ---- */

    /* Search the haystack, while the needle can still be within it. */
    while (hlen >= nlen)
    {
        /* scan from the end of the needle */
        for (scan = last; haystack[scan] == needle[scan]; scan = scan - 1)
            if (scan == 0) /* If the first byte matches, we've found it. */
                return haystack - orignal;

        /* otherwise, we need to skip some bytes and start again.
         Note that here we are getting the skip value based on the last byte
         of needle, no matter where we didn't match. So if needle is: "abcd"
         then we are skipping based on 'd' and that value will be 4, and
         for "abcdd" we again skip on 'd' but the value will be only 1.
         The alternative of pretending that the mismatched character was
         the last character is slower in the normal case (Eg. finding
         "abcd" in "...azcd..." gives 4 by using 'd' but only
         4-2==2 using 'z'. */
        hlen -= bad_char_skip[haystack[last]];
        haystack += bad_char_skip[haystack[last]];
    }

    return -1;
}

static inline int bitap_bitwise_search(const unsigned char *text, size_t len,
        const unsigned char *pattern, size_t patlen)
{
    int m = patlen;
    unsigned long R;
    unsigned long pattern_mask[CHAR_MAX + 1];
    int i;

    if (pattern[0] == '\0')
        return 0;
    if (m > 31)
        //return "The pattern is too long!";
        return -1;

    /* Initialize the bit array R */
    R = ~1;

    /* Initialize the pattern bitmasks */
    for (i = 0; i <= CHAR_MAX; ++i)
        pattern_mask[i] = ~0;
    for (i = 0; i < m; ++i)
        pattern_mask[pattern[i]] &= ~(1UL << i);

    for (i = 0; i < len; ++i)
    {
        /* Update the bit array */
        R |= pattern_mask[text[i]];
        R <<= 1;

        if (0 == (R & (1UL << m)))
            return (i - m) + 1;
    }

    return -1;
}
typedef char BIT; /* needs only to hold the values 0 and 1 */
int bitap_search(const unsigned char *text, size_t len,
        const unsigned char *pattern, size_t patlen)
{
    if (patlen < 32)
    {
        return bitap_bitwise_search(text, len, pattern, patlen);
    }
    int result = -1;
    //int m = strlen(pattern);
    int m = patlen;
    //BIT *R;
    int i, k;

    if (pattern[0] == '\0')
        return 0;

    /* Initialize the bit array R */
    //R = malloc((m+1) * sizeof *R);
    BIT R[m + 1];
    R[0] = 1;
    memset(R + 1, 0, m);
    //for (k = 1; k <= m; ++k)
    //    R[k] = 0;

    for (i = 0; i < len; ++i)
    {
        /* Update the bit array. */
        for (k = m; k >= 1; --k)
            R[k] = R[k - 1] && (text[i] == pattern[k - 1]);

        if (R[m])
        {
            result = (i - m) + 1;
            break;
        }
    }

    //free(R);
    return result;
}

static inline int hash(const unsigned char *s, unsigned int count)
{
    int h = 0;
    int off = 0;
    int i = 0;
    for (; i < count; i++)
    {
        h = 31 * h + s[off++];
    }
    return h;
}

int robin_karp_search(const unsigned char *text, size_t text_len,
        const unsigned char *find, size_t find_len)
{
    if (text_len == 0 || find_len == 0)
        return -1;
    int i, j;
    //int text_len = strlen(text);
    //int find_len = strlen(find);
    if (text_len < find_len)
        return -1;
    int h_find = hash(find, find_len);
    int h_tmp = 0;
    for (i = 0; i <= (text_len - find_len); i++)
    {
        h_tmp = hash(text + i, find_len);
        if (h_tmp == h_find)
        {
            for (j = 0; j < find_len; j++)
            {
                if (find[j] != text[i + j])
                {
                    break;
                }
            }
            if (j == find_len)
                return i;
        }
    }
    return -1;
}

int sunday_search(const unsigned char *text, size_t text_len,
        const unsigned char *find, size_t find_len)
{
    if (text_len == 0 || find_len == 0)
        return -1;
    char map[CHAR_MAX];
    int i;
    //int text_len = strlen(text);
    //int find_len = strlen(find);
    if (text_len < find_len)
        return -1;
    //preprocess
    for (i = 0; i < CHAR_MAX; i++)
        map[i] = find_len + 1;
    for (i = 0; i < find_len; i++)
        map[find[i]] = find_len - i;
    //match process
    i = 0;
    while (i <= (text_len - find_len))
    {
        if (memcmp(find, text + i, find_len) == 0)
            return i;
        else
            i += map[text[i + find_len]];
    }
    return -1;
}

int railgun_quadruplet_search(const unsigned char * pbTarget,
        size_t cbTarget, const unsigned char * pbPattern, size_t cbPattern)
{
    const unsigned char * original = pbTarget;
    const unsigned char * pbTargetMax = pbTarget + cbTarget;
    register unsigned long ulHashPattern;
    unsigned long ulHashTarget;
    signed long count;
    signed long countSTATIC;

    unsigned char SINGLET;
    unsigned long Quadruplet2nd;
    unsigned long Quadruplet3rd;
    unsigned long Quadruplet4th;
    unsigned long AdvanceHopperGrass;

    long i;
    int a, j, bm_bc[CHAR_MAX];
    //unsigned char ch;

    if (cbPattern > cbTarget)
        return -1;

    if (cbPattern < 4)
    {
        pbTarget = pbTarget + cbPattern;
        ulHashPattern = ((*(char *) (pbPattern)) << 8)
                + *(pbPattern + (cbPattern - 1));
        if (cbPattern == 3)
        {
            for (;;)
            {
                if (ulHashPattern
                        == ((*(char *) (pbTarget - 3)) << 8) + *(pbTarget - 1))
                {
                    if (*(char *) (pbPattern + 1) == *(char *) (pbTarget - 2))
                        return ((pbTarget - original - 3));
                }
                if ((char) (ulHashPattern >> 8) != *(pbTarget - 2))
                    pbTarget++;
                pbTarget++;
                if (pbTarget > pbTargetMax)
                    return -1;
            }
        }
        else
        {
        }
        for (;;)
        {
            if (ulHashPattern
                    == ((*(char *) (pbTarget - 2)) << 8) + *(pbTarget - 1))
                return ((pbTarget -original- 2));
            if ((char) (ulHashPattern >> 8) != *(pbTarget - 1))
                pbTarget++;
            pbTarget++;
            if (pbTarget > pbTargetMax)
                return -1;
        }
        return -1;
    }
    else
    {
        if (cbTarget < 961)
        {
            pbTarget = pbTarget + cbPattern;
            ulHashPattern = *(unsigned long *) (pbPattern);
            SINGLET = ulHashPattern & 0xFF;
            Quadruplet2nd = SINGLET << 8;
            Quadruplet3rd = SINGLET << 16;
            Quadruplet4th = SINGLET << 24;
            for (;;)
            {
                AdvanceHopperGrass = 0;
                ulHashTarget = *(unsigned long *) (pbTarget - cbPattern);
                if (ulHashPattern == ulHashTarget)
                {
                    count = cbPattern - 1;
                    while (count
                            && *(char *) (pbPattern + (cbPattern - count))
                                    == *(char *) (pbTarget - count))
                    {
                        if (cbPattern - 1 == AdvanceHopperGrass + count
                                && SINGLET != *(char *) (pbTarget - count))
                            AdvanceHopperGrass++;
                        count--;
                    }
                    if (count == 0)
                        return ((pbTarget - original- cbPattern));
                }
                else
                {
                    if (Quadruplet2nd != (ulHashTarget & 0x0000FF00))
                    {
                        AdvanceHopperGrass++;
                        if (Quadruplet3rd != (ulHashTarget & 0x00FF0000))
                        {
                            AdvanceHopperGrass++;
                            if (Quadruplet4th != (ulHashTarget & 0xFF000000))
                                AdvanceHopperGrass++;
                        }
                    }
                }
                AdvanceHopperGrass++;
                pbTarget = pbTarget + AdvanceHopperGrass;
                if (pbTarget > pbTargetMax)
                    return -1;
            }
        }
        else
        {
            countSTATIC = cbPattern - 2 - 2;
            ulHashPattern = *(unsigned long *) (pbPattern);
            for (a = 0; a < CHAR_MAX; a++)
                bm_bc[a] = cbPattern;
            for (j = 0; j < cbPattern - 1; j++)
                bm_bc[pbPattern[j]] = cbPattern - j - 1;
            i = 0;
            while (i <= cbTarget - cbPattern)
            {
                if (*(unsigned long *) &pbTarget[i] == ulHashPattern)
                {
                    count = countSTATIC;
                    while (count != 0
                            && *(char *) (pbPattern + 1 + 3
                                    + (countSTATIC - count))
                                    == *(char *) (&pbTarget[i] + 1 + 3
                                            + (countSTATIC - count)))
                        count--;
                    if (count == 0)
                        return (pbTarget - original + i);
                }
                i = i + bm_bc[(unsigned char) pbTarget[i + cbPattern - 1]];
            }
            return -1;
        }
    }
    return -1;
}

