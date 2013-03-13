/*
 * event.cpp
 *
 *  Created on: 2011-6-9
 *      Author: wqy
 */
#include "event/event.hpp"
#include "util/string_helper.hpp"

using namespace arch::event;
using namespace arch::util;

//EventParser::EventHandlerTable EventParser::m_handler_table;

EventParser::RegisterEntryValue* EventParser::GetRegisterEntryValue(
        uint32 type, uint32 ver)
{
    EventHandlerEntryKey key = std::make_pair(type, ver);
    if (m_handler_table.count(key) == 0)
    {
        return NULL;
    }
    return &(m_handler_table[key]);
}

bool EventParser::PeekHeader(Buffer* buf, EventHeader& header)
{
    RETURN_FALSE_IF_NULL(buf);
    size_t mark = buf->GetReadIndex();
    bool ret = false;
    if (!header.Decode(buf))
    {
        ret = false;
    }
    else
    {
        ret = true;
    }
    buf->SetReadIndex(mark);
    return ret;
}

bool EventParser::Parse(Buffer* buf, void* data)
{
    RETURN_FALSE_IF_NULL(buf);
    EventHeader header;
    uint32 bufsize = buf->ReadableBytes();
    if (!header.Decode(buf))
    {
        ERROR_LOG("Failed to decode event header while buffer size:%u", bufsize);
        return false;
    }
    RegisterEntryValue* entry = GetRegisterEntryValue(header.type,
            header.version);
    if (NULL == entry)
    {
        ERROR_LOG("No parser callback entry found for %u:%u.", header.type,
                header.version);
        return false;
    }

    //(*(entry->second))(header, entry->first, buf);
    return entry->first(header, entry->second, buf, data);
}

