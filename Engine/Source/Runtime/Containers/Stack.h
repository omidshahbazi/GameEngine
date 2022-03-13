// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef STACK_H
#define STACK_H

#include <Common\PrimitiveTypes.h>
#include <Platform\PlatformMemory.h>
#include <Containers\Private\ContainersAllocators.h>
#include <functional>

namespace Engine
{
	using namespace Common;
	using namespace Platform;
	using namespace Allocators;

	namespace Containers
	{
		using namespace Private;

		template<typename T>
		class Stack
		{
		public:
			typedef T ItemType;

			typedef std::function<bool(const T& Item)> FindFunction;

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
			Stack(uint32 Capacity = 0) :
				m_Capacity(0),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(nullptr)
			{
				Reacllocate(Capacity);
			}

			Stack(AllocatorBase* Allocator, uint32 Capacity = 0) :
				m_Capacity(0),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(Allocator)
			{
				Reacllocate(Capacity);
			}

			Stack(const Stack<T>& Other) :
				m_Capacity(0),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(Other.m_Allocator)
			{
				Copy(Other);
			}

			Stack(AllocatorBase* Allocator, const Stack<T>& Other) :
				m_Capacity(0),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(Allocator)
			{
				Copy(Other);
			}

			Stack(Stack<T>&& Other) :
				m_Capacity(0),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(Other.m_Allocator)
			{
				*this = std::move(Other);
			}

			Stack(T* Items, uint32 Count) :
				m_Capacity(0),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(nullptr)
			{
				Copy(Items, 0, Count);
			}

			Stack(T* Items, uint32 Index, uint32 Count) :
				m_Capacity(0),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(nullptr)
			{
				Copy(Items, Index, Count);
			}

			Stack(AllocatorBase* Allocator, T* Items, uint32 Index, uint32 Count) :
				m_Capacity(0),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(Allocator)
			{
				Copy(Items, Index, Count);
			}

			~Stack(void)
			{
				Deallocate();
			}

			INLINE void Push(const T& Item)
			{
				Extend(1);

				for (uint32 i = m_Size - 1; i != 0; --i)
					m_Items[i] = std::move(m_Items[i - 1]);

				m_Items[0] = Item;
			}

			INLINE void Peek(T* Item)
			{
				HardAssert(m_Size > 0, "m_Size must be greater than 0");
				HardAssert(Item != nullptr, "Item cannot be null");

				*Item = m_Items[0];
			}

			INLINE void Pop(T* Item = nullptr)
			{
				HardAssert(m_Size > 0, "m_Size must be greater than 0");

				if (Item != nullptr)
					*Item = std::move(m_Items[0]);

				Destruct(&m_Items[0]);

				for (uint32 i = 1; i < m_Size; ++i)
					m_Items[i - 1] = std::move(m_Items[i]);

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
				return ContainsIf([](auto item) { return Item == item; });
			}

			INLINE bool ContainsIf(FindFunction Function) const
			{
				for (uint32 i = 0; i < m_Size; ++i)
					if (Function(m_Items[i]))
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

			INLINE Stack<T>& operator=(const Stack<T>& Other)
			{
				if (m_Items != nullptr && m_Items == Other.m_Items)
					return *this;

				Copy(Other);

				return *this;
			}

			INLINE Stack<T>& operator=(Stack<T>&& Other)
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
			INLINE void Copy(const Stack<T>& Other)
			{
				Copy(Other.m_Items, 0, Other.m_Size);
			}

			INLINE void Copy(T* Items, uint32 Index, uint32 Count)
			{
				Clear();

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
					m_Allocator = ContainersAllocators::StackAllocator;
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
				Clear();

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