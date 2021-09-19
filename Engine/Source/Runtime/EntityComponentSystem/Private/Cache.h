// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef CACHE_H
#define CACHE_H

#include <Common\PrimitiveTypes.h>
#include <Allocators\AllocatorBase.h>
#include <Platform\PlatformMemory.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace Common;
	using namespace Allocators;
	using namespace Platform;
	using namespace Debugging;

	namespace EntityComponentSystem
	{
		namespace Private
		{
			template<typename T>
			class Cache
			{
			protected:
				Cache(AllocatorBase* Allocator, uint16 GrowthCount) :
					m_Allocator(Allocator),
					m_GrowthCount(GrowthCount),
					m_Buffer(nullptr),
					m_BufferCount(0),
					m_AllocatedCount(0),
					m_EnabledCount(0)
				{
				}

				T* Allocate(void)
				{
					if (++m_AllocatedCount > m_BufferCount)
					{
						m_AllocatedCount = m_BufferCount + m_GrowthCount;
						m_Buffer = ReinterpretCast(T*, ReallocateMemory(m_Allocator, m_Buffer, m_AllocatedCount * sizeof(T)));
						CoreDebugAssert(Categories::EntityComponentSystem, m_Buffer != nullptr, "Couldn't allocate memory buffer for cache");

						PlatformMemory::Set(m_Buffer + m_BufferCount, 0, m_GrowthCount);

						m_BufferCount += m_GrowthCount;
					}

					if (m_EnabledCount != 0)
						PlatformMemory::Copy(m_Buffer, m_EnabledCount, m_Buffer, (uint64)m_EnabledCount + 1, m_AllocatedCount - m_EnabledCount);

					T* result = &m_Buffer[m_EnabledCount];

					++m_EnabledCount;

					return result;
				}

				void Destroy(const T& Value)
				{

				}

				void Enable(const T& Value)
				{

				}

				void Disable(const T& Value)
				{

				}

			private:
				AllocatorBase* m_Allocator;
				uint16 m_GrowthCount;
				T* m_Buffer;
				uint32 m_BufferCount;
				uint32 m_AllocatedCount;
				uint32 m_EnabledCount;
			};
		}
	}
}

#endif