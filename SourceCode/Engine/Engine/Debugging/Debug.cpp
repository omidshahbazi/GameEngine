// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include "Debug.h"
#include <assert.h>
#include <stdio.h>

namespace Engine
{
	namespace Debugging
	{
#if DEBUG_MODE

		void Debug::Assertion(bool Condition, cstr ConditionText, cstr Message, cstr File, uint32 LineNumber, cstr Function)
		{
			if (Condition)
				return;

			__debugbreak();
		}

		void Debug::Print(cstr Message)
		{
			printf(Message);
		}

#endif
	}
}