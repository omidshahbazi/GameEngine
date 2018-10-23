// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef BUFFER_H
#define BUFFER_H

#include <Common\BytesOf.h>
#include <Containers\Vector.h>
#include <MemoryManagement\Allocator\AllocatorBase.h>
#include <string>

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
			Buffer(void) :
				Buffer(nullptr, 0)
			{
			}

			Buffer(AllocatorBase *Allocator) :
				Buffer(Allocator, 0)
			{
			}

			Buffer(const uint64 &Capacity) :
				Buffer(nullptr, Capacity)
			{
			}

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
				m_Buffer(Buffer.m_Buffer)
			{
			}

			Buffer(Buffer<T> &&Buffer)
			{
				Move(Buffer);
			}

			~Buffer(void)
			{
			}

			INLINE void AppendBuffer(const Buffer<T> &Buffer)
			{
				m_Buffer.AddRange(Buffer.m_Buffer);
			}

			INLINE void AppendBuffer(const T *const Buffer, const uint64 &Index, const uint64 &Size)
			{
				m_Buffer.AddRange(Buffer, Index, Size);
			}

			template<typename V>
			INLINE void AppendValue(V Value)
			{
				std::basic_string<T> str = std::_Integral_to_string<T, V>(Value);
				AppendBuffer(str.c_str(), 0, str.length());
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
				AppendValue<int>(Value);
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

			template<typename V>
			INLINE V ReadValue(uint64 Index) const
			{
				BytesOf<V> value;

				for (uint32 i = 0; i < sizeof(V); ++i)
					value.Bytes[i] = m_Buffer[Index++];

				return value.Value;
			}

			INLINE const T * ReadValue(uint64 Index, uint64 Size) const
			{
				Assert(Index + Size <= m_Buffer.GetSize(), "Size exceeds");

				return &m_Buffer[Index];
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
		INLINE Buffer<T> &operator << (Buffer<T> &Buffer, void *Value)
		{
			Buffer.Append((int)Value);
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

		typedef Buffer<byte> ByteBuffer;
	}
}

#endif