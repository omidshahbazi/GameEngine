// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef CORE_DEBUG_H
#define CORE_DEBUG_H

#include <Common\PrimitiveTypes.h>
#include <Containers\Exception.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace Debugging
	{
		class DEBUGGING_API CoreDebug
		{
		public:
			static void Print(Categories CategoryFlags, cstr Message, ...);
			static void Print(cstr File, uint32 LineNumber, cstr Function, Categories CategoryFlags, cstr Message, ...);

			static void LogInfo(Categories CategoryFlags, cstr Message, ...);
			static void LogInfo(cstr File, uint32 LineNumber, cstr Function, Categories CategoryFlags, cstr Message, ...);

			static void LogWarning(Categories CategoryFlags, cstr Message, ...);
			static void LogWarning(cstr File, uint32 LineNumber, cstr Function, Categories CategoryFlags, cstr Message, ...);

			static void LogError(Categories CategoryFlags, cstr Message, ...);
			static void LogError(cstr File, uint32 LineNumber, cstr Function, Categories CategoryFlags, cstr Message, ...);

			static void LogException(Categories CategoryFlags, const Exception& Exception);

			static void AssertionFailed(cstr File, uint32 LineNumber, cstr Function, cstr ConditionText, Categories CategoryFlags, cstr Message, ...);
		};

#define CoreDebugPrint(CategoryFlags, Content, ...) Engine::Debugging::CoreDebug::Print(DEBUG_ARGUMENTS, CategoryFlags, Content, __VA_ARGS__)

#define CoreDebugLogInfo(CategoryFlags, Content, ...) Engine::Debugging::CoreDebug::LogInfo(DEBUG_ARGUMENTS, CategoryFlags, Content, __VA_ARGS__)
#define CoreDebugLogWarning(CategoryFlags, Content, ...) Engine::Debugging::CoreDebug::LogWarning(DEBUG_ARGUMENTS, CategoryFlags, Content, __VA_ARGS__)
#define CoreDebugLogError(CategoryFlags, Content, ...) Engine::Debugging::CoreDebug::LogError(DEBUG_ARGUMENTS, CategoryFlags, Content, __VA_ARGS__)

#define CoreDebugLogException(CategoryFlags, Exception) Engine::Debugging::CoreDebug::LogException(CategoryFlags, Exception)

#define CoreDebugAssert(CategoryFlags, Condition, Message, ...) \
	if (!(Condition)) \
		Engine::Debugging::CoreDebug::AssertionFailed(DEBUG_ARGUMENTS, #Condition, CategoryFlags, Message, __VA_ARGS__)
	}
}

#endif