/*
 * so_loader.cpp
 *
 *  Created on: 2012-4-28
 *      Author: qiyingwang
 */
#include "so_loader.hpp"

using namespace arch::util;

bool SoLoader::IsOpened()
{
    return m_handle != NULL;
}

int SoLoader::Load(int flag)
{
    if (IsOpened())
    {
        SetError(m_path + " already opened.");
        return -1;
    }
    m_handle = dlopen(m_path.c_str(), flag);
    if (NULL == m_handle)
    {
        SetError(dlerror());
        return -1;
    }
    return 0;
}

int SoLoader::ReLoad(int flag)
{
    if (0 != Close())
    {
        return -1;
    }
    return Load(flag);
}

void* SoLoader::GetSymbol(const std::string& symbol)
{
    void* func = dlsym(m_handle, symbol.c_str());
    if (NULL == func)
    {
        SetError(dlerror());
    }
    return func;
}

int SoLoader::Close()
{
    if (!IsOpened())
    {
        SetError(m_path + " is not opened.");
        return -1;
    }
    if (0 != dlclose(m_handle))
    {
        SetError(dlerror());
        return -1;
    }
    return 0;
}

SoLoader::~SoLoader()
{
    if (IsOpened())
    {
        Close();
    }
}

