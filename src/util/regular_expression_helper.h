/*
 * regular_expression_helper.h
 * Copyright (C) Rob Pike
 *
 *  Created on: 2011-7-5
 *      port by: qiyingwang
 */

#ifndef REGULAR_EXPRESSION_HELPER_H_
#define REGULAR_EXPRESSION_HELPER_H_

#ifdef __cplusplus
extern "C" {
#endif
/**
 *  c    matches any literal character c
    .    matches any single character
    ^    matches the beginning of the input string
    $    matches the end of the input string
    *    matches zero or more occurrences of the previous character

    return 1:matched, 0:not matched
 */

int regular_expression_match(const char *regexp, const char *text);

#ifdef __cplusplus
}
#endif
#endif /* REGULAR_EXPRESSION_HELPER_H_ */
