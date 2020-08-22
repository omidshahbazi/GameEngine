// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef DEBUG_H
#define DEBUG_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace Debugging
	{
		//https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019
		class DEBUGGING_API Debug
		{
		public:
			static void Assertion(bool Condition, cstr ConditionText, cstr Message, cstr File, uint32 LineNumber, cstr Function);

			static void Print(cstr Message);

			static void LogInfo(cstr Message);
			static void LogWarning(cstr Message);
			static void LogError(cstr Message);
		};

		// __FUNCDNAME__		??0CustomAllocator@Allocator@MemoryManagement@Engine@@IEAA@PEBDPEAVAllocatorBase@123@_K@Z
		// __FUNCSIG__			__cdecl Engine::MemoryManagement::Allocator::CustomAllocator::CustomAllocator(const char *,class Engine::MemoryManagement::Allocator::AllocatorBase *,unsigned __int64)
		// __FUNCTION__			Engine::MemoryManagement::Allocator::CustomAllocator::CustomAllocator
		// __func__				CustomAllocator
		// __PRETTY_FUNCTION__	Doesn't declared

#ifdef DEBUG_MODE

#define DEBUG_ARGUMENTS __FILE__, __LINE__, __FUNCSIG__

#define Assert(Condition, Message) \
	Engine::Debugging::Debug::Assertion(Condition, #Condition, Message, DEBUG_ARGUMENTS)

#define StaticAssert(Condition, Message) \
	static_assert(Condition, Message)

#define DebugLogError(Message) \
	Engine::Debugging::Debug::LogError(Message)

#else

#define Assert(Condition, Message) (Condition)

#define StaticAssert(Condition, Message)

#define DebugLogError(Message)

#endif
	}
}

#endif