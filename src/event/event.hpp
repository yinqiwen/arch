/*
 * event.hpp
 *
 *  Created on: 2011-6-9
 *      Author: qiyingwang
 */

#ifndef RTA_EVENT_HPP_
#define RTA_EVENT_HPP_

#include "common/base.hpp"
#include "buffer/buffer.hpp"
#include "buffer/buffer_helper.hpp"
#include "logging/logger_macros.hpp"
#include <map>
#include <utility>
#include <set>

using namespace arch::buffer;
using std::pair;
using namespace std;

namespace arch
{
	namespace event
	{
		struct EventHeader
		{
				uint32 type;
				uint32 version;
				uint32 hash;
				EventHeader()
						: type(0), hash(0), version(0)
				{
				}
				bool Encode(Buffer* buf)
				{
					RETURN_FALSE_IF_NULL(buf);
					return BufferHelper::WriteFixUInt32(*buf, type)
							&& BufferHelper::WriteFixUInt32(*buf, version)
							&& BufferHelper::WriteFixUInt32(*buf, hash);
				}
				bool Decode(Buffer* buf)
				{
					RETURN_FALSE_IF_NULL(buf);
					return BufferHelper::ReadFixUInt32(*buf, type)
							&& BufferHelper::ReadFixUInt32(*buf, version)
							&& BufferHelper::ReadFixUInt32(*buf, hash);
				}
		};

		template<typename T>
		struct Event
		{
			public:
				EventHeader header;
				T value;
			public:
				void FillHeader()
				{
					header.type = T::Type;
					header.version = T::Version;
				}
				bool Encode(Buffer* buf, bool hasIntHeader = true)
				{
					RETURN_FALSE_IF_NULL(buf);
					FillHeader();
					uint32 writeIndex = buf->GetWriteIndex();
					if (hasIntHeader)
					{
						BufferHelper::WriteFixUInt32(*buf, 1);
					}
					if (header.Encode(buf) && value.Encode(buf))
					{
						if (hasIntHeader)
						{
							uint32 curWriteIndex = buf->GetWriteIndex();
							uint32 len = curWriteIndex - writeIndex;
							buf->SetWriteIndex(writeIndex);
							BufferHelper::WriteFixUInt32(*buf, len);
							buf->SetWriteIndex(curWriteIndex);
						}
						return true;
					}
					return false;
				}

				T& GetValue()
				{
					return value;
				}
		};

		/// A tag class emphasises the objects are event handler.
		/// The empty base class optimization applies.
		/// Any derived class of EventHandlerTag should be a event handler.
		struct EventHandlerTag
		{
				virtual ~EventHandlerTag()
				{
				}
		};

		template<typename T>
		struct EventHandler: public EventHandlerTag
		{
				virtual void OnEvent(EventHeader& header, T& event,
						void* data) = 0;
				virtual ~EventHandler()
				{
				}
		};

		typedef pair<uint32, uint32> EventHandlerEntryKey;

		struct EventHandlerKeyComp
		{
				bool operator()(const EventHandlerEntryKey& lhs,
						const EventHandlerEntryKey& rhs) const
				{
					if (lhs.first < rhs.first)
					{
						return true;
					}
					if (lhs.first > rhs.first)
					{
						return false;
					}
					return lhs.second < rhs.second;
				}
		};

		class EventParser
		{
			public:
				typedef std::set<EventHandlerTag*> RegisterHandlerSet;
			private:
				typedef bool ParseCallBack(EventHeader& header,
						RegisterHandlerSet& handlers, Buffer* buf, void* data);
				typedef pair<EventHandlerTag*, ParseCallBack*> RegisterEntryValueItem;

				typedef pair<ParseCallBack*, RegisterHandlerSet> RegisterEntryValue;
				typedef std::map<EventHandlerEntryKey, RegisterEntryValue,
						EventHandlerKeyComp> EventHandlerTable;
				EventHandlerTable m_handler_table;
				RegisterEntryValue* GetRegisterEntryValue(uint32 type,
						uint32 ver);
			public:
				template<typename T>
				bool RegisterHandler(EventHandler<T>* handler)
				{
					RETURN_FALSE_IF_NULL(handler);
					EventHandlerEntryKey key = std::make_pair(T::Type,
							T::Version);
					EventHandlerTable::iterator found = m_handler_table.find(
							key);
					if (found != m_handler_table.end())
					{
						if (found->second.first != T::OnParse)
						{
							return false;
						}
						return found->second.second.insert(handler).second;
					}
					RegisterHandlerSet handlerSet;
					handlerSet.insert(handler);
					RegisterEntryValue value = make_pair(T::OnParse,
							handlerSet);
					m_handler_table.insert(std::make_pair(key, value));
					return true;
				}

				template<typename T>
				bool FireEvent(Event<T>& ev, void* data)
				{
					RegisterEntryValue* entry = GetRegisterEntryValue(
							ev.header.type, ev.header.version);
					if (NULL == entry)
					{
						WARN_LOG(
								"No parser callback entry found for %u:%u.", ev.header.type, ev.header.version);
						return false;
					}
					EventParser::RegisterHandlerSet::iterator it =
							entry->second.begin();
					while (it != entry->second.end())
					{
						ex::event::EventHandler<T>* h =
								static_cast<arch::event::EventHandler<T>*>(*it);
						if (NULL != h)
						{
							h->OnEvent(ev.header, ev.value, data);
						}
						it++;
					}
					return true;
				}

				bool Parse(Buffer* buf, void* data = NULL);
				static bool PeekHeader(Buffer* buf, EventHeader& header);
		};

	}
}

#define DECL_EVENT_TYPE_VERSION(T, __type__, __ver__)  \
	public:                                    \
	    static const uint32 Type = __type__; \
	    static const uint32 Version = __ver__;  \
	    static bool OnParse(arch::event::EventHeader& header,                        \
	            EventParser::RegisterHandlerSet& handlers, Buffer* buf, void* data) \
	    {                                                                        \
	    	if(header.type != Type || header.version != Version)                 \
	    	{                                                                    \
	    		ERROR_LOG("Invalid type:version() for Type:Version",             \
                             header.type, header.version, Type, Version);        \
	    		return false;                                                    \
	    	}                                                                    \
	    	T instance;                                                           \
	    	if(!instance.Decode(buf))                                             \
            {                                                                     \
	    	    ERROR_LOG("Failed to decode event in Type:Version",Type, Version);\
                return false;                                                     \
            }                                                                     \
	    	EventParser::RegisterHandlerSet::iterator it = handlers.begin();      \
	    	while(it != handlers.end())                                           \
	    	{                                                                     \
	    	  arch::event::EventHandler<T>* h = static_cast<arch::event::EventHandler<T>* >(*it);          \
	    	  if(NULL != h)                                                         \
	    	  {                                                                     \
	    		   h->OnEvent(header, instance, data);                             \
	    	  }                                                                     \
              it++;                                                                 \
	    	}                                                                       \
	    	return true;                                                            \
	    }

#endif /* EVENT_HPP_ */
