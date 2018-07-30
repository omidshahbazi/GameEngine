// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef BUFFER_H
#define BUFFER_H

#include <Common\BytesOf.h>
#include <Containers\Vector.h>
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
				m_Buffer(Allocator, Capacity)
			{
				m_Buffer.Resize(Capacity);
			}

			Buffer(AllocatorBase *Allocator, const T *const Buffer, const uint64 &Size) :
				Buffer(Allocator, Buffer, 0, Size)
			{
			}

			Buffer(AllocatorBase *Allocator, const T *const Buffer, const uint64 &Index, const uint64 &Size) :
				m_Buffer(Allocator, Size)
			{
				m_Buffer.Resize(Capacity);

				Append(Buffer, Index, Size);
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
			}

			INLINE void AppendBuffer(const T *const Buffer, const uint64 &Index, const uint64 &Size)
			{
				m_Buffer.AddRange(Buffer, Index, Size);
			}

			template<typename V>
			INLINE void AppendValue(V Value)
			{
				BytesOf<V> value;
				value.Value = Value;

				Append(reinterpret_cast<T*>(value.Bytes), 0, sizeof(V));
			}

			INLINE void Append(char8 Value)
			{
				AppendValue(Value);
			}

			INLINE void Append(char16 Value)
			{
				AppendValue(Value);
			}

			INLINE void Append(int8 Value)
			{
				AppendValue(Value);
			}

			INLINE void Append(int16 Value)
			{
				AppendValue(Value);
			}

			INLINE void Append(int32 Value)
			{
				AppendValue(Value);
			}

			INLINE void Append(int64 Value)
			{
				AppendValue(Value);
			}

			INLINE void Append(uint8 Value)
			{
				AppendValue(Value);
			}

			INLINE void Append(uint16 Value)
			{
				AppendValue(Value);
			}

			INLINE void Append(uint32 Value)
			{
				AppendValue(Value);
			}

			INLINE void Append(const uint64 &Value)
			{
				AppendValue(Value);
			}

			INLINE void Append(float32 Value)
			{
				AppendValue(Value);
			}

			INLINE void Append(const float64 &Value)
			{
				AppendValue(Value);
			}

			INLINE void Append(const float128 &Value)
			{
				AppendValue(Value);
			}

			INLINE void Append(cstr Value)
			{
			}

			INLINE void Append(cwstr Value)
			{
			}

			template<typename V>
			INLINE V ReadValue(uint64 Index)
			{
				BytesOf<V> value;

				for (uint32 i = 0; i < sizeof(V); ++i)
					value.Bytes[i] = m_Buffer[Index++];

				return value.Value;
			}

			INLINE Buffer &operator = (const Buffer<T> &Buffer)
			{
				Append(Buffer.m_Buffer, 0, Buffer.m_Size);
				return *this;
			}

			INLINE Buffer &operator = (Buffer<T> &&Buffer)
			{
				Move(Buffer);
				return *this;
			}

			INLINE T &operator [] (const uint64 &Index)
			{
				return m_Buffer[Index];
			}

			INLINE T operator [] (const uint64 &Index) const
			{
				return m_Buffer[Index];
			}

			INLINE uint64 GetSize(void)
			{
				return m_Buffer.GetSize();
			}

			INLINE const uint64 &GetSize(void) const
			{
				return m_Buffer.GetSize();
			}

			INLINE T *GetBuffer(void)
			{
				return m_Buffer.GetData();
			}

			INLINE const T *const GetBuffer(void) const
			{
				return m_Buffer.GetData();
			}

		private:
			void Move(Buffer<T> &Buffer)
			{
				m_Buffer = PlatformMemory::Move(Buffer.m_Buffer);
			}

		private:
			Vector<T> m_Buffer;
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