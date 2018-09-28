// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef REALTIME_PROFILER_H
#define REALTIME_PROFILER_H

#include <Common\PrimitiveTypes.h>
#include <Containers\Strings.h>
#include <MemoryManagement\Singleton.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace Profiler
	{
		namespace Private
		{
			class ProfilerAllocators;
		}

		class PROFILER_API RealtimeProfiler
		{
			SINGLETON_DEFINITION(RealtimeProfiler)

		private:
			struct SampleData
			{
			public:
				String ModuleName;
				String SampleName;

				uint32 CallCount;
				uint32 EndCount;

				float32 StartTime;

				SampleData *Parent;
			};

			friend class Private::ProfilerAllocators;

		private:
			RealtimeProfiler(void);

		public:
			void BeginSmaple(const String &ModuleName, const String &SampleName);

			void EndSample(void);

		private:
			SampleData * m_CurrentSample;
		};
	}
}

#endif