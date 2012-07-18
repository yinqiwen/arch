/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define to 1 if you have the `bcopy' function. */
#define HAVE_BCOPY 1

/* define to 1 if you have the B_PAGE_SIZE define */
/* #undef HAVE_B_PAGE_SIZE */

/* define to 1 if you have the CHILD_MAX define */
/* #undef HAVE_CHILD_MAX */

/* Define to 1 if you have the `create_area' function. */
/* #undef HAVE_CREATE_AREA */

/* Define to 1 if you have the `create_sem' function. */
/* #undef HAVE_CREATE_SEM */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* define to 1 if you have the F_SETLK define */
#define HAVE_F_SETLK 1

/* Define to 1 if you have the `getpagesize' function. */
#define HAVE_GETPAGESIZE 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* define to 1 if you have the IPC_PRIVATE define */
#define HAVE_IPC_PRIVATE 1

/* Define to 1 if you have the <kernel/OS.h> header file. */
/* #undef HAVE_KERNEL_OS_H */

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* define to 1 if you have the LOCK_EX define */
#define HAVE_LOCK_EX 1

/* define to 1 if you have the MAP_ANON define */
#define HAVE_MAP_ANON 1

/* define to 1 if you have the MAXPATHLEN define */
#define HAVE_MAXPATHLEN 1

/* Define to 1 if you have the `memcpy' function. */
#define HAVE_MEMCPY 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `memset' function. */
#define HAVE_MEMSET 1

/* Define to 1 if you have the `mmap' function. */
#define HAVE_MMAP 1

/* Define to 1 if you have the `munmap' function. */
#define HAVE_MUNMAP 1

/* define to 1 if you have the PATH_MAX define */
#define HAVE_PATH_MAX 1

/* Define to 1 if you have the `semctl' function. */
#define HAVE_SEMCTL 1

/* Define to 1 if you have the `semget' function. */
#define HAVE_SEMGET 1

/* define to 1 if you have the SEM_UNDO define */
#define HAVE_SEM_UNDO 1

/* Define to 1 if you have the `shmat' function. */
#define HAVE_SHMAT 1

/* Define to 1 if you have the `shmctl' function. */
#define HAVE_SHMCTL 1

/* Define to 1 if you have the `shmdt' function. */
#define HAVE_SHMDT 1

/* Define to 1 if you have the `shmget' function. */
#define HAVE_SHMGET 1

/* Define to 1 if you have the `shm_open' function. */
/* #undef HAVE_SHM_OPEN */

/* Define to 1 if you have the `shm_unlink' function. */
/* #undef HAVE_SHM_UNLINK */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `sysconf' function. */
#define HAVE_SYSCONF 1

/* Define to 1 if you have the <sys/file.h> header file. */
#define HAVE_SYS_FILE_H 1

/* Define to 1 if you have the <sys/ipc.h> header file. */
#define HAVE_SYS_IPC_H 1

/* Define to 1 if you have the <sys/mman.h> header file. */
#define HAVE_SYS_MMAN_H 1

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/sem.h> header file. */
#define HAVE_SYS_SEM_H 1

/* Define to 1 if you have the <sys/shm.h> header file. */
#define HAVE_SYS_SHM_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* define to 1 if you have union semun */
/* #undef HAVE_UNION_SEMUN */

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* define to 1 if you have the _POSIX_CHILD_MAX define */
#define HAVE__POSIX_CHILD_MAX 1

/* define to 1 if you have the _POSIX_PATH_MAX define */
#define HAVE__POSIX_PATH_MAX 1

/* define to 1 if you have the _SC_PAGESIZE define */
#define HAVE__SC_PAGESIZE 1

/* define to enable debugging */
/* #undef MM_DEBUG */

/* define to 1 for BeOS support */
/* #undef MM_OS_BEOS */

/* define to 1 for BS2000 support */
/* #undef MM_OS_BS2000 */

/* define to 1 for SunOS 4.x support */
/* #undef MM_OS_SUNOS */

/* mutex implementation method: BeOS semaphores */
/* #undef MM_SEMT_BEOS */

/* mutex implementation method: SVR4-style fcntl() on temporary file */
/* #undef MM_SEMT_FCNTL */

/* mutex implementation method: 4.2BSD-style flock() on temporary file */
/* #undef MM_SEMT_FLOCK */

/* mutex implementation method: SysV IPC semget() */
#define MM_SEMT_IPCSEM 1

/* shared memory allocation method: BeOS areas */
/* #undef MM_SHMT_BEOS */

/* shared memory allocation method: SysV IPC shmget() */
#define MM_SHMT_IPCSHM 1

/* shared memory allocation method: 4.4BSD-style mmap() via MAP_ANON */
/* #undef MM_SHMT_MMANON */

/* shared memory allocation method: Classical mmap() on temporary file */
/* #undef MM_SHMT_MMFILE */

/* shared memory allocation method: mmap() via POSIX.1 shm_open() on temporary
   file */
/* #undef MM_SHMT_MMPOSX */

/* shared memory allocation method: SVR4-style mmap() on /dev/zero */
/* #undef MM_SHMT_MMZERO */

/* maximum segment size */
#define MM_SHM_MAXSEGSIZE 33554432

/* memory page size determination: BeOS B_PAGE_SIZE */
/* #undef MM_VMPS_BEOS */

/* memory page size determination: 4.2BSD getpagesize() */
/* #undef MM_VMPS_GETPAGESIZE */

/* memory page size determination: POSIX.1 sysconf(_SC_PAGESIZE) */
#define MM_VMPS_SYSCONF 1

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1
