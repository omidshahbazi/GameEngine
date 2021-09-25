// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef CACHE_H
#define CACHE_H

#include <Common\PrimitiveTypes.h>
#include <Allocators\AllocatorBase.h>
#include <Platform\PlatformMemory.h>
#include <Debugging\CoreDebug.h>
#include <EntityComponentSystem\Private\Iterator.h>
#include <EntityComponentSystem\EntityComponentSystemException.h>

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
						THROW_IF_ENTITY_COMPONENT_SYSTEM_EXCEPTION(m_Buffer != nullptr, "Couldn't allocate memory buffer for cache");

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

					Swap(m_Buffer + index, m_Buffer + m_AllocatedCount);
				}

				T* Find(std::function<bool(T&)> Condition)
				{
					CoreDebugAssert(Categories::EntityComponentSystem, Condition != nullptr, "Condition cannot be null");

					for (uint32 i = 0; i < m_AllocatedCount; ++i)
						if (Condition(m_Buffer[i]))
							return &m_Buffer[i];

					return nullptr;
				}

				const T* Find(std::function<bool(const T&)> Condition) const
				{
					CoreDebugAssert(Categories::EntityComponentSystem, Condition != nullptr, "Condition cannot be null");

					for (uint32 i = 0; i < m_AllocatedCount; ++i)
						if (Condition(m_Buffer[i]))
							return &m_Buffer[i];

					return nullptr;
				}

				void Swap(T* Left, T* Right)
				{
					CoreDebugAssert(Categories::EntityComponentSystem, Left != nullptr, "Left cannot be null");
					CoreDebugAssert(Categories::EntityComponentSystem, Right != nullptr, "Right cannot be null");

					T temp = *Left;
					*Left = *Right;
					*Right = temp;
				}

				void Sort(std::function<bool(const T&, const T&)> IsLessThan)
				{
					CoreDebugAssert(Categories::EntityComponentSystem, IsLessThan != nullptr, "IsLessThan cannot be null");

					for (uint32 i = 0; i < m_AllocatedCount - 1; ++i)
						for (uint32 j = i + 1; j < m_AllocatedCount; ++j)
						{
							if (!IsLessThan(m_Buffer[j], m_Buffer[i]))
								continue;

							T temp = m_Buffer[i];
							m_Buffer[i] = m_Buffer[j];
							m_Buffer[j] = temp;
						}
				}

			public:
				IteratorType begin(void) const
				{
					if (m_AllocatedCount == 0)
						return {};

					return IteratorType(m_Buffer);
				}

				IteratorType end(void) const
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