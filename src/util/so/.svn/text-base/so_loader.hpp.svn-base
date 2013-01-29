/*
 * so_loader.hpp
 *
 *  Created on: 2012-4-28
 *      Author: qiyingwang
 */

#ifndef SO_LOADER_HPP_
#define SO_LOADER_HPP_
#include "common/base.hpp"
#include <dlfcn.h>
#include <string>

namespace arch
{
    namespace util
    {
        class SoLoader
        {
            private:
                std::string m_path;
                std::string m_error;
                void* m_handle;
                void SetError(const std::string& error)
                {
                    m_error = error;
                }
            public:
                SoLoader(const std::string& path) :
                        m_path(path), m_error(NULL),m_handle(NULL)
                {
                }
                bool IsOpened();
                int Load(int flag = RTLD_NOW);
                int ReLoad(int flag = RTLD_NOW);
                int Close();
                const std::string& GetSoPath()
                {
                    return m_path;
                }
                const std::string& GetError()
                {
                    return m_error;
                }
                void* GetSymbol(const std::string& symbol);
                ~SoLoader();
        };
    }
}

#endif /* SO_LOADER_HPP_ */
