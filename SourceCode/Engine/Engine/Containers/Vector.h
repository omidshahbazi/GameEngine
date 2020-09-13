// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef VECTOR_H
#define VECTOR_H

#include <Common\PrimitiveTypes.h>
#include <Platform\PlatformMemory.h>
#include <Containers\Private\ContainersAllocators.h>
#include <functional>

namespace Engine
{
	using namespace Common;
	using namespace Platform;
	using namespace MemoryManagement::Allocator;

	namespace Containers
	{
		using namespace Private;

		template<typename T>
		class Vector
		{
		public:
			typedef T ItemType;

			typedef std::function<bool(const T& A, const T& B)> SortFunction;

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
			Vector(uint32 Capacity = 0) :
				m_Capacity(Capacity),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(nullptr)
			{
				if (m_Capacity != 0)
					Reacllocate(m_Capacity);
			}

			Vector(AllocatorBase* Allocator, uint32 Capacity = 0) :
				m_Capacity(Capacity),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(Allocator)
			{
				if (m_Capacity != 0)
					Reacllocate(m_Capacity);
			}

			Vector(const Vector<T>& Other) :
				m_Capacity(0),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(Other.m_Allocator)
			{
				Copy(Other);
			}

			Vector(AllocatorBase* Allocator, const Vector<T>& Other) :
				m_Capacity(0),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(Allocator)
			{
				Copy(Other);
			}

			Vector(Vector<T>&& Other) :
				m_Capacity(0),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(Other.m_Allocator)
			{
				*this = std::move(Other);
			}

			Vector(T* Items, uint32 Size) :
				m_Capacity(Size),
				m_Size(Size),
				m_Items(Items),
				m_Allocator(nullptr)
			{
			}

			Vector(T* Items, uint32 Index, uint32 Size) :
				m_Capacity(Size),
				m_Size(Size),
				m_Items(Items + Index),
				m_Allocator(nullptr)
			{
			}

			~Vector(void)
			{
				Clear();

				Deallocate();
			}

			INLINE void Add(const T& Item)
			{
				uint32 index = Extend(1);

				m_Items[index] = Item;
			}

			INLINE void AddRange(const T* const Items, uint32 Count)
			{
				AddRange(Items, 0, Count);
			}

			INLINE void AddRange(const T* const Items, uint32 Index, uint32 Count)
			{
				if (Count == 0)
					return;

				uint32 index = Extend(Count);

				for (uint32 i = 0; i < Count; ++i)
					m_Items[index + i] = Items[Index + i];
			}

			INLINE void AddRange(const Vector<T>& Other)
			{
				if (Other.m_Size == 0)
					return;

				AddRange(Other.m_Items, Other.m_Size);
			}

			INLINE void Insert(uint32 Index, const T& Item)
			{
				Assert(m_Size == 0 || Index < m_Size, "Index cannot be greater-equal with m_Size");

				Extend(1);

				int copyIndex = Index + 1;
				int copySize = m_Size - copyIndex;

				if (copySize != 0)
					PlatformMemory::Copy(m_Items, Index, m_Items, copyIndex, copySize);

				m_Items[Index] = Item;
			}

			INLINE void Remove(const T& Item)
			{
				uint32 index = 0;
				while ((index = Find(Item)) != -1)
					RemoveAt(index);
			}

			INLINE void RemoveAt(uint32 Index)
			{
				Assert(Index < m_Size, "Index cannot be greater-equal with m_Size");

				Destruct(&m_Items[Index]);

				int indexToMove = Index + 1;

				if (indexToMove < m_Size)
					PlatformMemory::Copy(m_Items, indexToMove, m_Items, Index, m_Size - indexToMove);

				--m_Size;
			}

			INLINE void Sort(SortFunction Function)
			{
				for (int32 i = 0; i < (int32)m_Size - 1; ++i)
				{
					for (int32 j = i + 1; j < (int32)m_Size; ++j)
					{
						bool result = Function(m_Items[i], m_Items[j]);

						if (!result)
							continue;

						T temp = m_Items[j];
						m_Items[j] = m_Items[i];
						m_Items[i] = temp;
					}
				}
			}

			INLINE void Clear(void)
			{
				for (uint32 i = 0; i < m_Size; ++i)
					Destruct(&m_Items[i]);

				m_Size = 0;
			}

			INLINE int32 Find(const T& Item) const
			{
				for (uint32 i = 0; i < m_Size; ++i)
					if (m_Items[i] == Item)
						return i;

				return -1;
			}

			INLINE bool Contains(const T& Item) const
			{
				return (Find(Item) != -1);
			}

			INLINE void Recap(uint32 Count)
			{
				if (Count <= m_Capacity)
					return;

				Reacllocate(Count);
			}

			INLINE uint32 Extend(uint32 Count)
			{
				if (m_Size + Count <= m_Capacity)
				{
					m_Size += Count;

					return (m_Size - Count);
				}

				//Reacllocate(m_Capacity + Count);
				Reacllocate(m_Capacity + (Count - (m_Capacity - m_Size)));

				m_Size = m_Capacity;

				return (m_Capacity - Count);
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

			INLINE Vector<T>& operator=(const Vector<T>& Other)
			{
				if (m_Items != nullptr && m_Items == Other.m_Items)
					return *this;

				Copy(Other);

				return *this;
			}

			INLINE Vector<T>& operator=(Vector<T>&& Other)
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

			INLINE T& operator[](uint32 Index)
			{
				Assert(Index < m_Size, "Index cannot be greater-equal with m_Size");

				return m_Items[Index];
			}

			INLINE  const T& operator[](uint32 Index) const
			{
				Assert(Index < m_Size, "Index cannot be greater-equal with m_Size");

				return m_Items[Index];
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
			INLINE void Copy(const Vector<T>& Other)
			{
				if (m_Capacity < Other.m_Size)
				{
					Deallocate();

					m_Capacity = Other.m_Size;

					m_Items = Allocate(m_Capacity);
				}

				m_Size = Other.m_Size;

				if (Other.m_Items == nullptr)
					return;

				for (uint32 i = 0; i < m_Size; ++i)
					m_Items[i] = Other.m_Items[i];
			}

			INLINE void Reacllocate(uint32 Count)
			{
				T* newMem = Allocate(Count);

				if (m_Items == nullptr)
				{
					m_Capacity = Count;
					m_Items = newMem;
					return;
				}

				PlatformMemory::Copy(m_Items, newMem, m_Size);

				Deallocate();

				m_Capacity = Count;
				m_Items = newMem;
			}

			INLINE T* Allocate(uint32 Count)
			{
				if (Count == 0)
					return nullptr;

				if (m_Allocator == nullptr)
				{
					ContainersAllocators::Create();
					m_Allocator = ContainersAllocators::VectorAllocator;
				}

				uint32 size = Count * sizeof(T);
				byte* block = AllocateMemory(m_Allocator, size);

				PlatformMemory::Set(block, 0, size);

				return ReinterpretCast(T*, block);
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