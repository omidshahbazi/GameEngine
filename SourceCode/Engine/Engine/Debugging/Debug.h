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
#if _DEBUG

		class Debug
		{
		public:
			static void Assertion(bool Condition, const char8 *ConditionText, const char8 *Message, const char8 *File, uint32 LineNumber);
		};

#define Assert(Condition) \
	Assert(Condition, "")

#define Assert(Condition, Message) \
	Engine::Debugging::Debug::Assertion(Condition, #Condition, Message, __FILE__, __LINE__)

#else

#define Assert(Condition)
#define Assert(Condition, Message)

#endif
	}
}

#endif