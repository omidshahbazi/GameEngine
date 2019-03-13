// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef REALTIME_PROFILER_H
#define REALTIME_PROFILER_H

#include <MemoryManagement\Singleton.h>
#include <Profiler\ProfileSample.h>
#include <Containers\Strings.h>
#include <Platform\PlatformFile.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace Platform;

	namespace Profiler
	{
		class PROFILER_API RealtimeProfiler
		{
			SINGLETON_DECLARATION(RealtimeProfiler)

		private:
			RealtimeProfiler(void);

		public:
			void BeginFrame(void);
			void EndFrame(void);

			void BeginSmaple(const String &ModuleName, const String &SampleName);
			void EndSample(void);

		private:
			void DumpSmapleData(SampleData *Data);

		private:
			SampleData *m_CurrentSample;
			PlatformFile::Handle m_File;
		};
	}
}

#endif