// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include "Debug.h"
#include <assert.h>

namespace Engine
{
	namespace Debugging
	{
#if _DEBUG

		void Debug::Assertion(bool Condition, const char8 *ConditionText, const char8 *Message, const char8 *File, uint32 LineNumber)
		{
			if (Condition)
				return;

			__debugbreak();
		}

#endif
	}
}