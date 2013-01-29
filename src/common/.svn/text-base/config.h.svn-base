/*
 * config.h
 *
 *  Created on: 2011-5-5
 *      Author: qiyingwang
 */

#ifndef ARCH_CONFIG_H_
#define ARCH_CONFIG_H_

#define HAVE_GOOGLE_PROFILER 0
#define HAVE_MYSQL_LIB 1
#define HAVE_SQLITE_LIB 1
#define HAVE_SCHED_YIELD 1

#ifdef __APPLE__
#include <AvailabilityMacros.h>
#endif

/* test for malloc_size() */
#ifdef __APPLE__
#include <malloc/malloc.h>
#define HAVE_MALLOC_SIZE 1
#define redis_malloc_size(p) malloc_size(p)
#endif

/* define redis_fstat to fstat or fstat64() */
#if defined(__APPLE__) && !defined(MAC_OS_X_VERSION_10_6)
#define redis_fstat fstat64
#define redis_stat stat64
#else
#define redis_fstat fstat
#define redis_stat stat
#endif

/* test for backtrace() */
#if defined(__APPLE__) || defined(__linux__)
#define HAVE_BACKTRACE 1
#endif

/* test for polling API */
#ifdef __linux__
#define HAVE_EPOLL 1
#endif

#if (defined(__APPLE__) && defined(MAC_OS_X_VERSION_10_6)) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined (__NetBSD__)
#define HAVE_KQUEUE 1
#endif

/* define aof_fsync to fdatasync() in Linux and fsync() for all the rest */
#ifdef __linux__
#define aof_fsync fdatasync
#else
#define aof_fsync fsync
#endif

#ifdef __linux__
#  include <linux/version.h>
#  if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28)
#define HAVE_ACCEPT4 1
#endif
#  if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,22)
#define HAVE_EVENTFD 1
#define HAVE_SIGNALFD 1
#include <sys/eventfd.h>
#include <sys/signalfd.h>
#endif

#endif

#endif /* CONFIG_H_ */
