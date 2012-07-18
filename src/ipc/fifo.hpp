/*
 * fifo.hpp
 *
 *  Created on: 2010-9-28
 *      Author: wqy
 */

#ifndef NOVA_FIFO_HPP_
#define NOVA_FIFO_HPP_

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include "exception/api_exception.hpp"

using std::string;
using arch::exception::APIException;
namespace arch
{
	namespace ipc
	{
		class FIFO
		{
			protected:
				int m_FD;
				string m_location;
				virtual int GetOpenFlag() = 0;
			public:
				FIFO(const string& location);
				int GetFD();
				int Open();
				int Open(int flag);
				void Close();
				virtual ~FIFO();
		};

		class ReadFIFO: public FIFO
		{
			protected:
				int GetOpenFlag();
			public:
				ReadFIFO(const string& location);
				int Read(void* buf, int buflen);
		};

		class WriteFIFO: public FIFO
		{
			protected:
				int GetOpenFlag();
			public:
				WriteFIFO(const string& location);
				int Write(const void* buf, int buflen);
		};
	}
}

#endif /* FIFO_HPP_ */
