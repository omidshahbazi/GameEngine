// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include "PrimitiveTypes.h"

#ifndef MEMORY_HANDLE_H
#define MEMORY_HANDLE_H

//namespace Engine::MemoryManagement
//{
	struct HandleInfo
	{
	public:
		HandleInfo(byte *Address, uint32 Size, bool IsFree) :
			Address(Address),
			Size(Size),
			IsFree(IsFree),
			Previous(nullptr),
			Next(nullptr),
			ReferencedCount(0)
		{
		}

		HandleInfo(byte *Address, uint32 Size, bool IsFree, HandleInfo *Previous) :
			Address(Address),
			Size(Size),
			IsFree(IsFree),
			Previous(Previous),
			Next(nullptr),
			ReferencedCount(0)
		{
		}

		HandleInfo(byte *Address, uint32 Size, bool IsFree, HandleInfo *Previous, HandleInfo *Next) :
			Address(Address),
			Size(Size),
			IsFree(IsFree),
			Previous(Previous),
			Next(Next),
			ReferencedCount(0)
		{
		}

		void Grab(void)
		{
			++ReferencedCount;
		}

		void Drop(void)
		{
			--ReferencedCount;
		}

	public:
		byte *Address;
		//std::atomic<void*> Address;
		uint32 Size;
		bool IsFree;
		HandleInfo *Previous;
		HandleInfo *Next;
		uint32 ReferencedCount;
	};

	template <typename T> class MemoryHandle
	{
		friend class Allocator;

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
			m_Info = Other.m_Info;
			m_Info->Grab();
		}
	};
//}

#endif