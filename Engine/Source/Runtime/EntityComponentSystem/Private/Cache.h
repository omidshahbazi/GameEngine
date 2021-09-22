// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef CACHE_H
#define CACHE_H

#include <Common\PrimitiveTypes.h>
#include <Allocators\AllocatorBase.h>
#include <Platform\PlatformMemory.h>
#include <Debugging\CoreDebug.h>
#include <EntityComponentSystem\Private\Iterator.h>

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
			public:
				typedef Iterator<T> IteratorType;

			protected:
				Cache(AllocatorBase* Allocator, uint16 GrowthCount) :
					m_Allocator(Allocator),
					m_GrowthCount(GrowthCount),
					m_Buffer(nullptr),
					m_BufferCount(0),
					m_AllocatedCount(0)
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

					T* result = &m_Buffer[m_AllocatedCount - 1];

					return result;
				}

				void Deallocate(T* Value)
				{
					--m_AllocatedCount;

					uint32 index = Value - m_Buffer;

					if (m_AllocatedCount != 0)
						PlatformMemory::Copy(m_Buffer, index + 1, m_Buffer, (uint64)index, m_AllocatedCount - index);
				}

				T* Find(std::function<bool(T&)> Condition)
				{
					CoreDebugAssert(Categories::EntityComponentSystem, Condition != nullptr, "Body cannot be null");

					for (uint32 i = 0; i < m_AllocatedCount; ++i)
						if (Condition(m_Buffer[i]))
							return &m_Buffer[i];

					return nullptr;
				}

				const T* Find(std::function<bool(const T&)> Condition) const
				{
					CoreDebugAssert(Categories::EntityComponentSystem, Condition != nullptr, "Body cannot be null");

					for (uint32 i = 0; i < m_AllocatedCount; ++i)
						if (Condition(m_Buffer[i]))
							return &m_Buffer[i];

					return nullptr;
				}

			public:
				IteratorType begin(void) const
				{
					if (m_AllocatedCount == 0)
						return {};

					return IteratorType(m_Buffer);
				}

				IteratorType end(bool IncludeDisabled = false) const
				{
					return IteratorType(m_Buffer + m_AllocatedCount);
				}

			private:
				AllocatorBase* m_Allocator;
				uint16 m_GrowthCount;
				T* m_Buffer;
				uint32 m_BufferCount;
				uint32 m_AllocatedCount;
			};
		}
	}
}

#endif