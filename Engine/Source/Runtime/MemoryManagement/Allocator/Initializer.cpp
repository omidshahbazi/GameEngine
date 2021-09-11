// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef INSTANTIATION_H
#define INSTANTIATION_H

#include <MemoryManagement\Allocator\Initializer.h>
#include <MemoryManagement\Allocator\DefaultAllocator.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Platform\PlatformMemory.h>
#include <Platform\PlatformFile.h>
#include <Platform\PlatformOS.h>
#include <sstream>
#include <experimental\filesystem>

namespace Engine
{
	using namespace Platform;

	namespace MemoryManagement
	{
		namespace Allocator
		{
			CREATOR_DEFINITION(Initializer);

			void Initializer::Initialize(uint64 ReserveSize, const AllocatorInfo* const AllocatorsInfo, uint32 AllocatorInfoCount)
			{
				HardAssert(!m_Initialized, "Initializer already initialized");

#ifndef ONLY_USING_C_ALLOCATOR
				HardAssert(ReserveSize > 0, "ReserveSize cannot be zero");
				HardAssert(AllocatorsInfo != nullptr, "AllocatorsInfo cannot be null");
				HardAssert(AllocatorInfoCount > 0, "AllocatorInfoCount cannot be zero");
				HardAssert(AllocatorInfoCount <= MAX_ALLOCATORS_COUNT, "AllocatorInfoCount exceeds MAX_ALLOCATORS_COUNT");

				m_ResevedSize = ReserveSize;
				m_AllocatorInfoCount = AllocatorInfoCount;
				for (uint32 i = 0; i < m_AllocatorInfoCount; ++i)
					m_AllocatorsInfo[i] = AllocatorsInfo[i];
#endif

				DefaultAllocator::Create();
				RootAllocator::Create(DefaultAllocator::GetInstance());

				m_Initialized = true;
			}

			void Initializer::Initialize(uint64 ReserveSize, cwstr FilePath)
			{
#ifdef ONLY_USING_C_ALLOCATOR
				Initialize(ReserveSize, nullptr, 0);
#else
				Initializer::AllocatorInfo allocatorsInfo[MAX_ALLOCATORS_COUNT];
				uint32 allocatorInfoCount = Initializer::ReadInfoFromFile(FilePath, allocatorsInfo, MAX_ALLOCATORS_COUNT);

				HardAssert(allocatorInfoCount != 0, "Couldn't read any AllocatorInfo");

				Initialize(ReserveSize, allocatorsInfo, allocatorInfoCount);
#endif
			}

			uint32 Initializer::ReadInfoFromFile(cwstr FilePath, AllocatorInfo* AllocatorsInfo, uint32 AllocatorInfoCount)
			{
				std::experimental::filesystem::path path(FilePath);

				if (path.is_relative())
				{
					char16 exePath[1024];
					PlatformOS::GetExecutablePath(exePath);

					path = std::experimental::filesystem::path(exePath).remove_filename().concat('/').concat(FilePath);
				}

				if (!PlatformFile::Exists(path.c_str()))
					return 0;

				PlatformFile::Handle handle = PlatformFile::Open(path.c_str(), PlatformFile::OpenModes::Input);
				if (handle == 0)
					return 0;

				uint64 size = PlatformFile::Size(handle);

				const uint16 DataSize = 2048;

				HardAssert(size <= DataSize, "File size is longer than buffer size");

				char8 data[2048];
				PlatformFile::Read(handle, data, size);

				PlatformFile::Close(handle);

#define READ_VALUE(Name, EndCharacter) \
				std::string Name; \
				while (index < size) \
				{ \
					c = data[index++]; \
					if (c == EndCharacter || c == '\n') \
						break; \
					Name += c; \
				}

				uint32 allocatorInfoCount = 0;

				uint32 index = 0;
				while (index < size)
				{
					char8 c;

					READ_VALUE(name, ':');

					if (name.length() == 0)
						continue;

					HardAssert(name.length() <= MAX_ALLOCATOR_NAME_LENGTH, "Allocator name length must be smaller than MAX_ALLOCATOR_NAME_LENGTH");

					READ_VALUE(rateStr, '\n');

					char8* endPtr;
					float32 rate = strtof(rateStr.c_str(), &endPtr);

					HardAssert(rateStr.c_str() != endPtr, "Rate argument is invalid");

					PlatformMemory::Copy(name.c_str(), AllocatorsInfo[allocatorInfoCount].Name, name.length() + 1);
					AllocatorsInfo[allocatorInfoCount].ReserveSizeRate = rate;
					++allocatorInfoCount;
				}

#undef READ_VALUE

				return allocatorInfoCount;
			}

			float32 Initializer::GetReserveSizeRate(cstr Name) const
			{
				for (uint32 i = 0; i < m_AllocatorInfoCount; ++i)
				{
					auto info = m_AllocatorsInfo[i];

					if (strcmp(info.Name, Name) == 0)
						return info.ReserveSizeRate;
				}

				std::stringstream stream;
				stream << "Couldn't find any AllocatorInfo for [" << Name << "]";

				HardAssert(false, stream.str().c_str());

				return 0;
			}
		}
	}
}

#endif