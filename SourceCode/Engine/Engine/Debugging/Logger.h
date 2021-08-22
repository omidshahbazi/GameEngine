// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef LOGGER_H
#define LOGGER_H

#include <Common\SpinLock.h>
#include <Common\Categories.h>
#include <Containers\Strings.h>
#include <Containers\ListenerContainer.h>
#include <Containers\Exception.h>
#include <Threading\Thread.h>

namespace Engine
{
	using namespace Threading;

	namespace Debugging
	{
		class DEBUGGING_API Logger
		{
		public:
			enum class Levels
			{
				Info,
				Warning,
				Error,
				Exception,
				Fatal
			};

			struct Log
			{
			public:
				template<uint16 MaxLength>
				struct String
				{
					char8 Value[MaxLength];
					uint16 Length;
				};

			public:
				Levels Level;
				Categories CategoryFlags;
				String<512> File;
				uint32 LineNumber;
				String<256> Function;
				String<2048> Content;
			};

			class IListener
			{
			public:
				virtual void OnLog(const Log& Log)
				{
				}
			};

			LISTENER_DECLARATION(IListener);

		public:
			Logger(const WString& FilePath);
			~Logger(void);

			void Put(Levels Level, const String Content, ...);
			void Put(Levels Level, const String Content, va_list Args);

			void Put(Levels Level, Categories CategoryFlags, const String Content, ...);
			void Put(Levels Level, Categories CategoryFlags, const String Content, va_list Args);

			void Put(const String& File, uint32 LineNumber, const String& Function, Levels Level, const String Content, ...);
			void Put(const String& File, uint32 LineNumber, const String& Function, Levels Level, const String Content, va_list Args);

			void Put(const String& File, uint32 LineNumber, const String& Function, Levels Level, Categories CategoryFlags, const String Content, ...);
			void Put(const String& File, uint32 LineNumber, const String& Function, Levels Level, Categories CategoryFlags, const String Content, va_list Args);

			void Put(const Exception& Exception);

			Levels GetMinimumLevel(void) const
			{
				return m_MinimumLevel;
			}
			void SetMinimumLevel(Levels Level)
			{
				m_MinimumLevel = Level;
			}

			Categories GetCategoryMask(void) const
			{
				return m_CategoryMask;
			}
			void SetCategoryMask(Categories CategoryMask)
			{
				m_CategoryMask = CategoryMask;
			}

		private:
			void InsertLog(Levels Level, Categories CategoryFlags, cstr File, uint32 LineNumber, cstr Function, cstr Content, va_list Args);
			void InsertLog(Levels Level, Categories CategoryFlags, cstr File, uint32 LineNumber, cstr Function, cstr Content);

			void ThreadWorker(void);

		private:
			Thread m_WorkerThread;

			Log m_LogQueue[256];
			uint8 m_QueueSize;
			SpinLock m_Lock;

			WString m_FilePath;
			Levels m_MinimumLevel;
			Categories m_CategoryMask;
		};
	}
}

#endif