// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Debugging\Debug.h>
#include <Debugging\LogManager.h>
#include <Platform\PlatformWindow.h>
#include <Containers\StringUtility.h>
#include <stdarg.h>

namespace Engine
{
	using namespace Containers;

	namespace Debugging
	{
#define Call_CALLBACK(Level) \
			va_list args; \
			va_start(args, Message); \
			LogManager::GetInstance()->GetCoreLogger()->Put(Logger::Levels::Level, Message, args); \
			va_end(args);

		void Debug::Print(cstr Message, ...)
		{
			if (Message == nullptr)
				return;

			Call_CALLBACK(Info);
		}

		void Debug::LogInfo(cstr Message, ...)
		{
			if (Message == nullptr)
				return;

			Call_CALLBACK(Info);
		}

		void Debug::LogWarning(cstr Message, ...)
		{
			if (Message == nullptr)
				return;

			Call_CALLBACK(Warning);
		}

		void Debug::LogError(cstr Message, ...)
		{
			if (Message == nullptr)
				return;

			Call_CALLBACK(Error);
		}

		void Debug::LogException(const Exception& Exception)
		{
			LogManager::GetInstance()->GetCoreLogger()->Put(Exception);
		}

		void Debug::AssertionFailed(cstr File, uint32 LineNumber, cstr Function, cstr ConditionText, cstr Message, ...)
		{
			char8 str[4096];
			uint16 size = 0;

			va_list args;
			va_start(args, Message);
			size = StringUtility::Format(str, Message, args);
			va_end(args);

			size += sprintf(str + size, "\non [%s] in [%s] at [%s:Ln%d]", ConditionText, Function, File, LineNumber);
			str[size] = '\0';

			LogManager::GetInstance()->GetCoreLogger()->Put(Logger::Levels::Fatal, str);

			if (PlatformOS::IsDebuggerAttached())
			{
				__debugbreak();
				return;
			}

			PlatformWindow::ShowMessageBox(str, "Assertion Failed", PlatformWindow::MessageBoxButtons::OK, PlatformWindow::MessageBoxIcons::Stop, PlatformWindow::MessageBoxDefaultButtons::Button1, PlatformWindow::MessageBoxModlities::Task);
		}
	}
}