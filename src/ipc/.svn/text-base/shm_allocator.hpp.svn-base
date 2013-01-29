/*
 * shm_allocator.hpp
 *
 *  Created on: 2011-4-7
 *      Author: qiyingwang
 */

#ifndef SHM_ALLOCATOR_HPP_
#define SHM_ALLOCATOR_HPP_

#include "common/base.hpp"
#include "ipc/shm_pool.hpp"
#include <map>
#include <queue>
#include <vector>
#include <set>
#include <list>
#include <deque>

#define  SHM_ALLOCATOR_RESERVERD_DEFAULT_TYPE 0

namespace arch
{
	namespace ipc
	{
		template<typename T, uint32 type>
		class SharedMemoryAllocator
		{
			public:
				typedef T value_type;
				typedef value_type* pointer;
				typedef const value_type* const_pointer;
				typedef value_type& reference;
				typedef const value_type& const_reference;
				typedef std::size_t size_type;
				typedef std::ptrdiff_t difference_type;
				inline explicit SharedMemoryAllocator()
				{
				}
				template<typename U, uint32 other_type>
				inline explicit SharedMemoryAllocator(SharedMemoryAllocator<U,
				        other_type> const&)
				{
				}
				template<typename U>
				struct rebind
				{
						typedef SharedMemoryAllocator<U, type> other;
				};
				pointer address(reference r) const
				{
					return &r;
				}
				const_pointer address(const_reference r) const
				{
					return &r;
				}
				pointer allocate(size_type n, const void* p = 0)
				{
					SharedMemoryPool* pool =
					        SharedMemoryPoolHolder<type>::GetSharedMemoryPool();
					void* ret = pool->Malloc(n * sizeof(value_type));
					if (!ret)
					{
						throw std::bad_alloc();
					}
					return static_cast<pointer> (ret);
				}

				void construct(pointer p, const value_type& x)
				{
					new (p) value_type(x);
				}

				void destroy(pointer p)
				{
					p->~value_type();
				}

				void deallocate(pointer p, size_type n)
				{
					SharedMemoryPool* pool =
					        SharedMemoryPoolHolder<type>::GetSharedMemoryPool();
					pool->Free(p);
				}
				size_type max_size() const
				{
					SharedMemoryPool* pool =
					        SharedMemoryPoolHolder<type>::GetSharedMemoryPool(
					                type);
					return pool->Avaliable();
				}
		};

		template<typename T1, typename T2, uint32 type1, uint32 type2>
		bool operator ==(SharedMemoryAllocator<T1, type1> const &,
		        SharedMemoryAllocator<T2, type2> const &) throw ()
		{
			return type1 == type2;
		}

		template<typename T1, typename T2, uint32 type1, uint32 type2>
		bool operator !=(SharedMemoryAllocator<T1, type1> const &,
		        SharedMemoryAllocator<T2, type2> const &) throw ()
		{
			return type1 != type2;
		}

		typedef std::basic_string<char, std::char_traits<char>,
		        SharedMemoryAllocator<char,
		                SHM_ALLOCATOR_RESERVERD_DEFAULT_TYPE> > SHMString;
		template<class T>
		struct SHMContainer
		{
				typedef std::vector<T, SharedMemoryAllocator<T,
				        SHM_ALLOCATOR_RESERVERD_DEFAULT_TYPE> > vector;
				typedef std::list<T, SharedMemoryAllocator<T,
				        SHM_ALLOCATOR_RESERVERD_DEFAULT_TYPE> > list;
				typedef std::queue<T, SharedMemoryAllocator<T,
				        SHM_ALLOCATOR_RESERVERD_DEFAULT_TYPE> > queue;
				typedef std::set<T, std::less<T>, SharedMemoryAllocator<T,
				        SHM_ALLOCATOR_RESERVERD_DEFAULT_TYPE> > set;
				typedef std::deque<T, SharedMemoryAllocator<T,
				        SHM_ALLOCATOR_RESERVERD_DEFAULT_TYPE> > deque;
		};

		template<class Key, class Value>
		struct SHMMap
		{
				typedef std::map<Key, Value, std::less<Key>,
				        SharedMemoryAllocator<std::pair<Key, Value>,
				                SHM_ALLOCATOR_RESERVERD_DEFAULT_TYPE> > map;
		};
	}
}

#endif /* SHM_ALLOCATOR_HPP_ */
