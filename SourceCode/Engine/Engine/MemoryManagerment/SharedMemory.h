// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <MemoryManagerment\ReferenceCounted.h>
#include <Debugging\Debug.h>
#include <utility>

//using namespace Engine::Debugging;

#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

//namespace Engine::MemoryManagement
//{
	template <typename T, typename Allocator = DefaultAllocator> class SharedMemory
	{
	private:
		struct Block : public ReferenceCounted<Allocator>
		{
		public:
			Block(uint32 Size) 
				//: m_Address((T*)Allocator::Allocate(Size))
			{
			}

			virtual void Drop(void) override
			{
				ReferenceCounted::Drop();

				m_Address = nullptr;
			}

			T &Get(void)
			{
				return *m_Address;
			}

		private:
			T *m_Address;
		};

	public:
		SharedMemory(void) :
			m_Block(nullptr)
		{
			Allocate();
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
			Allocate();

			AssignValue(Value);
		}

		SharedMemory(const T &&Value) :
			m_Block(nullptr)
		{
			Allocate();

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
		void Allocate(void)
		{
			Assert(m_Block == nullptr, "");

			m_Block = new Block(sizeof(T));
		}

		void AssignValue(const T &Value)
		{
			Assert(m_Block != nullptr, "");

			m_Block->Get() = Value;
		}

	private:
		Block *m_Block;
	};

	template <typename T, typename Allocator = DefaultAllocator, typename... ArgumentTypes> SharedMemory<T, Allocator> NewSharedMemory(ArgumentTypes&&... Arguments)
	{
		byte * p = nullptr;// = Allocator::Allocate(sizeof(T));

		try
		{
			new (p) T(std::forward<ArgumentTypes>(Arguments)...);
		}
		catch (...)
		{
			//Allocator::Deallocate(p);
			throw;
		}

		return SharedMemory<T, Allocator>(*reinterpret_cast<T*>(p));
	}
//}

#endif