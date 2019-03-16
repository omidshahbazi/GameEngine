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

			template<typename BaseType>
		BaseType *Allocate(void)
		{
			return ReinterpretCast(BaseType*, AllocateMemory(&ProfilerAllocators::SampleDataAllocator, 1));
		}

		template<typename BaseType>
		void Deallocate(BaseType *Ptr)
		{
			DeallocateMemory(&ProfilerAllocators::SampleDataAllocator, Ptr);
		}

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
			int32 type = (int32)DataTypes::BegineFrame;
			PlatformFile::Write(m_File, ReinterpretCast(byte*, &type), sizeof(int32));
		}

		void RealtimeProfiler::EndFrame(void)
		{
			int32 type = (int32)DataTypes::EndFrame;
			PlatformFile::Write(m_File, ReinterpretCast(byte*, &type), sizeof(int32));
		}

		void RealtimeProfiler::BeginSmaple(const String &ModuleName, const String &SampleName)
		{
			if (m_CurrentSample != nullptr && m_CurrentSample->ModuleName == ModuleName && m_CurrentSample->SampleName == SampleName)
			{
				++m_CurrentSample->CallCount;
				return;
			}

			SampleData *data = Allocate<SampleData>();
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

					for each (auto child in m_CurrentSample->Children)
						Deallocate(child);
					Deallocate(m_CurrentSample);
				}

				m_CurrentSample = m_CurrentSample->Parent;
			}
		}

		void RealtimeProfiler::WriteSmapleData(SampleData *Data)
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

			for each (auto child in Data->Children)
				WriteSmapleData(child);

			type = (int32)DataTypes::EndSample;
			PlatformFile::Write(m_File, ReinterpretCast(byte*, &type), sizeof(int32));
		}
	}
}