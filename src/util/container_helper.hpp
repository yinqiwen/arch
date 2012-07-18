/*
 * container_helper.hpp
 *
 *  Created on: 2012-6-2
 *      Author: wqy
 */

#ifndef CONTAINER_HELPER_HPP_
#define CONTAINER_HELPER_HPP_

#include "common/base.hpp"
#include <vector>

namespace arch
{
	namespace util
	{
		template<typename T>
		class VectorSelector
		{
			private:
				std::vector<T> m_container;
				typedef T					 value_type;
				uint32 m_cursor;
			public:
				VectorSelector() :
						m_cursor(0)
				{
				}
				VectorSelector(const std::vector<value_type>& vec) :
						m_container(vec), m_cursor(0)
				{
				}
				void Add(const value_type& e)
				{
					m_container.push_back(e);
				}
				void AddAll(const std::vector<value_type>& vec)
				{
					typename std::vector<value_type>::const_iterator cit = vec.begin();
					while (cit != vec.end())
					{
						Add(*cit);
						cit++;
					}
				}

				uint32 Size()
				{
					return m_container.size();
				}
				value_type& Select()
				{
					if (m_cursor >= Size())
					{
						m_cursor = 0;
					}
					return m_container[m_cursor++];
				}

		};
	}
}

#endif /* CONTAINER_HELPER_HPP_ */
