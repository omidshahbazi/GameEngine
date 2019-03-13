// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Profiler\RealtimeProfiler.h>
#include <Profiler\Private\ProfilerAllocators.h>
#include <Utility\HighResolutionTime.h>
#include <Platform\PlatformDirectory.h>
#include <Utility\FileSystem.h>
#include <Utility\Path.h>
#include <Containers\StringUtility.h>
#include <Common\BitwiseUtils.h>

namespace Engine
{
	using namespace Containers;
	using namespace Platform;
	using namespace Utility;

	namespace Profiler
	{
		using namespace Private;

		const WString PROFILER_DIRECTORY_NAME(L"Profiler");

		SINGLETON_DEFINITION(RealtimeProfiler)

			uint64 GetTime(void)
		{
			return HighResolutionTime::GetTime().GetMicroseconds();
		}

		RealtimeProfiler::RealtimeProfiler(void) :
			m_CurrentSample(nullptr)
		{
			WString dir = Path::Combine(FileSystem::GetWorkingPath(), PROFILER_DIRECTORY_NAME);
			if (!PlatformDirectory::Exists(dir.GetValue()))
				PlatformDirectory::Create(dir.GetValue());

			dir += L"/";
			dir += StringUtility::ToString<char16>(GetTime());
			dir += L".profile";

			m_File = PlatformFile::Open(dir.GetValue(), PlatformFile::OpenModes::Output | PlatformFile::OpenModes::Binary);
		}

		void RealtimeProfiler::BeginFrame(void)
		{
		}

		void RealtimeProfiler::EndFrame(void)
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
					DumpSmapleData(m_CurrentSample);

				m_CurrentSample = m_CurrentSample->Parent;
			}
		}

		void RealtimeProfiler::DumpSmapleData(SampleData *Data)
		{
			PlatformFile::Write(m_File, ReinterpretCast(byte*, Data), sizeof(SampleData));
		}
	}
}