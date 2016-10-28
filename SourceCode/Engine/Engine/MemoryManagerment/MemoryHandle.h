// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <MemoryManagerment\HandleInfo.h>
#include <Platform\Memory.h>

#ifndef MEMORY_HANDLE_H
#define MEMORY_HANDLE_H

namespace Engine
{
	using namespace Platform;

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
				Memory::Copy(&Value, m_Info->Address, sizeof(T));
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