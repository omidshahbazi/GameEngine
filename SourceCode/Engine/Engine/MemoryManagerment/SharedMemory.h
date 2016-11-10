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
		template <typename T> class MEMORYMANAGERMENT_API SharedMemory
		{
			template <typename T, typename... ArgumentTypes> friend SharedMemory<T> NewSharedMemory(Allocator::AllocatorBase &Allocator, ArgumentTypes&&... Arguments);

		private:
			struct MEMORYMANAGERMENT_API Block : public ReferenceCountedInfo
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

			private:
				Allocator::AllocatorBase *m_Allocator;
			};

		private:
			SharedMemory<T>(Block *Block)
			{
				Assert(Block != nullptr, "");

				m_Data = reinterpret_cast<T*>((byte*)Block + sizeof(SharedMemory<T>::Block));
			}

		public:
			SharedMemory<T>(SharedMemory<T> &Other) :
				m_Data(nullptr)
			{
				AssignData(Other.m_Data);
				Grab();
			}

			SharedMemory<T>(SharedMemory<T> &&Other) :
				m_Block(nullptr)
			{
				AssignData(Other.m_Data);
			}

			~SharedMemory<T>(void)
			{
				Drop();
			}

			SharedMemory<T> & operator =(const SharedMemory<T> &Other)
			{
				AssignData(Other.m_Data);
				Grab();

				return *this;
			}

			SharedMemory<T> & operator =(const SharedMemory<T> &&Other)
			{
				AssignData(Other.m_Data);

				return *this;
			}

			SharedMemory<T> & operator =(const T &Value)
			{
				AssignValue(Value);

				return *this;
			}

			SharedMemory<T> & operator =(const T &&Value)
			{
				AssignValue(Value);

				return *this;
			}

			bool operator ==(const T &Value) const
			{
				return (*m_Data == Value);
			}

			bool operator ==(const SharedMemory<T> &Other) const
			{
				return (m_Data == Other.m_Data);
			}

			bool operator !=(const T &Value) const
			{
				return (*m_Data != Value);
			}

			bool operator !=(const SharedMemory<T> &Other) const
			{
				return (m_Data != Other.m_Data);
			}

			T * operator ->(void)
			{
				return m_Data;
			}

			const T * operator ->(void) const
			{
				return m_Data;
			}

			operator T*()
			{
				return *m_Data;
			}

			operator T&()
			{
				return m_Data;
			}

			operator const T&() const
			{
				return m_Data;
			}

		private:
			void AssignData(T *Data)
			{
				Assert(Data != nullptr, "");

				if (m_Data != nullptr)
					Drop();

				m_Data = Data;
			}

			void AssignValue(const T &Value)
			{
				*m_Data = Value;
			}

			void Grab(void)
			{
				GetBlock()->Grab();
			}

			void Drop(void)
			{
				GetBlock()->Drop();

				m_Data = nullptr;
			}

			Block *GetBlock(void)
			{
				Assert(m_Data != nullptr, "");

				return reinterpret_cast<Block*>((byte*)m_Data - sizeof(Block));
			}

		private:
			T *m_Data;
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

			return SharedMemory<T>(reinterpret_cast<SharedMemory<T>::Block*>(p));
		}
	}
}

#endif