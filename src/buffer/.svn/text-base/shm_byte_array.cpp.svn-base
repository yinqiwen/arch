/*
 * shm_byte_array.cpp
 *
 *  Created on: 2011-3-24
 *      Author: qiyingwang
 */
#include "buffer/shm_byte_array.hpp"
#include "exception/exception.hpp"

using namespace arch::buffer;
using namespace arch::exception;

SHMByteArray::SHMByteArray(SharedMemoryPool* mm, size_t size) :
    m_content(NULL), m_len(0), m_pool(mm)
{
    m_content = (char*) mm->Malloc(size);
    if (NULL == m_content)
    {
        throw Exception("Can not malloc memory from shared memory pool!");
    }
    m_len = size;
}

uint32 SHMByteArray::Read(Buffer* buf)
{
    buf->Write(m_content, m_len);
    return m_len;
}

uint32 SHMByteArray::Write(Buffer* buf)
{
    size_t expected = buf->ReadableBytes();
    if (expected > m_len)
    {
        expected = m_len;
    } else
    {
        m_len = expected;
    }
    buf->Read(m_content, expected);
    return expected;
}

SHMByteArray::~SHMByteArray()
{
    //mm_free(m_pool, m_buffer);
    m_pool->Free(m_content);
}

SHMByteArray* SHMByteArrayFIFO::Take()
{
    if (NULL == head)
    {
        ASSERT(0 == m_size);
        return NULL;
    }
    SHMByteArray* result = head->array;
    SHMByteArrayNode* tmp = head;
    head = head->next;
    m_pool->Free(tmp);
    m_size--;
    if (0 == m_size)
    {
        ASSERT(head == NULL);
        tail = NULL;
    }
    return result;
}
bool SHMByteArrayFIFO::Offer(SHMByteArray* array)
{
    void* mem = m_pool->Malloc(sizeof(SHMByteArrayNode));
    if (NULL == mem)
    {
        return false;
    }
    memset(mem, 0, sizeof(SHMByteArrayNode));
    SHMByteArrayNode* node = (SHMByteArrayNode*) mem;
    node->array = array;
    node->next = NULL;
    if (NULL == tail)
    {
        ASSERT(head == NULL);
        head = tail = node;
    } else
    {
        ASSERT(head != NULL);
        tail->next = node;
        tail = node;
    }
    m_size++;
    return true;
}

void SHMByteArrayFIFO::Clear()
{
    SHMByteArrayNode* next = head;
    while (next != NULL)
    {
        SHMByteArrayNode* tmp = next;
        next = next->next;
        m_pool->Free(tmp->array);
        m_pool->Free(tmp);
    }
    head = tail = NULL;
    m_size = 0;
}

SHMByteArrayFIFO::~SHMByteArrayFIFO()
{
    Clear();
}
