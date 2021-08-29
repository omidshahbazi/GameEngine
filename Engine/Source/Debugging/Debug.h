// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef DEBUG_H
#define DEBUG_H

#include <Common\PrimitiveTypes.h>
#include <Containers\Exception.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace Debugging
	{
		class DEBUGGING_API Debug
		{
		public:
			static void Print(cstr Message, ...);

			static void LogInfo(cstr Message, ...);
			static void LogWarning(cstr Message, ...);
			static void LogError(cstr Message, ...);

			static void LogException(const Exception& Exception);

			static void AssertionFailed(cstr File, uint32 LineNumber, cstr Function, cstr ConditionText, cstr Message, ...);
		};

#define DebugPrint(Content, ...) Debug::Print(Content, __VA_ARGS__)

#define DebugLogInfo(Content, ...) Debug::LogInfo(Content, __VA_ARGS__)
#define DebugLogWarning(Content, ...) Debug::LogWarning(Content, __VA_ARGS__)
#define DebugLogError(Content, ...) Debug::LogError(Content, __VA_ARGS__)

#define DebugLogException(Exception) Debug::LogException(Exception)

#define DebugAssert(Condition, Message, ...) \
	if (!(Condition)) \
		Engine::Debugging::Debug::AssertionFailed(DEBUG_ARGUMENTS, #Condition, Message, __VA_ARGS__)
	}
}

#endif