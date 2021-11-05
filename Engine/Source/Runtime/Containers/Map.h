// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef MAP_H
#define MAP_H

#include <Containers\Pair.h>
#include <Platform\PlatformMemory.h>
#include <Containers\Private\ContainersAllocators.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;
	using namespace Allocators;

	namespace Containers
	{
		using namespace Private;

		template<typename K, typename V>
		class Map
		{
		public:
			typedef K KeyType;
			typedef V ValueType;
			typedef Pair<K, V> PairType;

			class ConstIterator;

			class Iterator
			{
			public:
				Iterator(PairType* Pointer) :
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

				INLINE PairType& operator*(void)
				{
					return *m_Pointer;
				}

				INLINE operator ConstIterator(void)
				{
					return ConstIterator(m_Pointer);
				}

			private:
				PairType* m_Pointer;
			};

			class ConstIterator
			{
			public:
				ConstIterator(PairType* Pointer) :
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

				INLINE const PairType& operator*(void) const
				{
					return *m_Pointer;
				}

			private:
				PairType* m_Pointer;
			};

		public:
			Map(uint32 Capacity = 0) :
				m_Capacity(0),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(nullptr)
			{
				Reacllocate(m_Capacity);
			}

			Map(AllocatorBase* Allocator, uint32 Capacity = 0) :
				m_Capacity(0),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(Allocator)
			{
				Reacllocate(m_Capacity);
			}

			Map(const Map<K, V>& Other) :
				m_Capacity(0),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(Other.m_Allocator)
			{
				Copy(Other);
			}

			Map(AllocatorBase* Allocator, const Map<K, V>& Other) :
				m_Capacity(0),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(Allocator)
			{
				Copy(Other);
			}

			Map(Map<K, V>&& Other) :
				m_Capacity(0),
				m_Size(0),
				m_Items(nullptr),
				m_Allocator(Other.m_Allocator)
			{
				*this = std::move(Other);
			}

			~Map(void)
			{
				Deallocate();
			}

			INLINE PairType& Add(const K& Key, const V& Value)
			{
				HardAssert(!Contains(Key), "Key already added");

				uint32 index = Extend(1);

				m_Items[index] = PairType(Key, Value);

				return m_Items[index];
			}

			INLINE void Add(const PairType& Pair)
			{
				HardAssert(!Contains(Pair.GetFirst()), "Key already added");

				uint32 index = Extend(1);

				m_Items[index] = Pair;
			}

			INLINE void Remove(const K& Key)
			{
				int32 index = Find(Key);

				HardAssert(index != -1, "Key not found");

				Destruct(&m_Items[index]);

				int indexToMove = index + 1;
				if (indexToMove < m_Size)
					PlatformMemory::Copy(m_Items, indexToMove, m_Items, index, m_Size - indexToMove);
				--m_Size;
			}

			INLINE V& Get(const K& Key)
			{
				int32 index = Find(Key);

				HardAssert(index != -1, "Key not found");

				return m_Items[index].GetSecond();
			}

			INLINE const V& Get(const K& Key) const
			{
				return Get(Key);
			}

			INLINE void Clear(void)
			{
				for (uint32 i = 0; i < m_Size; ++i)
					Destruct(&m_Items[i]);

				m_Size = 0;
			}

			INLINE bool Contains(const K& Key) const
			{
				return (Find(Key) != -1);
			}

			INLINE V& operator[](const K& Key)
			{
				int32 index = Find(Key);

				if (index == -1)
					return Add(Key, V()).GetSecond();

				return m_Items[index].GetSecond();
			}

			INLINE const V& operator[](const K& Key) const
			{
				int32 index = Find(Key);

				HardAssert(index != -1, "Key not found");

				return m_Items[index].GetSecond();
			}

			INLINE Map<K, V>& operator=(const Map<K, V>& Other)
			{
				if (m_Items != nullptr && m_Items == Other.m_Items)
					return *this;

				Copy(Other);

				return *this;
			}

			INLINE Map<K, V>& operator=(Map<K, V>&& Other)
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

			INLINE uint32 GetCapacity(void) const
			{
				return m_Capacity;
			}

			INLINE uint32 GetSize(void) const
			{
				return m_Size;
			}

			INLINE AllocatorBase* GetAllocator(void) const
			{
				return m_Allocator;
			}

		private:
			INLINE int32 Find(const K& Key) const
			{
				for (int32 i = 0; i < m_Size; ++i)
					if (m_Items[i].GetFirst() == Key)
						return i;

				return -1;
			}

			INLINE void Copy(const Map<K, V>& Other)
			{
				Clear();

				if (m_Capacity < Other.m_Size)
					Reacllocate(Other.m_Size);

				m_Size = Other.m_Size;

				if (Other.m_Items == nullptr)
					return;

				for (uint32 i = 0; i < m_Size; ++i)
					m_Items[i] = Other.m_Items[i];
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
					//Reacllocate(m_Capacity + Count);
					Reacllocate(m_Capacity + (Count - (m_Capacity - m_Size)));

					m_Size = m_Capacity;

					index = m_Capacity - Count;
				}

				for (uint32 i = index; i < m_Size; ++i)
					Construct(&m_Items[i]);

				return index;
			}

			INLINE void Reacllocate(uint32 Count)
			{
				if (m_Allocator == nullptr)
				{
					ContainersAllocators::Create();
					m_Allocator = ContainersAllocators::MapAllocator;
				}

				if (Count == 0)
				{
					m_Capacity = 0;
					return;
				}

				m_Items = ReinterpretCast(PairType*, ReallocateMemory(m_Allocator, m_Items, Count * sizeof(PairType)));

				if (m_Capacity < Count)
					PlatformMemory::Set(m_Items + m_Capacity, 0, Count - m_Capacity);

				m_Capacity = Count;
			}

			INLINE void Deallocate(void)
			{
				if (m_Items == nullptr)
					return;

				Clear();

				DeallocateMemory(m_Allocator, m_Items);

				m_Capacity = 0;

				m_Items = nullptr;
			}

		private:
			uint32 m_Capacity;
			uint32 m_Size;
			PairType* m_Items;
			AllocatorBase* m_Allocator;
		};
	}
}

#endif