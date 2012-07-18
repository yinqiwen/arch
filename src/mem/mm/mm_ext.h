/*
 * mm_ext.h
 *
 *  Created on: 2011-3-15
 *      Author: qiyingwang
 */

#ifndef MM_EXT_H_
#define MM_EXT_H_

#include "mem/mm/mm.h"

#ifdef __cplusplus
extern "C"
{
#endif

    MM *mm_create_with_shmid(int shmid);
    int mm_get_shmid(MM *);
    long long mm_offset(MM*, void*);

#ifdef __cplusplus
}
#endif

#endif /* MM_EXT_H_ */
