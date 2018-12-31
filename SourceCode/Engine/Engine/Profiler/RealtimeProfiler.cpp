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

		SINGLETON_DEFINITION(RealtimeProfiler)

			uint64 GetTime(void)
		{
			return HighResolutionTime::GetTime().GetMilliseconds();
		}

		RealtimeProfiler::RealtimeProfiler(void) :
			m_CurrentFrame(nullptr),
			m_CurrentSample(nullptr)
		{
		}

		void RealtimeProfiler::BeginFrame(void)
		{
			m_CurrentFrame = ReinterpretCast(Frame*, AllocateMemory(&ProfilerAllocators::FrameAllocator, 1));
			Construct(m_CurrentFrame);
		}

		void RealtimeProfiler::EndFrame(void)
		{
			if (m_CurrentFrame != nullptr)
				m_Frames.Add(m_CurrentFrame);
		}

		void RealtimeProfiler::BeginSmaple(const String &ModuleName, const String &SampleName)
		{
			if (m_CurrentSample != nullptr && m_CurrentSample->ModuleName == ModuleName && m_CurrentSample->SampleName == SampleName)
			{
				++m_CurrentSample->CallCount;
				return;
			}

			SampleData *data = ReinterpretCast(SampleData*, AllocateMemory(&ProfilerAllocators::SampleDataAllocator, 1));
			Construct(data);

			data->CallCount = 1;

			data->ModuleName = ModuleName;
			data->SampleName = SampleName;

			data->Parent = m_CurrentSample;
			if (m_CurrentSample != nullptr)
				m_CurrentSample->Children.Add(data);

			data->StartTime = GetTime();

			m_CurrentSample = data;
		}

		void RealtimeProfiler::EndSample(void)
		{
			if (m_CurrentSample != nullptr && ++m_CurrentSample->EndCount >= m_CurrentSample->CallCount)
			{
				m_CurrentSample->EndTime = GetTime();

				if (m_CurrentSample->Parent == nullptr)
					m_CurrentFrame->Samples.Add(m_CurrentSample);

				m_CurrentSample = m_CurrentSample->Parent;
			}
		}
	}
}