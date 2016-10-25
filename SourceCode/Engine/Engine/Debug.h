// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include "PrimitiveTypes.h"
#include <assert.h>

#ifndef DEBUG_H
#define DEBUG_H

#if _DEBUG

class Debug
{
public:
	static void Assert(bool Condition, const char8 *ConditionText, const char8 *Message, const char8 *File, uint32 LineNumber)
	{
		if (Condition)
			return;

		__debugbreak();
	}
};

#define Assert(Condition) \
	Assert(Condition, "")

#define Assert(Condition, Message) \
	Debug::Assert(Condition, #Condition, Message, __FILE__, __LINE__)

#else

#define Assert(Condition)
#define Assert(Condition, Message)

#endif

#endif