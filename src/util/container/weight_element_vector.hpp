/*
 * weight_element_vector.hpp
 *
 *  Created on: 2012-6-30
 *      Author: wqy
 */

#ifndef WEIGHT_ELEMENT_VECTOR_HPP_
#define WEIGHT_ELEMENT_VECTOR_HPP_

#include "common/base.hpp"
#include <algorithm>
#include <vector>
namespace arch
{
	namespace util
	{
		template<typename T>
		struct WeightElement
		{
				T e;
				uint32 weight;
				bool operator<(const WeightElement& other)
				{
					return weight < other.weight;
				}
				WeightElement() :
						weight(0)
				{
				}
		};

//		template<typename T>
//		typedef std::vector<WeightElement<T> > WeightElementArray;

		template<typename T>
		struct WeightElementArrayHolder
		{
				typedef T value_type;
				typedef WeightElement<value_type> Element;
				typedef std::vector<WeightElement<value_type> > WeightElementArray;
				WeightElementArray array;
				void Add(const Element& id)
				{
					if (array.empty())
					{
						array.push_back(id);
					}
					else
					{
						Erase(id);
						typename WeightElementArray::iterator vit =
						        std::lower_bound(array.begin(), array.end(),
						                id);
						if (vit == array.end())
						{
							array.push_back(id);
						}
						else
						{
							array.insert(vit, id);
						}
					}
				}
				bool Erase(const Element& id)
				{
					typename WeightElementArray::iterator it = array.begin();
					while (it != array.end())
					{
						if (it->e == id.e)
						{
							array.erase(it);
							return true;
						}
						it++;
					}
					return false;
				}
		};
	}
}

#endif /* WEIGHT_ELEMENT_VECTOR_HPP_ */
