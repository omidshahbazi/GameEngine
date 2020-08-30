// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef LIST_H
#define LIST_H

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

#define TRAVERSE_NODES(Name, NodeName, Number) \
		Node* Name = NodeName; \
		for (uint32 __index = 1; __index < (Number); ++__index) \
			Name = Name->Next;

#define DEFINE_NODE_AT(Name, Number) TRAVERSE_NODES(Name, m_FirstNode,  Number)

		template<typename T>
		class List
		{
		private:
			struct Node
			{
			public:
				Node* Previous;
				Node* Next;
				T Value;
			};

		public:
			typedef T ItemType;

			typedef std::function<bool(const T& A, const T& B)> SortFunction;

			class ConstIterator;

			class Iterator
			{
			public:
				Iterator(Node* Pointer, uint32 Index) :
					m_Pointer(Pointer),
					m_Index(Index)
				{
				}

				INLINE Iterator operator++(void)
				{
					++m_Index;
					return *this;
				}

				INLINE Iterator operator--(void)
				{
					--m_Index;
					return *this;
				}

				INLINE Iterator operator+=(int32 Count)
				{
					m_Index += Count;
					return *this;
				}

				INLINE Iterator operator-=(int32 Count)
				{
					m_Index -= Count;
					return *this;
				}

				INLINE bool operator==(const Iterator& Other) const
				{
					return (m_Index == Other.m_Index);
				}

				INLINE bool operator!=(const Iterator& Other) const
				{
					return !operator==(Other);
				}

				INLINE T& operator*(void)
				{
					TRAVERSE_NODES(node, m_Pointer, m_Index + 1);
					return node->Value;
				}

				INLINE operator ConstIterator(void)
				{
					return ConstIterator(m_Pointer, m_Index);
				}

			private:
				Node* m_Pointer;
				uint32 m_Index;
			};

			class ConstIterator
			{
			public:
				ConstIterator(Node* Pointer, uint32 Index) :
					m_Pointer(Pointer),
					m_Index(Index)
				{
				}

				INLINE ConstIterator operator++(void)
				{
					++m_Index;
					return *this;
				}

				INLINE ConstIterator operator--(void)
				{
					--m_Index;
					return *this;
				}

				INLINE ConstIterator operator+=(int32 Count)
				{
					m_Index += Count;
					return *this;
				}

				INLINE ConstIterator operator-=(int32 Count)
				{
					m_Index -= Count;
					return *this;
				}

				INLINE bool operator==(const ConstIterator& Other) const
				{
					return (m_Index == Other.m_Index);
				}

				INLINE bool operator!=(const ConstIterator& Other) const
				{
					return !operator==(Other);
				}

				INLINE const T& operator*(void) const
				{
					TRAVERSE_NODES(node, m_Pointer, m_Index + 1);
					return node->Value;
				}

			private:
				Node* m_Pointer;
				uint32 m_Index;
			};

		public:
			List(uint32 Capacity = 0) :
				m_Capacity(0),
				m_Size(0),
				m_FirstNode(nullptr),
				m_Allocator(nullptr)
			{
				if (Capacity != 0)
					Recap(Capacity);
			}

			List(AllocatorBase* Allocator, uint32 Capacity = 0) :
				m_Capacity(Capacity),
				m_Size(0),
				m_FirstNode(nullptr),
				m_Allocator(Allocator)
			{
				if (m_Capacity != 0)
					Recap(Capacity);
			}

			List(const List<T>& Other) :
				m_Capacity(0),
				m_Size(0),
				m_FirstNode(nullptr),
				m_Allocator(Other.m_Allocator)
			{
				Copy(Other);
			}

			List(AllocatorBase* Allocator, const List<T>& Other) :
				m_Capacity(0),
				m_Size(0),
				m_FirstNode(nullptr),
				m_Allocator(Allocator)
			{
				Copy(Other);
			}

			List(List<T>&& Other) :
				m_Capacity(0),
				m_Size(0),
				m_FirstNode(nullptr),
				m_Allocator(Other.m_Allocator)
			{
				*this = std::move(Other);
			}

			List(T* Items, uint32 Count) :
				m_Capacity(0),
				m_Size(0),
				m_FirstNode(nullptr),
				m_Allocator(nullptr)
			{
				AddRange(Items, 0, Count);
			}

			List(T* Items, uint32 Index, uint32 Size) :
				m_Capacity(0),
				m_Size(0),
				m_FirstNode(nullptr),
				m_Allocator(nullptr)
			{
				AddRange(Items, Index, Count);
			}

			~List(void)
			{
				Clear();

				Deallocate();
			}

			INLINE void Add(const T& Item)
			{
				uint32 index = Extend(1);

				DEFINE_NODE_AT(lastNode, index + 1);

				lastNode->Value = Item;
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

				DEFINE_NODE_AT(lastNode, index + 1);

				for (uint32 i = 0; i < Count; ++i)
				{
					lastNode->Value = Items[Index + i];

					lastNode = lastNode->Next;
				}
			}

			INLINE void AddRange(const List<T>& Other)
			{
				if (Other.m_Size == 0)
					return;

				uint32 otherSize = Other.m_Size;

				uint32 index = Extend(otherSize);

				DEFINE_NODE_AT(lastNode, index + 1);

				Node* otherNode = Other.m_FirstNode;
				for (uint32 i = 0; i < otherSize; ++i)
				{
					lastNode->Value = otherNode->Value;

					lastNode = lastNode->Next;
					otherNode = otherNode->Next;
				}
			}

			INLINE void Insert(uint32 Index, const T& Item)
			{
				Assert(m_Size == 0 || Index < m_Size, "Index cannot be greater-equal with m_Size");

				Node* shiftNode = nullptr;
				if (Index < m_Size)
				{
					DEFINE_NODE_AT(node, Index + 1);
					shiftNode = node;
				}

				uint32 extendedIndex = Extend(1);

				DEFINE_NODE_AT(extendedNode, extendedIndex + 1);

				extendedNode->Value = Item;

				if (shiftNode != nullptr)
				{
					extendedNode->Previous = shiftNode->Previous;

					if (shiftNode->Previous != nullptr)
						shiftNode->Previous->Next = extendedNode;

					shiftNode->Previous = extendedNode;

					if (shiftNode->Next == extendedNode)
						shiftNode->Next = nullptr;

					extendedNode->Next = shiftNode;

					if (Index == 0)
						m_FirstNode = extendedNode;
				}
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

				DEFINE_NODE_AT(endingNode, m_Capacity);

				DEFINE_NODE_AT(removedNode, Index + 1);

				Destruct(&removedNode->Value);

				PlatformMemory::Set(&removedNode->Value, 0, 1);

				if (Index == 0)
					m_FirstNode = removedNode->Next;

				if (endingNode != removedNode)
				{
					if (removedNode->Next != nullptr)
						removedNode->Next->Previous = removedNode->Previous;

					if (removedNode->Previous != nullptr)
						removedNode->Previous->Next = removedNode->Next;

					endingNode->Next = removedNode;
					removedNode->Previous = endingNode;
					removedNode->Next = nullptr;
				}

				--m_Size;
			}

			INLINE void Sort(SortFunction Function)
			{
				Node* firstNode = m_FirstNode;
				for (int32 i = 0; i < (int32)m_Size - 1; ++i)
				{
					DEFINE_NODE_AT(secondNode, i + 1 + 1);

					for (int32 j = i + 1; j < (int32)m_Size; ++j)
					{
						bool result = Function(firstNode->Value, secondNode->Value);

						if (result)
						{
							T temp = secondNode->Value;
							secondNode->Value = firstNode->Value;
							firstNode->Value = temp;
						}

						secondNode = secondNode->Next;
					}

					firstNode = firstNode->Next;
				}
			}

			INLINE void Clear(void)
			{
				Node* node = m_FirstNode;
				for (uint32 i = 0; i < m_Size; ++i)
				{
					Destruct(&node->Value);

					PlatformMemory::Set(&node->Value, 0, 1);

					node = node->Next;
				}

				m_Size = 0;
			}

			INLINE int32 Find(const T& Item) const
			{
				Node* node = m_FirstNode;
				for (uint32 i = 0; i < m_Size; ++i)
				{
					if (node->Value == Item)
						return i;

					node = node->Next;
				}

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

				Node* node = Allocate(Count - m_Capacity);

				DEFINE_NODE_AT(lastNode, m_Capacity);

				if (lastNode == nullptr)
					m_FirstNode = node;
				else
				{
					lastNode->Next = node;

					node->Previous = lastNode;
				}

				m_Capacity = Count;
			}

			INLINE uint32 Extend(uint32 Count)
			{
				if (m_Size + Count <= m_Capacity)
				{
					m_Size += Count;

					return (m_Size - Count);
				}

				Recap(m_Capacity + (Count - (m_Capacity - m_Size)));

				m_Size = m_Capacity;

				return (m_Capacity - Count);
			}

			INLINE Iterator GetBegin(void)
			{
				return Iterator(m_FirstNode, 0);
			}

			INLINE ConstIterator GetBegin(void) const
			{
				return ConstIterator(m_FirstNode, 0);
			}

			INLINE Iterator GetEnd(void)
			{
				return Iterator(m_FirstNode, m_Size);
			}

			INLINE ConstIterator GetEnd(void) const
			{
				return ConstIterator(m_FirstNode, m_Size);
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

			INLINE List<T>& operator=(const List<T>& Other)
			{
				if (m_FirstNode != nullptr && m_FirstNode == Other.m_FirstNode)
					return *this;

				Copy(Other);

				return *this;
			}

			INLINE List<T>& operator=(List<T>&& Other)
			{
				if (m_FirstNode != nullptr && m_FirstNode == Other.m_FirstNode)
					return *this;

				Deallocate();

				m_Capacity = Other.m_Capacity;
				m_Size = Other.m_Size;
				m_FirstNode = Other.m_FirstNode;
				m_Allocator = Other.m_Allocator;

				Other.m_Capacity = 0;
				Other.m_Size = 0;
				Other.m_FirstNode = nullptr;

				return *this;
			}

			INLINE T& operator[](uint32 Index)
			{
				Assert(Index < m_Size, "Index cannot be greater-equal with m_Size");

				DEFINE_NODE_AT(node, Index + 1);

				return node->Value;
			}

			INLINE  const T& operator[](uint32 Index) const
			{
				Assert(Index < m_Size, "Index cannot be greater-equal with m_Size");

				DEFINE_NODE_AT(node, Index + 1);

				return node->Value;
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
			INLINE void Copy(const List<T>& Other)
			{
				if (m_Capacity < Other.m_Size)
				{
					Deallocate();

					m_Capacity = Other.m_Size;

					m_FirstNode = Allocate(m_Capacity);
				}

				m_Size = Other.m_Size;

				if (Other.m_FirstNode == nullptr)
					return;

				Node* selfNode = m_FirstNode;
				Node* otherNode = Other.m_FirstNode;
				for (uint32 i = 0; i < m_Size; ++i)
				{
					selfNode->Value = otherNode->Value;

					selfNode = selfNode->Next;
					otherNode = otherNode->Next;
				}
			}

			INLINE Node* Allocate(uint32 Count)
			{
				if (Count == 0)
					return nullptr;

				if (m_Allocator == nullptr)
					m_Allocator = &ContainersAllocators::ListAllocator;

				Node* firstNode = ReinterpretCast(Node*, AllocateMemory(m_Allocator, sizeof(Node)));
				PlatformMemory::Set(firstNode, 0, 1);

				Node* prev = firstNode;
				for (uint32 i = 0; i < Count - 1; ++i)
				{
					Node* curr = ReinterpretCast(Node*, AllocateMemory(m_Allocator, sizeof(Node)));
					PlatformMemory::Set(curr, 0, 1);

					if (prev != nullptr)
						prev->Next = curr;

					curr->Previous = prev;

					prev = curr;
				}

				return firstNode;
			}

			INLINE void Deallocate(void)
			{
				Node* node = m_FirstNode;
				for (uint32 i = 0; i < m_Capacity; ++i)
				{
					Node* nextNode = node->Next;

					DeallocateMemory(m_Allocator, node);

					node = nextNode;
				}

				m_Capacity = 0;
			}

		private:
			uint32 m_Capacity;
			uint32 m_Size;
			Node* m_FirstNode;
			AllocatorBase* m_Allocator;
		};
	}
}

#endif