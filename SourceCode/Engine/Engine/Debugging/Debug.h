// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\PrimitiveTypes.h>

#ifndef DEBUG_H
#define DEBUG_H

namespace Engine
{
	using namespace Common;

	namespace Debugging
	{
		class DEBUGGING_API Debug
		{
		public:
			static void Assertion(bool Condition, cstr ConditionText, cstr Message, cstr File, uint32 LineNumber, cstr Function);
		};

		// __FUNCDNAME__		??0CustomAllocator@Allocator@MemoryManagement@Engine@@IEAA@PEBDPEAVAllocatorBase@123@_K@Z
		// __FUNCSIG__			__cdecl Engine::MemoryManagement::Allocator::CustomAllocator::CustomAllocator(const char *,class Engine::MemoryManagement::Allocator::AllocatorBase *,unsigned __int64)
		// __FUNCTION__			Engine::MemoryManagement::Allocator::CustomAllocator::CustomAllocator
		// __func__				CustomAllocator
		// __PRETTY_FUNCTION__	Doesn't declared

#if DEBUG_MODE

#define DEBUG_ARGUMENTS __FILE__, __LINE__, __FUNCSIG__

#define Assert(Condition, Message) \
	Engine::Debugging::Debug::Assertion(Condition, #Condition, Message, DEBUG_ARGUMENTS)

#define STATIC_ASSERT(Condition, Message) \
	static_assert(Condition, Message)

#else

#define Assert(Condition, Message) (Condition)

#define STATIC_ASSERT(Condition, Message)

#endif
	}
}

#endif