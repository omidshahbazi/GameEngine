// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Profiler\RealtimeProfiler.h>
#include <Profiler\Private\ProfilerAllocators.h>
#include <Platform\PlatformMemory.h>
#include <Utility\HighResolutionTime.h>

namespace Engine
{
	using namespace Platform;
	using namespace Utility;

	namespace Profiler
	{
		using namespace Private;

		SINGLETON_DECLARATION(RealtimeProfiler)

		RealtimeProfiler::RealtimeProfiler(void) :
			m_CurrentSample(nullptr)
		{
		}

		void RealtimeProfiler::BeginSmaple(const String &ModuleName, const String &SampleName)
		{
			if (m_CurrentSample != nullptr && m_CurrentSample->ModuleName == ModuleName && m_CurrentSample->SampleName == SampleName)
			{
				++m_CurrentSample->CallCount;
				return;
			}

			SampleData *data = ReinterpretCast(SampleData*, AllocateMemory(&ProfilerAllocators::SampleDataAllocator, 1));
			PlatformMemory::Set(data, 0, sizeof(SampleData));

			data->CallCount = 1;
			data->ModuleName = ModuleName;
			data->SampleName = SampleName;
			data->Parent = m_CurrentSample;
			data->StartTime = HighResolutionTime::GetTime().GetSeconds();

			m_CurrentSample = data;
		}

		void RealtimeProfiler::EndSample(void)
		{
			if (m_CurrentSample != nullptr && ++m_CurrentSample->EndCount >= m_CurrentSample->CallCount)
				m_CurrentSample = m_CurrentSample->Parent;
		}
	}
}