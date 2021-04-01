// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef DYNAMIC_STRING_H
#define DYNAMIC_STRING_H

#include <Common\CharacterUtility.h>
#include <Common\Mathematics.h>
#include <Platform\PlatformMemory.h>
#include <Containers\List.h>
#include <Containers\Private\ContainersAllocators.h>

namespace Engine
{
	using namespace Platform;

	namespace Containers
	{
		using namespace Private;

		template<typename T>
		class DynamicString
		{
		public:
			typedef T CharType;

#define GET_ALLOCATOR() (m_Allocator == nullptr ? (m_Allocator = ContainersAllocators::DynamicStringAllocator) : m_Allocator)
#define ALLOCATE(Count) ReinterpretCast(T*, AllocateMemory(GET_ALLOCATOR(), ((Count) + 1) * sizeof(T)));

		public:
			//DynamicString(void) :
			//	m_String(nullptr),
			//	m_Length(0),
			//	m_Capacity(0),
			//	m_Allocator(nullptr)
			//{
			//	ContainersAllocators::Create();

			//	m_Allocator = ContainersAllocators::DynamicStringAllocator;
			//}

			DynamicString(uint32 Capacity = 0) :
				m_String(nullptr),
				m_Length(0),
				m_Capacity(Capacity),
				m_Allocator(nullptr)
			{
				ContainersAllocators::Create();

				m_Allocator = ContainersAllocators::DynamicStringAllocator;

				if (m_Capacity != 0)
					m_String = ALLOCATE(m_Capacity + 1);
			}

			DynamicString(AllocatorBase* Allocator, uint32 Capacity = 0) :
				m_String(nullptr),
				m_Length(0),
				m_Capacity(Capacity),
				m_Allocator(Allocator)
			{
				ContainersAllocators::Create();

				if (m_Capacity != 0)
					m_String = ALLOCATE(m_Capacity + 1);
			}

			DynamicString(const T Value) :
				m_String(nullptr),
				m_Length(0),
				m_Capacity(0),
				m_Allocator(nullptr)
			{
				ContainersAllocators::Create();

				m_Allocator = ContainersAllocators::DynamicStringAllocator;

				SetValue(&Value, 1);
			}

			DynamicString(AllocatorBase* Allocator, const T Value) :
				m_String(nullptr),
				m_Length(0),
				m_Capacity(0),
				m_Allocator(Allocator)
			{
				ContainersAllocators::Create();

				SetValue(&Value, 1);
			}

			DynamicString(const T* Value) :
				m_String(nullptr),
				m_Length(0),
				m_Capacity(0),
				m_Allocator(nullptr)
			{
				ContainersAllocators::Create();

				m_Allocator = ContainersAllocators::DynamicStringAllocator;

				SetValue(Value);
			}

			DynamicString(AllocatorBase* Allocator, const T* Value) :
				m_String(nullptr),
				m_Length(0),
				m_Capacity(0),
				m_Allocator(Allocator)
			{
				ContainersAllocators::Create();

				SetValue(Value);
			}

			DynamicString(const T* Value, uint32 Length) :
				m_String(nullptr),
				m_Length(0),
				m_Capacity(0),
				m_Allocator(nullptr)
			{
				ContainersAllocators::Create();

				m_Allocator = ContainersAllocators::DynamicStringAllocator;

				SetValue(Value, Length);
			}

			DynamicString(AllocatorBase* Allocator, const T* Value, uint32 Length) :
				m_String(nullptr),
				m_Length(0),
				m_Capacity(0),
				m_Allocator(Allocator)
			{
				ContainersAllocators::Create();

				SetValue(Value, Length);
			}

			DynamicString(const DynamicString<T>& Value) :
				m_String(nullptr),
				m_Length(0),
				m_Capacity(0),
				m_Allocator(Value.m_Allocator)
			{
				SetValue(Value.m_String);
			}

			DynamicString(AllocatorBase* Allocator, const DynamicString<T>& Value) :
				m_String(nullptr),
				m_Length(0),
				m_Capacity(0),
				m_Allocator(Allocator)
			{
				SetValue(Value.m_String);
			}

			DynamicString(DynamicString<T>&& Value) :
				m_String(nullptr),
				m_Length(0),
				m_Capacity(0),
				m_Allocator(nullptr)
			{
				Move(Value);
			}

			~DynamicString(void)
			{
				SetValue(nullptr, 0);
			}

			INLINE DynamicString<T> Replace(const DynamicString<T>& OldValue, const DynamicString<T>& NewValue) const
			{
				T* result = ALLOCATE(m_Length + Mathematics::Max(0, ((int32)m_Length * ((int32)NewValue.m_Length - 1))) + 1);

				uint32 newIndex = 0;
				for (uint32 i = 0; i < m_Length; ++i)
				{
					if (Compare(i, OldValue))
					{
						if (NewValue.m_Length != 0)
							PlatformMemory::Copy(NewValue.m_String, 0, result, newIndex, NewValue.m_Length);

						i += OldValue.m_Length - 1;
						newIndex += NewValue.m_Length;

						continue;
					}

					result[newIndex++] = m_String[i];
				}

				DynamicString<T> value(m_Allocator, result, newIndex);

				DeallocateMemory(m_Allocator, result);

				return value;
			}

			INLINE DynamicString<T> Replace(int32 StartIndex, int32 Length, const DynamicString<T>& NewValue) const
			{
				uint32 size = (m_Length - Length) + NewValue.m_Length + 1;

				T* result = ALLOCATE(size);

				if (StartIndex != 0)
					PlatformMemory::Copy(m_String, 0, result, 0, StartIndex);

				if (NewValue.m_Length != 0)
					PlatformMemory::Copy(NewValue.m_String, 0, result, StartIndex, NewValue.m_Length);

				PlatformMemory::Copy(m_String, StartIndex + Length, result, StartIndex + NewValue.m_Length, m_Length - (StartIndex + Length));

				DynamicString<T> value(m_Allocator, result, size);

				DeallocateMemory(m_Allocator, result);

				return value;
			}

			INLINE DynamicString<T> Remove(int32 StartIndex, int32 Length) const
			{
				uint32 size = (m_Length - Length) + 1;

				T* result = ALLOCATE(size);

				if (StartIndex != 0)
					PlatformMemory::Copy(m_String, 0, result, 0, StartIndex);

				PlatformMemory::Copy(m_String, StartIndex + Length, result, StartIndex, m_Length - (StartIndex + Length));

				DynamicString<T> value(m_Allocator, result, size);

				DeallocateMemory(m_Allocator, result);

				return value;
			}

			INLINE DynamicString<T> SubString(uint32 StartIndex) const
			{
				return SubString(StartIndex, m_Length - StartIndex);
			}

			INLINE DynamicString<T> SubString(uint32 StartIndex, uint32 Length) const
			{
				Assert(StartIndex < m_Length, "StartIndex must be less than m_Length");
				Assert(Length != 0, "Length cannot be zero");

				DynamicString<T> newValue(m_Allocator, &m_String[StartIndex], Length);
				return newValue;
			}

			INLINE DynamicString<T> ToLower(void) const
			{
				DynamicString<T> newValue(*this);

				for (uint32 i = 0; i < newValue.m_Length; ++i)
					newValue.m_String[i] = CharacterUtility::ToLower(newValue.m_String[i]);

				return newValue;
			}

			INLINE DynamicString<T> ToUpper(void) const
			{
				DynamicString<T> newValue(*this);

				for (uint32 i = 0; i < newValue.m_Length; ++i)
					newValue.m_String[i] = CharacterUtility::ToUpper(newValue.m_String[i]);

				return newValue;
			}

			INLINE DynamicString<T> TrimLeft(void) const
			{
				uint32 i = 0;
				for (; i < m_Length; ++i)
					if (m_String[i] != CharacterUtility::Character<T, ' '>::Value)
						break;

				return SubString(i);
			}

			INLINE DynamicString<T> TrimRight(void) const
			{
				uint32 i = m_Length - 1;
				for (; i >= 0; --i)
					if (m_String[i] != CharacterUtility::Character<T, ' '>::Value)
						break;

				return SubString(0, i + 1);
			}

			INLINE DynamicString<T> TrimAll(void) const
			{
				return TrimLeft().TrimRight();
			}

			INLINE List<DynamicString<T>> Split(const DynamicString<T>& Splitter, bool IgnoreEmptyEntries = false) const
			{
				List<DynamicString<T>> result;

				int32 prevIndex = 0;
				int32 index = 0;
				while ((index = FirstIndexOf(Splitter, index)) != -1)
				{
					uint32 len = index - prevIndex;

					if (len == 0)
					{
						if (!IgnoreEmptyEntries)
							result.Add(DynamicString<T>());
					}
					else
						result.Add(SubString(prevIndex, len));

					prevIndex = index + Splitter.m_Length;
					++index;
				}

				if (prevIndex != m_Length)
					result.Add(SubString(prevIndex, m_Length - prevIndex));

				return result;
			}

			INLINE int32 FirstIndexOf(const DynamicString<T>& Value, uint32 StartIndex = 0) const
			{
				if (StartIndex >= m_Length)
					return -1;

				if (Value.m_Length == 0)
					return -1;

				for (uint32 i = StartIndex; i < m_Length; ++i)
					if (Compare(i, Value))
						return i;

				return -1;
			}

			INLINE int32 LastIndexOf(const DynamicString<T>& Value, uint32 StartIndex = 0) const
			{
				if (StartIndex >= m_Length)
					return -1;

				if (Value.m_Length == 0)
					return -1;

				for (int32 i = m_Length - 1; i > StartIndex; --i)
					if (Compare(i, Value))
						return i;

				return -1;
			}

			INLINE bool StartsWith(const DynamicString<T>& Value) const
			{
				return Compare(0, Value);
			}

			INLINE bool EndsWith(const DynamicString<T>& Value) const
			{
				if (m_Length < Value.m_Length)
					return false;

				return Compare(m_Length - Value.m_Length, Value);
			}

			INLINE bool Contains(const DynamicString<T>& Value) const
			{
				return (FirstIndexOf(Value, 0) != -1);
			}

			template<typename NewT>
			INLINE DynamicString<NewT> ChangeType(void) const
			{
				NewT* value = nullptr;

				if (m_String != nullptr)
				{
					value = ReinterpretCast(NewT*, AllocateMemory(GET_ALLOCATOR(), (m_Length + 1) * sizeof(NewT)));

					CharacterUtility::ChangeType(m_String, value, m_Length + 1);
				}

				DynamicString<NewT> result(m_Allocator, value, m_Length);

				if (value != nullptr)
					DeallocateMemory(m_Allocator, value);

				return result;
			}

			INLINE DynamicString<T>& operator = (const T Value)
			{
				SetValue(&Value, 1);

				return *this;
			}

			INLINE DynamicString<T>& operator = (const T* Value)
			{
				SetValue(Value);

				return *this;
			}

			INLINE DynamicString<T>& operator = (const DynamicString<T>& Value)
			{
				if (m_String != Value.m_String)
					SetValue(Value.m_String, Value.GetLength());

				return *this;
			}

			INLINE DynamicString<T>& operator = (DynamicString<T>&& Value)
			{
				if (m_String != Value.m_String)
					Move(Value);

				return *this;
			}

			INLINE DynamicString<T>& operator += (const T Value)
			{
				Append(&Value, 1);

				return *this;
			}

			INLINE DynamicString<T>& operator += (const T* Value)
			{
				Append(Value);

				return *this;
			}

			INLINE DynamicString<T>& operator += (const DynamicString<T>& Value)
			{
				Append(Value.m_String);

				return *this;
			}

			INLINE bool operator == (const T* Value) const
			{
				if (m_String == Value)
					return true;

				uint32 length = CharacterUtility::GetLength(Value);

				if (m_Length != length)
					return false;

				return CharacterUtility::AreEquals(m_String, Value);
			}

			INLINE bool operator == (const DynamicString<T>& Value) const
			{
				if (m_String == Value.m_String)
					return true;

				if (m_Length != Value.m_Length)
					return false;

				return CharacterUtility::AreEquals(m_String, Value.m_String);
			}

			INLINE bool operator != (const T* Value) const
			{
				return !(*this == Value);
			}

			INLINE bool operator != (const DynamicString<T>& Value) const
			{
				return !(*this == Value);
			}

			INLINE T& operator [] (uint32 Index)
			{
				Assert(Index < m_Length, "Index must be smaller than length of string");

				return m_String[Index];
			}

			INLINE const T& operator [] (uint32 Index) const
			{
				Assert(Index < m_Length, "Index must be smaller than length of string");

				return m_String[Index];
			}

			INLINE T* GetValue(void)
			{
				return m_String;
			}

			INLINE const T* GetValue(void) const
			{
				return m_String;
			}

			INLINE uint32 GetLength(void) const
			{
				return m_Length;
			}

		private:
			INLINE bool Compare(uint32 Index, const DynamicString<T>& Value) const
			{
				if (Index + Value.m_Length > m_Length)
					return false;

				for (uint32 j = 0; j < Value.m_Length; ++j)
					if (m_String[Index + j] != Value.m_String[j])
						return false;

				return true;
			}

			INLINE void SetValue(const T* Value)
			{
				SetValue(Value, CharacterUtility::GetLength(Value));
			}

			INLINE void SetValue(const T* Value, uint32 Length)
			{
				if (Length == 0)
				{
					if (m_String != nullptr)
						DeallocateMemory(m_Allocator, m_String);

					m_String = nullptr;
					m_Length = 0;
					m_Capacity = 0;

					return;
				}

				m_Capacity = Length;

				m_String = ReinterpretCast(T*, ReallocateMemory(GET_ALLOCATOR(), m_String, (m_Capacity + 1) * sizeof(T)));

				m_Length = Length;

				PlatformMemory::Copy((byte*)Value, (byte*)m_String, sizeof(T) * m_Length);
				m_String[m_Length] = CharacterUtility::Character<T, '\0'>::Value;
			}

			INLINE void Move(DynamicString<T>& Value)
			{
				if (m_String != nullptr)
					DeallocateMemory(m_Allocator, m_String);

				m_String = Value.m_String;
				m_Length = Value.m_Length;
				m_Capacity = Value.m_Capacity;
				m_Allocator = Value.m_Allocator;

				Value.m_String = nullptr;
				Value.m_Length = 0;
				Value.m_Capacity = 0;
			}

			INLINE void Append(const T* Value)
			{
				Append(Value, CharacterUtility::GetLength(Value));
			}

			INLINE void Append(const T* Value, uint32 Length)
			{
				if (Length == 0)
					return;

				uint32 newLength = m_Length + Length;
				uint32 newSize = sizeof(T) * (newLength + 1);

				m_String = ReinterpretCast(T*, ReallocateMemory(GET_ALLOCATOR(), m_String, newSize * sizeof(T)));

				PlatformMemory::Copy((byte*)Value, 0, (byte*)m_String, sizeof(T) * m_Length, sizeof(T) * Length);
				m_String[newLength] = CharacterUtility::Character<T, '\0'>::Value;

				m_Capacity = newLength;
				m_Length = newLength;
			}

			template<typename T>
			INLINE friend DynamicString<T> operator + (const T LeftValue, const DynamicString<T>& RightValue);

			template<typename T>
			INLINE friend DynamicString<T> operator + (const T* LeftValue, const DynamicString<T>& RightValue);

			template<typename T>
			INLINE friend DynamicString<T> operator + (const DynamicString<T>& LeftValue, const T RightValue);

			template<typename T>
			INLINE friend DynamicString<T> operator + (const DynamicString<T>& LeftValue, const T* RightValue);

			template<typename T>
			INLINE friend DynamicString<T> operator + (const DynamicString<T>& LeftValue, const DynamicString<T>& RightValue);

			template<typename T>
			INLINE friend bool operator < (const DynamicString<T>& LeftValue, const DynamicString<T>& RightValue);

		private:
			T* m_String;
			uint32 m_Length;
			uint32 m_Capacity;
			mutable AllocatorBase* m_Allocator;
		};

		template<typename T>
		DynamicString<T> operator + (const T LeftValue, const DynamicString<T>& RightValue)
		{
			DynamicString<T> value(LeftValue);
			value += RightValue;
			return value;
		}

		template<typename T>
		DynamicString<T> operator + (const T* LeftValue, const DynamicString<T>& RightValue)
		{
			DynamicString<T> value(LeftValue);
			value += RightValue;
			return value;
		}

		template<typename T>
		DynamicString<T> operator + (const DynamicString<T>& LeftValue, const T RightValue)
		{
			DynamicString<T> value(LeftValue);
			value += RightValue;
			return value;
		}

		template<typename T>
		DynamicString<T> operator + (const DynamicString<T>& LeftValue, const T* RightValue)
		{
			DynamicString<T> value(LeftValue);
			value += RightValue;
			return value;
		}

		template<typename T>
		DynamicString<T> operator + (const DynamicString<T>& LeftValue, const DynamicString<T>& RightValue)
		{
			DynamicString<T> value(LeftValue);
			value += RightValue;
			return value;
		}

		template<typename T>
		bool operator < (const DynamicString<T>& LeftValue, const DynamicString<T>& RightValue)
		{
			return LeftValue.m_String < RightValue.m_String;
		}
	}

#undef GET_ALLOCATOR
#undef ALLOCATE
}

#endif