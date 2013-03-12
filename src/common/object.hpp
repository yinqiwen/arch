/*
 * Object.hpp
 *
 *  Created on: 2011-1-12
 *      Author: qiyingwang
 */

#ifndef NOVA_OBJECT_HPP_
#define NOVA_OBJECT_HPP_

#ifndef NULL
#define NULL 0
#endif

namespace arch
{
	namespace common
	{


		/**
		 * Verify type information
		 */
		template<typename InheritType>
		struct InstanceOf
		{
			public:
				bool OK;
				template<typename BaseType>
				inline InstanceOf(const BaseType* ptr) :
					OK(false)
				{
					if (NULL != ptr)
					{
						const InheritType* sp =
						        dynamic_cast<const InheritType*> (ptr);
						OK = (NULL != sp);
					}
				}
		};
		/**
		 * A tag interface
		 */
		class Object
		{
			public:
				virtual ~Object()
				{
				}
		};

		template<typename T>
		struct Type
		{
				typedef void Destructor(T* obj);
		};

		template<typename T>
		void StandardDestructor(T* obj)
		{
			if(NULL != obj)
			{
				delete obj;
			}
		}

	}
}

#endif /* OBJECT_HPP_ */
