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
		template<typename T>
		class Buffer
		{
		public:
			typedef T ElementType;

		public:
			Buffer(AllocatorBase *Allocator, const uint64 &Capacity) :
				m_Allocator(Allocator)
			{
				Allocate(Capacity);
			}

			Buffer(AllocatorBase *Allocator, const T *const Buffer, const uint64 &Size) :
				Buffer(Allocator, Buffer, 0, Size)
			{
			}

			Buffer(AllocatorBase *Allocator, const T *const Buffer, const uint64 &Index, const uint64 &Size) :
				m_Allocator(Allocator)
			{
				Copy(Buffer, Index, Size);
			}

			Buffer(const Buffer<T> &Buffer) :
				Buffer(Buffer.m_Allocator, Buffer.m_Buffer, Buffer.m_Size)
			{
			}

			Buffer(Buffer<T> &&Buffer)
			{
				Move(Buffer);
			}

			~Buffer(void)
			{
				Deallocate();
			}

			template<typename V>
			void WriteValue(V Value, uint64 Index)
			{
				Assert(Index < m_Size, "Index must be in range of 0 to size of buffer");

				BytesOf<V> value;
				value.Value = Value;

				for (uint32 i = 0; i < sizeof(V); ++i)
					m_Buffer[Index++] = value.Bytes[i];
			}

			template<typename V>
			V ReadValue(uint64 Index)
			{
				Assert(Index < m_Size, "Index must be in range of 0 to size of buffer");

				BytesOf<V> value;

				for (uint32 i = 0; i < sizeof(V); ++i)
					value.Bytes[i] = m_Buffer[Index++];

				return value.Value;
			}

			INLINE Buffer &operator = (const Buffer<T> &Buffer)
			{
				m_Allocator = Buffer.m_Allocator;
				Copy(Buffer.m_Buffer, 0, Buffer.m_Size);
				return *this;
			}

			INLINE Buffer &operator = (Buffer<T> &&Buffer)
			{
				Move(Buffer);
				return *this;
			}

			INLINE T &operator [] (const uint64 &Index)
			{
				Assert(m_Buffer != nullptr, "Buffer is null");
				Assert(Index < m_Size, "Index must be in range of 0 to size of buffer");

				return m_Buffer[Index];
			}

			INLINE T operator [] (const uint64 &Index) const
			{
				Assert(m_Buffer != nullptr, "Buffer is null");
				Assert(Index < m_Size, "Index must be in range of 0 to size of buffer");

				return m_Buffer[Index];
			}

			INLINE void Append(char8 Value)
			{
				Buffer.Append(Value);
			}

			INLINE void Append(char16 Value)
			{
				Buffer.Append(Value);
			}

			INLINE void Append(int8 Value)
			{
				Buffer.Append(Value);
			}

			INLINE void Append(int16 Value)
			{
				Buffer.Append(Value);
			}

			INLINE void Append(int32 Value)
			{
				Buffer.Append(Value);
			}

			INLINE void Append(int64 Value)
			{
				Buffer.Append(Value);
			}

			INLINE void Append(uint8 Value)
			{
				Buffer.Append(Value);
			}

			INLINE void Append(uint16 Value)
			{
				Buffer.Append(Value);
			}

			INLINE void Append(uint32 Value)
			{
				Buffer.Append(Value);
			}

			INLINE void Append(const uint64 &Value)
			{
				Buffer.Append(Value);
			}

			INLINE void Append(float32 Value)
			{
				Buffer.Append(Value);
			}

			INLINE void Append(const float64 &Value)
			{
				Buffer.Append(Value);
			}

			INLINE void Append(const float128 &Value)
			{
				Buffer.Append(Value);
			}

			INLINE void Append(cstr Value)
			{
				Buffer.Append(Value);
			}

			INLINE void Append(cwstr Value)
			{
				Buffer.Append(Value);
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

			INLINE T *GetBuffer(void)
			{
				return m_Buffer;
			}

			INLINE const T *const GetBuffer(void) const
			{
				return m_Buffer;
			}

		private:
			void Allocate(const uint64 &Size)
			{
				m_Size = Size;
				m_Buffer = reinterpret_cast<T*>(AllocateMemory(m_Allocator, m_Size * sizeof(T)));
			}

			void Deallocate(void)
			{
				if (m_Buffer == nullptr)
					return;

				DeallocateMemory(m_Allocator, m_Buffer);
				m_Size = 0;
			}

			void Move(Buffer<T> &Buffer)
			{
				m_Allocator = Buffer.m_Allocator;
				m_Buffer = Buffer.m_Buffer;
				m_Size = Buffer.m_Size;

				Buffer.m_Buffer = nullptr;
				Buffer.m_Size = 0;
			}

			void Copy(const T *const Buffer, const uint64 &Index, const uint64 &Size)
			{
				bool mustRealloc = (m_Size != Size);

				if (mustRealloc)
				{
					Deallocate();

					m_Size = Size;
				}

				if (m_Size == 0)
					return;

				if (mustRealloc)
					Allocate(m_Size);

				PlatformMemory::Copy(Buffer, Index, m_Buffer, 0, m_Size * sizeof(T));
			}

		private:
			AllocatorBase *m_Allocator;
			T *m_Buffer;
			uint64 m_Size;
		};

		template<typename T>
		INLINE Buffer<T> &operator << (Buffer<T> &Buffer, char8 Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		template<typename T>
		INLINE Buffer<T> &operator << (Buffer<T> &Buffer, char16 Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		template<typename T>
		INLINE Buffer<T> &operator << (Buffer<T> &Buffer, int8 Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		template<typename T>
		INLINE Buffer<T> &operator << (Buffer<T> &Buffer, int16 Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		template<typename T>
		INLINE Buffer<T> &operator << (Buffer<T> &Buffer, int32 Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		template<typename T>
		INLINE Buffer<T> &operator << (Buffer<T> &Buffer, int64 Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		template<typename T>
		INLINE Buffer<T> &operator << (Buffer<T> &Buffer, uint8 Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		template<typename T>
		INLINE Buffer<T> &operator << (Buffer<T> &Buffer, uint16 Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		template<typename T>
		INLINE Buffer<T> &operator << (Buffer<T> &Buffer, uint32 Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		template<typename T>
		INLINE Buffer<T> &operator << (Buffer<T> &Buffer, const uint64 &Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		template<typename T>
		INLINE Buffer<T> &operator << (Buffer<T> &Buffer, float32 Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		template<typename T>
		INLINE Buffer<T> &operator << (Buffer<T> &Buffer, const float64 &Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		template<typename T>
		INLINE Buffer<T> &operator << (Buffer<T> &Buffer, const float128 &Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		template<typename T>
		INLINE Buffer<T> &operator << (Buffer<T> &Buffer, cstr Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		template<typename T>
		INLINE Buffer<T> &operator << (Buffer<T> &Buffer, cwstr Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		typedef Buffer<byte> ByteBuffer;
	}
}

#endif