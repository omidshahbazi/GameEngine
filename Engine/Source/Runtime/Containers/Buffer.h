// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef BUFFER_H
#define BUFFER_H

#include <Common\BytesOf.h>
#include <Containers\Vector.h>
#include <Allocators\AllocatorBase.h>
#include <string>

namespace Engine
{
	using namespace Common;
	using namespace Allocators;

	namespace Containers
	{
		template<typename T, bool IsBinary>
		class Buffer
		{
		public:
			typedef T ElementType;

		public:
			Buffer(void) :
				Buffer((AllocatorBase*)nullptr, 0)
			{
			}

			Buffer(AllocatorBase* Allocator) :
				Buffer(Allocator, 0)
			{
			}

			Buffer(const uint64& Capacity) :
				Buffer((AllocatorBase*)nullptr, Capacity)
			{
			}

			Buffer(AllocatorBase* Allocator, const uint64& Capacity) :
				m_Buffer(Allocator, Capacity),
				m_ReadIndex(0)
			{
				m_Buffer.Recap(Capacity);
			}

			Buffer(const Buffer<T, IsBinary>& Buffer) :
				m_Buffer(Buffer.m_Buffer),
				m_ReadIndex(0)
			{
			}

			Buffer(T* Buffer, const uint64& Size) :
				m_Buffer(Buffer, Size),
				m_ReadIndex(0)
			{
			}

			Buffer(T* Buffer, const uint64& Index, const uint64& Size) :
				m_Buffer(Buffer, Index, Size),
				m_ReadIndex(0)
			{
			}

			Buffer(Buffer<T, IsBinary>&& Buffer) :
				m_ReadIndex(0)
			{
				Move(Buffer);
			}

			~Buffer(void)
			{
			}

			INLINE void AppendBuffer(const Buffer<T, IsBinary>& Buffer)
			{
				m_Buffer.AddRange(Buffer.m_Buffer);
			}

			INLINE void AppendBuffer(const T* const Buffer, const uint64& Index, const uint64& Size)
			{
				m_Buffer.AddRange(Buffer, Index, Size);
			}

			template<typename V>
			INLINE void AppendIntegralValue(V Value, uint8 NumberOfDigits)
			{
				if (IsBinary)
				{
					BytesOf<V> value;

					value.Value = Value;

					m_Buffer.AddRange(ReinterpretCast(T*, value.Bytes), sizeof(V));

					return;
				}

				T data[20];

				if (Value < 0)
				{
					m_Buffer.Add('-');
					Value *= -1;
				}

				uint8 i = 0;
				while (Value)
				{
					data[i++] = ((int64)Value % 10) + '0';
					Value = Value / 10;
				}

				while (i < NumberOfDigits)
					data[i++] = '0';

				if (i > 0)
				{
					Reverse(data, 0, i);

					AppendBuffer(data, 0, i);
				}
			}

			template<typename V>
			INLINE void AppendFloatValue(V Value, uint8 AfterPointCount)
			{
				if (IsBinary)
				{
					BytesOf<V> value;

					value.Value = Value;

					m_Buffer.AddRange(ReinterpretCast(T*, value.Bytes), sizeof(V));

					return;
				}

				int64 iPart = (V)Value;
				V fPart = abs(Value) - abs(iPart);

				AppendIntegralValue(iPart, 1);

				if (AfterPointCount != 0)
				{
					m_Buffer.Add('.');

					fPart = fPart * pow(10, AfterPointCount);

					AppendIntegralValue((int64)fPart, AfterPointCount);
				}
			}

			INLINE void Append(int8 Value)
			{
				AppendIntegralValue(Value, 1);
			}

			INLINE void Append(int16 Value)
			{
				AppendIntegralValue(Value, 1);
			}

			INLINE void Append(int32 Value)
			{
				AppendIntegralValue(Value, 1);
			}

			INLINE void Append(int64 Value)
			{
				AppendIntegralValue(Value, 1);
			}

			INLINE void Append(uint8 Value)
			{
				AppendIntegralValue(Value, 1);
			}

			INLINE void Append(uint16 Value)
			{
				AppendIntegralValue(Value, 1);
			}

			INLINE void Append(uint32 Value)
			{
				AppendIntegralValue(Value, 1);
			}

			INLINE void Append(const uint64& Value)
			{
				AppendIntegralValue(Value, 1);
			}

			INLINE void Append(float32 Value)
			{
				AppendFloatValue(Value, 4);
			}

			INLINE void Append(const float64& Value)
			{
				AppendFloatValue(Value, 4);
			}

			INLINE void Append(const float128& Value)
			{
				AppendFloatValue(Value, 4);
			}

			INLINE void Reverse(uint64 Index, uint64 Count)
			{
				Reverse(m_Buffer.GetData(), Index, Count);
			}

			INLINE void Seek(uint64 Index) const
			{
				HardAssert(Index < m_Buffer.GetSize(), "Size exceeds");

				m_ReadIndex = Index;
			}

			template<typename V>
			INLINE V ReadValue(void) const
			{
				BytesOf<V> value;

				for (uint32 i = 0; i < sizeof(V); ++i)
					value.Bytes[i] = m_Buffer[m_ReadIndex++];

				return value.Value;
			}

			INLINE const T* ReadValue(uint64 Size) const
			{
				if (Size == 0)
					return nullptr;

				HardAssert(m_ReadIndex + Size <= m_Buffer.GetSize(), "Size exceeds");

				const T* value = &m_Buffer[m_ReadIndex];

				m_ReadIndex += Size;

				return value;
			}

			INLINE Buffer& operator = (const Buffer<T, IsBinary>& Buffer)
			{
				AppendBuffer(Buffer.m_Buffer.GetData(), 0, Buffer.m_Buffer.GetSize());
				return *this;
			}

			INLINE Buffer& operator = (Buffer<T, IsBinary>&& Buffer)
			{
				Move(Buffer);
				return *this;
			}

			INLINE T& operator [] (const uint64& Index)
			{
				return m_Buffer[Index];
			}

			INLINE T operator [] (const uint64& Index) const
			{
				return m_Buffer[Index];
			}

			INLINE uint64 GetSize(void)
			{
				return m_Buffer.GetSize();
			}

			INLINE const uint64& GetSize(void) const
			{
				return m_Buffer.GetSize();
			}

			INLINE T* GetBuffer(void)
			{
				return m_Buffer.GetData();
			}

			INLINE const T* const GetBuffer(void) const
			{
				return m_Buffer.GetData();
			}

			INLINE static void Reverse(T* Data, uint64 Index, uint64 Count)
			{
				uint64 i = 0;
				uint64 j = Count - 1;
				T temp;
				while (i < j)
				{
					temp = Data[i];
					Data[i] = Data[j];
					Data[j] = temp;
					i++;
					j--;
				}
			}

			INLINE void Recap(uint32 Count)
			{
				m_Buffer.Recap(Count);
			}

			INLINE void Extend(uint32 Count)
			{
				m_Buffer.Extend(Count);
			}

			INLINE AllocatorBase*& GetAllocator(void)
			{
				return m_Buffer.GetAllocator();
			}

		private:
			void Move(Buffer<T, IsBinary>& Buffer)
			{
				m_Buffer = PlatformMemory::Move(Buffer.m_Buffer);
			}

		private:
			Vector<T> m_Buffer;
			mutable uint64 m_ReadIndex;
		};

		template<typename T, bool IsBinary>
		INLINE Buffer<T, IsBinary>& operator << (Buffer<T, IsBinary>& Buffer, void* Value)
		{
			Buffer.Append((int)Value);
			return Buffer;
		}

		template<typename T, bool IsBinary>
		INLINE Buffer<T, IsBinary>& operator << (Buffer<T, IsBinary>& Buffer, int8 Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		template<typename T, bool IsBinary>
		INLINE Buffer<T, IsBinary>& operator << (Buffer<T, IsBinary>& Buffer, int16 Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		template<typename T, bool IsBinary>
		INLINE Buffer<T, IsBinary>& operator << (Buffer<T, IsBinary>& Buffer, int32 Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		template<typename T, bool IsBinary>
		INLINE Buffer<T, IsBinary>& operator << (Buffer<T, IsBinary>& Buffer, int64 Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		template<typename T, bool IsBinary>
		INLINE Buffer<T, IsBinary>& operator << (Buffer<T, IsBinary>& Buffer, uint8 Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		template<typename T, bool IsBinary>
		INLINE Buffer<T, IsBinary>& operator << (Buffer<T, IsBinary>& Buffer, uint16 Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		template<typename T, bool IsBinary>
		INLINE Buffer<T, IsBinary>& operator << (Buffer<T, IsBinary>& Buffer, uint32 Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		template<typename T, bool IsBinary>
		INLINE Buffer<T, IsBinary>& operator << (Buffer<T, IsBinary>& Buffer, const uint64& Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		template<typename T, bool IsBinary>
		INLINE Buffer<T, IsBinary>& operator << (Buffer<T, IsBinary>& Buffer, float32 Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		template<typename T, bool IsBinary>
		INLINE Buffer<T, IsBinary>& operator << (Buffer<T, IsBinary>& Buffer, const float64& Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		template<typename T, bool IsBinary>
		INLINE Buffer<T, IsBinary>& operator << (Buffer<T, IsBinary>& Buffer, const float128& Value)
		{
			Buffer.Append(Value);
			return Buffer;
		}

		typedef Buffer<byte, true> ByteBuffer;
	}
}

#endif