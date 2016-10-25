// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include "PrimitiveTypes.h"
#include <memory>

#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

template <typename T> class SharedMemory
{
private:
	struct Block
	{
	public:
		Block(uint32 Size) :
			m_Address((T*)malloc(Size)),
			m_ReferenceCount(1)
		{
		}

		void Grab(void)
		{
			++m_ReferenceCount;
		}

		void Drop(void)
		{
			if (--m_ReferenceCount != 0)
				return;

			free(m_Address);

			m_Address = nullptr;
		}

		T &Get(void)
		{
			return *m_Address;
		}

	private:
		T *m_Address;
		uint32 m_ReferenceCount;
	};

public:
	SharedMemory(void)
	{
		m_Block = new Block(sizeof(T));
	}

	SharedMemory(SharedMemory &Other)
	{
		m_Block = Other.m_Block;
		m_Block->Grab();
	}

	~SharedMemory(void)
	{
		m_Block->Drop();
	}

	SharedMemory<T> & operator =(const T &Value)
	{
		m_Block->Get() = Value;

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
	Block *m_Block;
};

#endif