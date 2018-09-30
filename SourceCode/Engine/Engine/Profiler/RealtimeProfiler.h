// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef REALTIME_PROFILER_H
#define REALTIME_PROFILER_H

#include <Common\PrimitiveTypes.h>
#include <Containers\Strings.h>
#include <Containers\List.h>
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
				SampleData(void) :
					CallCount(0),
					EndCount(0),
					StartTime(0),
					Parent(nullptr)
				{
				}

				String ModuleName;
				String SampleName;

				uint32 CallCount;
				uint32 EndCount;

				float32 StartTime;
				float32 EndTime;

				SampleData *Parent;
				List<SampleData*> Children;
			};

			struct Frame
			{
				List<SampleData*> Samples;
			};

			friend class Private::ProfilerAllocators;

		private:
			RealtimeProfiler(void);

		public:
			void BeginFrame(void);
			void EndFrame(void);

			void BeginSmaple(const String &ModuleName, const String &SampleName);
			void EndSample(void);

		private:
			List<Frame*> m_Frames;
			Frame *m_CurrentFrame;
			SampleData *m_CurrentSample;
		};
	}
}

#endif