// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Debugging\CoreDebug.h>
#include <Debugging\LogManager.h>
#include <Platform\PlatformWindow.h>
#include <Containers\StringUtility.h>
#include <stdarg.h>

namespace Engine
{
	using namespace Containers;

	namespace Debugging
	{
#define PUT_LOG(Level) \
			va_list args; \
			va_start(args, Message); \
			LogManager::GetInstance()->GetCoreLogger()->Put(Logger::Levels::Level, CategoryFlags, Message, args); \
			va_end(args);

#define PUT_FULL_LOG(Level) \
			va_list args; \
			va_start(args, Message); \
			LogManager::GetInstance()->GetCoreLogger()->Put(File, LineNumber, Function, Logger::Levels::Level, CategoryFlags, Message, args); \
			va_end(args);

		void CoreDebug::Print(Categories CategoryFlags, cstr Message, ...)
		{
			if (Message == nullptr)
				return;

			PUT_LOG(Info);
		}

		void CoreDebug::Print(cstr File, uint32 LineNumber, cstr Function, Categories CategoryFlags, cstr Message, ...)
		{
			if (Message == nullptr)
				return;

			PUT_FULL_LOG(Info);
		}

		void CoreDebug::LogInfo(Categories CategoryFlags, cstr Message, ...)
		{
			if (Message == nullptr)
				return;

			PUT_LOG(Info);
		}

		void CoreDebug::LogInfo(cstr File, uint32 LineNumber, cstr Function, Categories CategoryFlags, cstr Message, ...)
		{
			if (Message == nullptr)
				return;

			PUT_FULL_LOG(Info);
		}

		void CoreDebug::LogWarning(Categories CategoryFlags, cstr Message, ...)
		{
			if (Message == nullptr)
				return;

			PUT_LOG(Warning);
		}

		void CoreDebug::LogWarning(cstr File, uint32 LineNumber, cstr Function, Categories CategoryFlags, cstr Message, ...)
		{
			if (Message == nullptr)
				return;

			PUT_FULL_LOG(Warning);
		}

		void CoreDebug::LogError(Categories CategoryFlags, cstr Message, ...)
		{
			if (Message == nullptr)
				return;

			PUT_LOG(Error);
		}

		void CoreDebug::LogError(cstr File, uint32 LineNumber, cstr Function, Categories CategoryFlags, cstr Message, ...)
		{
			if (Message == nullptr)
				return;

			PUT_FULL_LOG(Error);
		}

		void CoreDebug::LogException(Categories CategoryFlags, const Exception& Exception)
		{
			LogManager::GetInstance()->GetCoreLogger()->Put(Exception);
		}

		void CoreDebug::AssertionFailed(cstr File, uint32 LineNumber, cstr Function, cstr ConditionText, Categories CategoryFlags, cstr Message, ...)
		{
			char8 str[4096];
			uint16 size = 0;

			va_list args;
			va_start(args, Message);
			size = StringUtility::Format(str, Message, args);
			va_end(args);

			size += sprintf(str + size, "\non [%s] in [%s] at [%s:Ln%d]", ConditionText, Function, File, LineNumber);
			str[size] = '\0';

			LogManager::GetInstance()->GetCoreLogger()->Put(Logger::Levels::Fatal, CategoryFlags, str);

			if (PlatformOS::IsDebuggerAttached())
			{
				__debugbreak();
				return;
			}

			PlatformWindow::ShowMessageBox(str, "Assertion Failed", PlatformWindow::MessageBoxButtons::OK, PlatformWindow::MessageBoxIcons::Stop, PlatformWindow::MessageBoxDefaultButtons::Button1, PlatformWindow::MessageBoxModlities::Task);
		}
	}
}