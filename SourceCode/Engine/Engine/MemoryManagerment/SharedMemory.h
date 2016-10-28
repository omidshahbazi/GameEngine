// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <MemoryManagerment\ReferenceCounted.h>
#include <MemoryManagerment\HandleInfo.h>
#include <Debugging\Debug.h>
#include <utility>

#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

namespace Engine
{
	using namespace Debugging;

	namespace MemoryManagement
	{
		template <typename T> class SharedMemory
		{
		private:
			struct Block
			{
			public:
#if _DEBUG
				Block(void) :
					m_Address(&Get())
				{
				}
#endif

				T &Get(void)
				{
					return *(T*)(this + sizeof(Block));
				}

#if _DEBUG
			private:
				T *m_Address;
#endif
			};

		public:
			SharedMemory(void) :
				m_Block(nullptr)
			{
			}

			SharedMemory(SharedMemory &Other) :
				m_Block(nullptr)
			{
				m_Block = Other.m_Block;
				m_Block->Grab();
			}

			SharedMemory(SharedMemory &&Other) :
				m_Block(nullptr)
			{
				m_Block = Other.m_Block;
			}

			SharedMemory(const T &Value) :
				m_Block(nullptr)
			{
				AssignValue(Value);
			}

			SharedMemory(const T &&Value) :
				m_Block(nullptr)
			{
				AssignValue(Value);
			}

			~SharedMemory(void)
			{
				m_Block->Drop();
			}

			SharedMemory & operator =(const SharedMemory &Other)
			{
				if (m_Block != nullptr)
					m_Block->Drop();

				m_Block = Other.m_Block;
				m_Block->Grab();

				return *this;
			}

			SharedMemory & operator =(const SharedMemory &&Other)
			{
				if (m_Block != nullptr)
					m_Block->Drop();

				m_Block = Other.m_Block;
				m_Block->Grab();

				return *this;
			}

			SharedMemory & operator =(const T &Value)
			{
				AssignValue(Value);

				return *this;
			}

			SharedMemory & operator =(const T &&Value)
			{
				AssignValue(Value);

				return *this;
			}

			bool operator ==(const T &Value) const
			{
				return (m_Block->Get<Block>()->Get() == Value);
			}

			bool operator ==(const SharedMemory &Other) const
			{
				return (m_Block->Get<Block>()->Get() == Other.m_Block->Get<Block>()->Get());
			}

			bool operator !=(const T &Value) const
			{
				return (m_Block->Get<Block>()->Get() != Value);
			}

			bool operator !=(const SharedMemory &Other) const
			{
				return (m_Block->Get<Block>()->Get() == Other.m_Block->Get<Block>()->Get());
			}

			T * operator ->(void)
			{
				return &m_Block->Get<Block>()->Get();
			}

			const T * operator ->(void) const
			{
				return &m_Block->Get<Block>()->Get();
			}

			operator T*()
			{
				return &m_Block->Get<Block>()->Get();
			}

			operator T&()
			{
				return m_Block->Get<Block>()->Get();
			}

			operator const T&() const
			{
				return m_Block->Get<Block>()->Get();
			}

		private:
			void AssignValue(const T &Value)
			{
				if (m_Block == nullptr)
					m_Block = DefaultAllocator::GetInstance().Allocate(sizeof(Block) + sizeof(T));

				new (m_Block->Get()) Block();

				Assert(m_Block != nullptr, "");

				m_Block->Get<Block>()->Get() = Value;
			}

		private:
			HandleInfo *m_Block;
		};

		//template <typename T, typename... ArgumentTypes> SharedMemory<T, Allocator> NewSharedMemory(ArgumentTypes&&... Arguments, const AllocatorBase &Allocator = DefaultAllocator::GetInstance())
		//{
		//	byte * p = Allocator.Allocate(sizeof(T));

		//	try
		//	{
		//		new (p) T(std::forward<ArgumentTypes>(Arguments)...);
		//	}
		//	catch (...)
		//	{
		//		//Allocator::Deallocate(p);
		//		throw;
		//	}

		//	return SharedMemory<T, Allocator>(*reinterpret_cast<T*>(p));
		//}
	}
}

#endif