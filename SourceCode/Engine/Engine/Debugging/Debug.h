// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef DEBUG_H
#define DEBUG_H

#include <Common\PrimitiveTypes.h>
#include <functional>
#include <memory>

namespace Engine
{
	using namespace Common;

	namespace LogSystem
	{
		class Logger;
	}

	using namespace LogSystem;

	namespace Debugging
	{
		//https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019
		class DEBUGGING_API Debug
		{
			friend class Logger;

		private:
			typedef std::function<void(cstr, va_list)> LogCallback;
			typedef std::function<void(cstr, uint32, cstr, cstr, cstr, va_list)> AssertionFailedCallback;

		public:
			static void Print(cstr Message, ...);

			static void LogInfo(cstr Message, ...);
			static void LogWarning(cstr Message, ...);
			static void LogError(cstr Message, ...);

			static void AssertionFailed(cstr File, uint32 LineNumber, cstr Function, cstr ConditionText, cstr Message, ...);

		private:
			static void SetPrintCallback(const LogCallback& Callback);
			static void SetLogInfoCallback(const LogCallback& Callback);
			static void SetLogWarningCallback(const LogCallback& Callback);
			static void SetLogErrorCallback(const LogCallback& Callback);
			static void SetOnAssertionFailedCallback(const AssertionFailedCallback& Callback);

		private:
			static std::shared_ptr<LogCallback> m_OnPrinCallback;
			static std::shared_ptr<LogCallback> m_OnLogInfoCallback;
			static std::shared_ptr<LogCallback> m_OnLogWarningCallback;
			static std::shared_ptr<LogCallback> m_OnLogErrorCallback;
			static std::shared_ptr<AssertionFailedCallback> m_OnAssertionFailedCallback;
		};

		// __FUNCDNAME__		??0CustomAllocator@Allocator@MemoryManagement@Engine@@IEAA@PEBDPEAVAllocatorBase@123@_K@Z
		// __FUNCSIG__			__cdecl Engine::MemoryManagement::Allocator::CustomAllocator::CustomAllocator(const char *,class Engine::MemoryManagement::Allocator::AllocatorBase *,unsigned __int64)
		// __FUNCTION__			Engine::MemoryManagement::Allocator::CustomAllocator::CustomAllocator
		// __func__				CustomAllocator
		// __PRETTY_FUNCTION__	Doesn't declared

#ifdef DEBUG_MODE

#define DEBUG_ARGUMENTS __FILE__, __LINE__, __FUNCSIG__

#define Assert(Condition, Message, ...) \
	if (!(Condition)) \
		Engine::Debugging::Debug::AssertionFailed(DEBUG_ARGUMENTS, #Condition, Message, __VA_ARGS__)

#else

#define DEBUG_ARGUMENTS 

#define Assert(Condition, Message) (Condition)

#endif
	}
}

#endif