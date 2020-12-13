// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef QUEUE_H
#define QUEUE_H

#include <Common\PrimitiveTypes.h>
#include <Platform\PlatformMemory.h>
#include <Containers\Private\ContainersAllocators.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;
	using namespace MemoryManagement::Allocator;

	namespace Containers
	{
		using namespace Private;

		template<typename T>
		class Queue
		{
		public:
			typedef T ItemType;

			class ConstIterator;

			class Iterator
			{
			public:
				Iterator(T* Pointer) :
					m_Pointer(Pointer)
				{
				}

				INLINE Iterator operator++(void)
				{
					++m_Pointer;
					return *this;
				}

				INLINE Iterator operator--(void)
				{
					--m_Pointer;
					return *this;
				}

				INLINE Iterator operator+=(int32 Count)
				{
					m_Pointer += Count;
					return *this;
				}

				INLINE Iterator operator-=(int32 Count)
				{
					m_Pointer -= Count;
					return *this;
				}

				INLINE bool operator==(const Iterator& Other) const
				{
					return (m_Pointer == Other.m_Pointer);
				}

				INLINE bool operator!=(const Iterator& Other) const
				{
					return !operator==(Other);
				}

				INLINE T& operator*(void)
				{
					return *m_Pointer;
				}

				INLINE operator ConstIterator(void)
				{
					return ConstIterator(m_Pointer);
				}

			private:
				T* m_Pointer;
			};

			class ConstIterator
			{
			public:
				ConstIterator(T* Pointer) :
					m_Pointer(Pointer)
				{
				}

				INLINE ConstIterator operator++(void)
				{
					++m_Pointer;
					return *this;
				}

				INLINE ConstIterator operator--(void)
				{
					--m_Pointer;
					return *this;
				}

				INLINE ConstIterator operator+=(int32 Count)
				{
					m_Pointer += Count;
					return *this;
				}

				INLINE ConstIterator operator-=(int32 Count)
				{
					m_Pointer -= Count;
					return *this;
				}

				INLINE bool operator==(const ConstIterator& Other) const
				{
					return (m_Pointer == Other.m_Pointer);
				}

				INLINE bool operator!=(const ConstIterator& Other) const
				{
					return !operator==(Other);
				}

				INLINE const T& operator*(void) const
				{
					return *m_Pointer;
				}

			private:
				T* m_Pointer;
			};

		public:
			Queue(uint32 Capacity = 0) :
				m_Capacity(0),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(nullptr)
			{
				Reacllocate(Capacity);
			}

			Queue(AllocatorBase* Allocator, uint32 Capacity = 0) :
				m_Capacity(0),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(Allocator)
			{
				Reacllocate(Capacity);
			}

			Queue(const Queue<T>& Other) :
				m_Capacity(0),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(Other.m_Allocator)
			{
				Copy(Other);
			}

			Queue(AllocatorBase* Allocator, const Queue<T>& Other) :
				m_Capacity(0),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(Allocator)
			{
				Copy(Other);
			}

			Queue(Queue<T>&& Other) :
				m_Capacity(0),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(Other.m_Allocator)
			{
				*this = std::move(Other);
			}

			Queue(T* Items, uint32 Count) :
				m_Capacity(0),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(nullptr)
			{
				Copy(Items, 0, Count);
			}

			Queue(T* Items, uint32 Index, uint32 Count) :
				m_Capacity(0),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(nullptr)
			{
				Copy(Items, Index, Count);
			}

			Queue(AllocatorBase* Allocator, T* Items, uint32 Index, uint32 Count) :
				m_Capacity(0),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(Allocator)
			{
				Copy(Items, Index, Count);
			}

			~Queue(void)
			{
				Clear();

				Deallocate();
			}

			INLINE void Enqueue(const T& Item)
			{
				uint32 index = Extend(1);

				m_Items[index] = Item;
			}

			INLINE void Peek(T* Item)
			{
				Assert(m_Size > 0, "m_Size must be greater than 0");

				*Item = m_Items[0];
			}

			INLINE void Dequeue(T* Item)
			{
				Assert(m_Size > 0, "m_Size must be greater than 0");

				*Item = m_Items[0];

				if (m_Size > 1)
					PlatformMemory::Copy(m_Items, 1, m_Items, 0, m_Size - 1);

				--m_Size;
			}

			INLINE void Clear(void)
			{
				for (uint32 i = 0; i < m_Size; ++i)
					Destruct(&m_Items[i]);

				m_Size = 0;
			}

			INLINE bool Contains(const T& Item) const
			{
				for (uint32 i = 0; i < m_Size; ++i)
					if (m_Items[i] == Item)
						return true;

				return false;
			}

			INLINE void Recap(uint32 Count)
			{
				if (Count <= m_Capacity)
					return;

				Reacllocate(Count);
			}

			INLINE uint32 Extend(uint32 Count)
			{
				uint32 index = 0;

				if (m_Size + Count <= m_Capacity)
				{
					m_Size += Count;

					index = m_Size - Count;
				}
				else
				{
					Reacllocate(m_Capacity + (Count - (m_Capacity - m_Size)));

					m_Size = m_Capacity;

					index = m_Capacity - Count;
				}

				for (uint32 i = index; i < m_Size; ++i)
					Construct(&m_Items[i]);

				return index;
			}

			INLINE Iterator GetBegin(void)
			{
				return Iterator(m_Items);
			}

			INLINE ConstIterator GetBegin(void) const
			{
				return ConstIterator(m_Items);
			}

			INLINE Iterator GetEnd(void)
			{
				return Iterator(m_Items + m_Size);
			}

			INLINE ConstIterator GetEnd(void) const
			{
				return ConstIterator(m_Items + m_Size);
			}

			INLINE Iterator begin(void)
			{
				return GetBegin();
			}

			INLINE ConstIterator begin(void) const
			{
				return GetBegin();
			}

			INLINE Iterator end(void)
			{
				return GetEnd();
			}

			INLINE ConstIterator end(void) const
			{
				return GetEnd();
			}

			INLINE Queue<T>& operator=(const Queue<T>& Other)
			{
				if (m_Items != nullptr && m_Items == Other.m_Items)
					return *this;

				Copy(Other);

				return *this;
			}

			INLINE Queue<T>& operator=(Queue<T>&& Other)
			{
				if (m_Items != nullptr && m_Items == Other.m_Items)
					return *this;

				Deallocate();

				m_Capacity = Other.m_Capacity;
				m_Size = Other.m_Size;
				m_Items = Other.m_Items;
				m_Allocator = Other.m_Allocator;

				Other.m_Capacity = 0;
				Other.m_Size = 0;
				Other.m_Items = nullptr;

				return *this;
			}

			INLINE uint32 GetCapacity(void) const
			{
				return m_Capacity;
			}

			INLINE uint32 GetSize(void) const
			{
				return m_Size;
			}

			INLINE T* GetData(void)
			{
				return m_Items;
			}

			INLINE const T* GetData(void) const
			{
				return m_Items;
			}

			INLINE AllocatorBase* GetAllocator(void) const
			{
				return m_Allocator;
			}

		private:
			INLINE void Copy(const Queue<T>& Other)
			{
				Copy(Other.m_Items, 0, Other.m_Size);
			}

			INLINE void Copy(T* Items, uint32 Index, uint32 Count)
			{
				if (m_Capacity < Count)
					Reacllocate(Count);

				m_Size = Count;

				if (Items == nullptr)
					return;

				for (uint32 i = 0; i < m_Size; ++i)
				{
					Construct(&m_Items[i]);

					m_Items[i] = Items[i + Index];
				}
			}

			INLINE void Reacllocate(uint32 Count)
			{
				if (m_Allocator == nullptr)
				{
					ContainersAllocators::Create();
					m_Allocator = ContainersAllocators::QueueAllocator;
				}

				if (Count == 0)
				{
					m_Capacity = 0;
					return;
				}

				m_Items = ReinterpretCast(T*, ReallocateMemory(m_Allocator, m_Items, Count * sizeof(T)));

				if (m_Capacity < Count)
					PlatformMemory::Set(m_Items + m_Capacity, 0, Count - m_Capacity);

				m_Capacity = Count;
			}

			INLINE void Deallocate(void)
			{
				if (m_Allocator == nullptr)
					return;

				if (m_Items == nullptr)
					return;

				DeallocateMemory(m_Allocator, m_Items);

				m_Capacity = 0;
				m_Items = nullptr;
			}

		private:
			uint32 m_Capacity;
			uint32 m_Size;
			T* m_Items;
			AllocatorBase* m_Allocator;
		};
	}
}

#endif