// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef BUFFER_H
#define BUFFER_H

#include <Common\BytesOf.h>
#include <MemoryManagement\Allocator\AllocatorBase.h>

namespace Engine
{
	using namespace Common;
	using namespace MemoryManagement::Allocator;

	namespace Containers
	{
		class CONTAINERS_API Buffer
		{
		public:
			Buffer(AllocatorBase *Allocator, const uint64 &Capacity) :
				m_Allocator(Allocator)
			{
				Allocate(Capacity);
			}

			Buffer(AllocatorBase *Allocator, const byte *const Buffer, const uint64 &Size) :
				Buffer(Allocator, Buffer, 0, Size)
			{
			}

			Buffer(AllocatorBase *Allocator, const byte *const Buffer, const uint64 &Index, const uint64 &Size) :
				m_Allocator(Allocator)
			{
				Copy(Buffer, Index, Size);
			}

			Buffer(const Buffer &Buffer) :
				Buffer(Buffer.m_Allocator, Buffer.m_Buffer, Buffer.m_Size)
			{
			}

			Buffer(Buffer &&Buffer)
			{
				Move(Buffer);
			}

			~Buffer(void)
			{
				Deallocate();
			}

			template<typename T>
			void WriteValue(T Value, uint64 Index)
			{
				Assert(Index < m_Size, "Index must be in range of 0 to size of buffer");

				BytesOf<T> value;
				value.Value = Value;

				for (uint32 i = 0; i < sizeof(T); ++i)
					m_Buffer[Index++] = value.Bytes[i];
			}

			template<typename T>
			T ReadValue(uint64 Index)
			{
				Assert(Index < m_Size, "Index must be in range of 0 to size of buffer");

				BytesOf<T> value;

				for (uint32 i = 0; i < sizeof(T); ++i)
					value.Bytes[i] = m_Buffer[Index++];

				return value.Value;
			}

			INLINE Buffer &operator = (const Buffer &Buffer)
			{
				m_Allocator = Buffer.m_Allocator;
				Copy(Buffer.m_Buffer, 0, Buffer.m_Size);
				return *this;
			}

			INLINE Buffer &operator = (Buffer &&Buffer)
			{
				Move(Buffer);
				return *this;
			}

			INLINE byte &operator [] (const uint64 &Index)
			{
				Assert(m_Buffer != nullptr, "Buffer is null");
				Assert(Index < m_Size, "Index must be in range of 0 to size of buffer");

				return m_Buffer[Index];
			}

			INLINE byte operator [] (const uint64 &Index) const
			{
				Assert(m_Buffer != nullptr, "Buffer is null");
				Assert(Index < m_Size, "Index must be in range of 0 to size of buffer");

				return m_Buffer[Index];
			}

			INLINE AllocatorBase *GetAllocator(void)
			{
				return m_Allocator;
			}

			INLINE uint64 &GetSize(void)
			{
				return m_Size;
			}

			INLINE const uint64 &GetSize(void) const
			{
				return m_Size;
			}

			INLINE byte *GetBuffer(void)
			{
				return m_Buffer;
			}

			INLINE const byte *const GetBuffer(void) const
			{
				return m_Buffer;
			}

		private:
			void Allocate(const uint64 &Size);
			void Deallocate(void);

			void Move(Buffer &Buffer);
			void Copy(const byte *const Buffer, const uint64 &Index, const uint64 &Size);

		private:
			AllocatorBase *m_Allocator;
			byte *m_Buffer;
			uint64 m_Size;
		};
	}
}

#endif