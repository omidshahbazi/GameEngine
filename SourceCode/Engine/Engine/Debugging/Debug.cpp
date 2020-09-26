// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Debugging\Debug.h>
#include <assert.h>
#include <stdio.h>
#include <iostream>

namespace Engine
{
	namespace Debugging
	{
		void Debug::Assertion(bool Condition, cstr ConditionText, cstr Message, cstr File, uint32 LineNumber, cstr Function)
		{
			if (Condition)
				return;

			std::cout << "Assertion Failed: " << Message << "\n" << "On [" << ConditionText << "] in [" << Function << "] at [" << File << " Ln" << LineNumber << "]" << "\n";

			__debugbreak();
		}

		void Debug::Print(cstr Message)
		{
			printf(Message);
		}

		void Debug::LogInfo(cstr Message)
		{
			std::cout << Message << "\n";
		}

		void Debug::LogWarning(cstr Message)
		{
			std::cout << "Warning " << Message << "\n";
		}

		void Debug::LogError(cstr Message)
		{
			std::cout << "Error " << Message << "\n";
		}
	}
}