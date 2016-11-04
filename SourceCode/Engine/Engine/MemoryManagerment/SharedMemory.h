// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <MemoryManagerment\ReferenceCountedInfo.h>
#include <MemoryManagerment\Allocator\AllocatorBase.h>
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
			template <typename T, typename... ArgumentTypes> friend SharedMemory<T> NewSharedMemory(Allocator::AllocatorBase &Allocator, ArgumentTypes&&... Arguments);

		private:
			struct Block : public ReferenceCountedInfo
			{
			public:
				Block(Allocator::AllocatorBase *Allocator) :
					m_Allocator(Allocator)
				{
				}

				void Drop(void) override
				{
					ReferenceCountedInfo::Drop();

					if (GetCount() == 0)
						m_Allocator->Deallocate((byte*)this);
				}

				T &Get(void)
				{
					return *(T*)(this + sizeof(Block));
				}

			private:
				Allocator::AllocatorBase *m_Allocator;
			};

		private:
			SharedMemory(Block *Block) :
				m_Block(Block)
			{
			}

		public:
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
				return (m_Block->Get() == Value);
			}

			bool operator ==(const SharedMemory &Other) const
			{
				return (m_Block->Get() == Other.m_Block->Get());
			}

			bool operator !=(const T &Value) const
			{
				return (m_Block->Get() != Value);
			}

			bool operator !=(const SharedMemory &Other) const
			{
				return (m_Block->Get() == Other.m_Block->Get());
			}

			T * operator ->(void)
			{
				return &m_Block->Get();
			}

			const T * operator ->(void) const
			{
				return &m_Block->Get();
			}

			operator T*()
			{
				return &m_Block->Get();
			}

			operator T&()
			{
				return m_Block->Get();
			}

			operator const T&() const
			{
				return m_Block->Get();
			}

		private:
			void AssignValue(const T &Value)
			{
				Assert(m_Block != nullptr, "");

				m_Block->Get() = Value;
			}

		private:
			Block *m_Block;
		};

		template <typename T, typename... ArgumentTypes> SharedMemory<T> NewSharedMemory(Allocator::AllocatorBase &Allocator, ArgumentTypes&&... Arguments)
		{
			uint8 blockSize = sizeof(SharedMemory<T>::Block);

			byte * p = Allocator.Allocate(blockSize + sizeof(T));

			try
			{
				new (p + blockSize) T(std::forward<ArgumentTypes>(Arguments)...);

				new (p) SharedMemory<T>::Block(&Allocator);
			}
			catch (...)
			{
				Assert(false, "Cannot call constructor");
			}

			return SharedMemory<T>((SharedMemory<T>::Block*)p);
		}
	}
}

#endif