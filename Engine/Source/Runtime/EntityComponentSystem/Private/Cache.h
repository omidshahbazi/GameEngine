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
						uint32 newBufferCount = m_BufferCount + m_GrowthCount;
						m_Buffer = ReinterpretCast(T*, ReallocateMemory(m_Allocator, m_Buffer, newBufferCount * sizeof(T)));
						CoreDebugAssert(Categories::EntityComponentSystem, m_Buffer != nullptr, "Couldn't allocate memory buffer for cache");

						PlatformMemory::Set(m_Buffer + m_BufferCount, 0, m_GrowthCount);

						m_BufferCount = newBufferCount;
					}

					++m_EnabledCount;

					if (m_AllocatedCount > m_EnabledCount)
						PlatformMemory::Copy(m_Buffer, m_EnabledCount, m_Buffer, (uint64)m_EnabledCount + 1, m_AllocatedCount - m_EnabledCount);

					T* result = &m_Buffer[m_EnabledCount - 1];

					return result;
				}

				void Deallocate(T* Value)
				{
					--m_AllocatedCount;

					uint32 index = Value - m_Buffer;

					if (index < m_EnabledCount)
						--m_EnabledCount;

					if (m_AllocatedCount != 0)
						PlatformMemory::Copy(m_Buffer, index + 1, m_Buffer, (uint64)index, m_AllocatedCount - index);
				}

				void Enable(T* Value)
				{
					uint32 index = Value - m_Buffer;

					if (index < m_EnabledCount)
						return;

					if (index != m_EnabledCount)
					{
						T temp = m_Buffer[index];
						m_Buffer[index] = m_Buffer[m_EnabledCount];
						m_Buffer[m_EnabledCount] = temp;
					}

					++m_EnabledCount;

				}

				void Disable(T* Value)
				{
					uint32 index = Value - m_Buffer;

					if (m_EnabledCount <= index)
						return;

					uint32 targetIndex = m_EnabledCount - 1;

					if (index != targetIndex)
					{
						T temp = m_Buffer[index];
						m_Buffer[index] = m_Buffer[targetIndex];
						m_Buffer[targetIndex] = temp;
					}

					--m_EnabledCount;
				}

				void ForEach(std::function<bool(T&)> Body, bool IncludeDisbabled)
				{
					CoreDebugAssert(Categories::EntityComponentSystem, Body != nullptr, "Body cannot be null");

					uint32 count = m_EnabledCount;
					if (IncludeDisbabled)
						count = m_AllocatedCount;

					for (uint32 i = 0; i < count; ++i)
						if (!Body(m_Buffer[i]))
							break;
				}

				void ForEach(std::function<bool(const T&)> Body, bool IncludeDisbabled) const
				{
					CoreDebugAssert(Categories::EntityComponentSystem, Body != nullptr, "Body cannot be null");

					uint32 count = m_EnabledCount;
					if (IncludeDisbabled)
						count = m_AllocatedCount;

					for (uint32 i = 0; i < count; ++i)
						if (!Body(m_Buffer[i]))
							break;
				}

				T* GetFirst(void)
				{
					return (m_EnabledCount == 0 ? nullptr : m_Buffer);
				}

				const T* GetFirst(void) const
				{
					return (m_EnabledCount == 0 ? nullptr : m_Buffer);
				}

				T* GetLast(bool IncludeDisabled)
				{
					uint32 count = m_EnabledCount;
					if (IncludeDisabled)
						count = m_AllocatedCount;

					return (count == 0 ? nullptr : m_Buffer + count);
				}

				const T* GetLast(bool IncludeDisabled) const
				{
					uint32 count = m_EnabledCount;
					if (IncludeDisabled)
						count = m_AllocatedCount;

					return (count == 0 ? nullptr : m_Buffer + count);
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