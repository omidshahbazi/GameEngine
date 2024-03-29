// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Profiler\RealtimeProfiler.h>
#include <Profiler\Private\ProfilerAllocators.h>
#include <TimeUtility\HighResolutionTime.h>
#include <Platform\PlatformDirectory.h>
#include <FileUtility\FileSystem.h>
#include <FileUtility\Path.h>
#include <Containers\StringUtility.h>
#include <Common\BitwiseUtils.h>

namespace Engine
{
	using namespace Containers;
	using namespace Platform;
	using namespace FileUtility;
	using namespace TimeUtility;

	namespace Profiler
	{
		using namespace Private;

		cwstr PROFILER_DIRECTORY_NAME(L"Profiler");

		SINGLETON_DEFINITION(RealtimeProfiler);

		uint64 GetTime(void)
		{
			return HighResolutionTime::GetTime().GetMicroseconds();
		}

		RealtimeProfiler::RealtimeProfiler(void) :
			m_CurrentSample(nullptr)
		{
			ProfilerAllocators::Create();

			WString dir = Path::Combine(FileSystem::GetWorkingPath(), WString(PROFILER_DIRECTORY_NAME));
			if (!PlatformDirectory::Exists(dir.GetValue()))
				PlatformDirectory::Create(dir.GetValue());

			dir += L"/";
			dir += StringUtility::ToString<char16>(GetTime());
			dir += L".profile";

			m_File = PlatformFile::Open(dir.GetValue(), PlatformFile::OpenModes::Output | PlatformFile::OpenModes::Binary);
		}

		void RealtimeProfiler::BeginFrame(void)
		{
			int32 type = (int32)DataTypes::BegineFrame;
			PlatformFile::Write(m_File, ReinterpretCast(byte*, &type), sizeof(int32));
		}

		void RealtimeProfiler::EndFrame(void)
		{
			int32 type = (int32)DataTypes::EndFrame;
			PlatformFile::Write(m_File, ReinterpretCast(byte*, &type), sizeof(int32));
		}

		void RealtimeProfiler::BeginSmaple(const String& ModuleName, const String& SampleName)
		{
			if (m_CurrentSample != nullptr && m_CurrentSample->ModuleName == ModuleName && m_CurrentSample->SampleName == SampleName)
			{
				++m_CurrentSample->CallCount;
				return;
			}

			SampleData* data = ProfilerAllocators::SampleDataAllocator_Allocate<SampleData>();
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
				{
					WriteSmapleData(m_CurrentSample);

					ProfilerAllocators::SampleDataAllocator_Deallocate(m_CurrentSample);

					m_CurrentSample = nullptr;

					return;
				}

				m_CurrentSample = m_CurrentSample->Parent;
			}
		}

		void RealtimeProfiler::WriteSmapleData(SampleData* Data)
		{
			int32 type = (int32)DataTypes::BegineSample;
			PlatformFile::Write(m_File, ReinterpretCast(byte*, &type), sizeof(int32));

			uint32 len = Data->ModuleName.GetLength();
			PlatformFile::Write(m_File, ReinterpretCast(byte*, &len), sizeof(uint32));
			PlatformFile::Write(m_File, ReinterpretCast(const byte*, Data->ModuleName.GetValue()), len);

			len = Data->SampleName.GetLength();
			PlatformFile::Write(m_File, ReinterpretCast(byte*, &len), sizeof(uint32));
			PlatformFile::Write(m_File, ReinterpretCast(const byte*, Data->SampleName.GetValue()), len);

			PlatformFile::Write(m_File, ReinterpretCast(byte*, &Data->CallCount), sizeof(uint32));

			uint64 duration = Data->EndTime - Data->StartTime;
			PlatformFile::Write(m_File, ReinterpretCast(byte*, &duration), sizeof(uint64));

			for (auto child : Data->Children)
				WriteSmapleData(child);

			type = (int32)DataTypes::EndSample;
			PlatformFile::Write(m_File, ReinterpretCast(byte*, &type), sizeof(int32));
		}
	}
}