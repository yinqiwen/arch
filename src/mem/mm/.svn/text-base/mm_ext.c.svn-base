/*
 * mm_ext.c
 *
 *  Created on: 2011-3-15
 *      Author: qiyingwang
 */
#define MM_PRIVATE
#include "mem/mm/mm_ext.h"


MM *mm_create_with_shmid(int shmid)
{
#if defined(MM_SHMT_IPCSHM)
    mem_core *mc;
    size_t size;
    void *area = ((void *) -1);
    struct shmid_ds shmbuf;
    if (shmctl(shmid, IPC_STAT, &shmbuf) == -1)
    {
        FAIL(MM_ERR_CORE|MM_ERR_SYSTEM, "failed to get status of shared memory");
    }
    //Just init static  lock/unlock structures for semop() which defined in mm_core.c
    mm_core_create(0, NULL);

    size = shmbuf.shm_segsz;
    if ((area = (void *) shmat(shmid, NULL, 0)) == ((void *) -1))
        FAIL(MM_ERR_CORE|MM_ERR_SYSTEM, "failed to attach shared memory");

    mc = (mem_core *) area;
    if(mc->mc_size != size)
    {
        FAIL(MM_ERR_CORE|MM_ERR_SYSTEM, "invalid shmid which is not created by mm_create");
    }
    return (MM *) mc;
#endif /* MM_SHMT_IPCSHM */
    BEGIN_FAILURE
#if defined(MM_SHMT_IPCSHM)
if    (area != ((void *)-1))
    shmdt(area);
#endif
    return NULL;
}

int mm_get_shmid(MM * mm)
{
    mem_core* core = (mem_core*) mm;
    return core->mc_fdmem;
}

long long mm_offset(MM* mm, void* obj)
{
    unsigned long long objaddr = (unsigned long long)obj;
    unsigned long long baseaddr = (unsigned long long)mm;
    if(baseaddr < objaddr)
    {
        return objaddr-baseaddr;
    }
    return -1;
}
