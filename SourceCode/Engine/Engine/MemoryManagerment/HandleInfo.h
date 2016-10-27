// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\ReferenceCounted.h>

#ifndef HANDLE_INFO
#define HANDLE_INFO

//namespace Engine::MemoryManagement
//{
	template<typename Allocator> class HandleInfo : public ReferenceCounted<Allocator>
	{
	public:
		HandleInfo(byte *Address, uint32 Size, bool IsFree) :
			Address(Address),
			Size(Size),
			IsFree(IsFree),
			Previous(nullptr),
			Next(nullptr)
		{
		}

		HandleInfo(byte *Address, uint32 Size, bool IsFree, HandleInfo *Previous) :
			Address(Address),
			Size(Size),
			IsFree(IsFree),
			Previous(Previous),
			Next(nullptr)
		{
		}

		HandleInfo(byte *Address, uint32 Size, bool IsFree, HandleInfo *Previous, HandleInfo *Next) :
			Address(Address),
			Size(Size),
			IsFree(IsFree),
			Previous(Previous),
			Next(Next)
		{
		}

	public:
		byte *Address;
		//std::atomic<void*> Address;
		uint32 Size;
		bool IsFree;
		HandleInfo *Previous;
		HandleInfo *Next;
	};
//}

#endif