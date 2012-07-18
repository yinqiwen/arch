/*
 * helper.hpp
 *
 *  Created on: 2011-6-16
 *      Author: qiyingwang
 */

#ifndef HELPER_HPP_
#define HELPER_HPP_

template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];
#define arraysize(array) (sizeof(ArraySizeHelper(array)))

#endif /* HELPER_HPP_ */
