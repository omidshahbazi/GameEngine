// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Debugging\Debug.h>
#include <stdio.h>
#include <iostream>
#include <stdarg.h>

namespace Engine
{
	namespace Debugging
	{
#define Call_CALLBACK(Callback) \
			va_list args; \
			va_start(args, Message); \
			(*Callback)(Message, args); \
			va_end(args);

		std::shared_ptr<Debug::LogCallback> Debug::m_OnPrinCallback = nullptr;
		std::shared_ptr<Debug::LogCallback> Debug::m_OnLogInfoCallback = nullptr;
		std::shared_ptr<Debug::LogCallback> Debug::m_OnLogWarningCallback = nullptr;
		std::shared_ptr<Debug::LogCallback> Debug::m_OnLogErrorCallback = nullptr;
		std::shared_ptr<Debug::AssertionFailedCallback> Debug::m_OnAssertionFailedCallback = nullptr;

		void Debug::Print(cstr Message, ...)
		{
			if (Message == nullptr)
				return;

			if (m_OnPrinCallback == nullptr)
				return;

			Call_CALLBACK(m_OnPrinCallback);
		}

		void Debug::LogInfo(cstr Message, ...)
		{
			if (Message == nullptr)
				return;

			if (m_OnLogInfoCallback == nullptr)
				return;

			Call_CALLBACK(m_OnLogInfoCallback);
		}

		void Debug::LogWarning(cstr Message, ...)
		{
			if (Message == nullptr)
				return;

			if (m_OnLogWarningCallback == nullptr)
				return;

			Call_CALLBACK(m_OnLogWarningCallback);
		}

		void Debug::LogError(cstr Message, ...)
		{
			if (Message == nullptr)
				return;

			if (m_OnLogErrorCallback == nullptr)
				return;

			Call_CALLBACK(m_OnLogErrorCallback);
		}

		void Debug::AssertionFailed(cstr File, uint32 LineNumber, cstr Function, cstr ConditionText, cstr Message, ...)
		{
			if (m_OnAssertionFailedCallback == nullptr)
				return;

			va_list args;
			va_start(args, Message);
			(*m_OnAssertionFailedCallback)(File, LineNumber, Function, ConditionText, Message, args);
			va_end(args);
		}

		void Debug::SetPrintCallback(const LogCallback& Callback)
		{
			m_OnPrinCallback = std::make_shared<LogCallback>(Callback);
		}

		void Debug::SetLogInfoCallback(const LogCallback& Callback)
		{
			m_OnLogInfoCallback = std::make_shared<LogCallback>(Callback);
		}

		void Debug::SetLogWarningCallback(const LogCallback& Callback)
		{
			m_OnLogWarningCallback = std::make_shared<LogCallback>(Callback);
		}

		void Debug::SetLogErrorCallback(const LogCallback& Callback)
		{
			m_OnLogErrorCallback = std::make_shared<LogCallback>(Callback);
		}

		void Debug::SetOnAssertionFailedCallback(const AssertionFailedCallback& Callback)
		{
			m_OnAssertionFailedCallback = std::make_shared<AssertionFailedCallback>(Callback);
		}
	}
}