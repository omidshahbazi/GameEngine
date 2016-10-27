// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\HandleInfo.h>

#ifndef MEMORY_HANDLE_H
#define MEMORY_HANDLE_H

namespace Engine
{
	namespace MemoryManagement
	{
		template <typename T> class MemoryHandle
		{
			//friend class Allocator;

		private:
			HandleInfo *m_Info;

		private:
			MemoryHandle(HandleInfo *Info) :
				m_Info(Info)
			{
				m_Info->Grab();
			}

		public:
			~MemoryHandle(void)
			{
				m_Info->Drop();
			}

			MemoryHandle(const MemoryHandle<T> &Other) :
				m_Info(Other.m_Info)
			{
				m_Info->Grab();
			}

		public:
			//T &Get(void)
			//{
			//	return *(T*)(m_Info->Address);
			//}

			//const T &Get(void) const
			//{
			//	return *(T*)(m_Info->Address);
			//}

			void operator = (const T &Value)
			{
				memcpy(m_Info->Address, &Value, sizeof(T));
			}

			void operator = (const MemoryHandle<T> &Other)
			{
				// drop prev if available
				m_Info = Other.m_Info;
				m_Info->Grab();
			}
		};
	}
}

#endif