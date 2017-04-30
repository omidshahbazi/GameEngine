// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\StringUtils.h>
#include <MemoryManagement\Allocator\AllocatorBase.h>
#include <Platform\PlatformMemory.h>

#ifndef DYNAMIC_STRING_H
#define DYNAMIC_STRING_H

namespace Engine
{
	using namespace MemoryManagement::Allocator;
	using namespace Platform;

	namespace Containers
	{
		template<typename T> class DynamicString
		{
		public:
			typedef T CharType;

			DynamicString(AllocatorBase *Allocator) :
				m_Allocator(Allocator),
				m_String(nullptr),
				m_Length(0),
				m_Capacity(0)
			{
			}

			DynamicString(AllocatorBase *Allocator, const T Value) :
				m_Allocator(Allocator),
				m_String(nullptr),
				m_Length(0),
				m_Capacity(0)
			{
				SetValue(&Value, 1);
			}

			DynamicString(AllocatorBase *Allocator, const T *Value) :
				m_Allocator(Allocator),
				m_String(nullptr),
				m_Length(0),
				m_Capacity(0)
			{
				SetValue(Value);
			}

			DynamicString(const DynamicString<T> &Value) :
				m_Allocator(Value.m_Allocator),
				m_String(nullptr),
				m_Length(0),
				m_Capacity(0)
			{
				SetValue(Value.m_String);
			}

			DynamicString(AllocatorBase *Allocator, const DynamicString<T> &Value) :
				m_Allocator(Allocator),
				m_String(nullptr),
				m_Length(0),
				m_Capacity(0)
			{
				SetValue(Value.m_String);
			}

			DynamicString(DynamicString<T> &&Value) :
				m_Allocator(Value.m_Allocator),
				m_String(nullptr),
				m_Length(0),
				m_Capacity(0)
			{
				Move(Value);
			}

			DynamicString(AllocatorBase *Allocator, DynamicString<T> &&Value) :
				m_Allocator(Allocator),
				m_String(nullptr),
				m_Length(0),
				m_Capacity(0)
			{
				Move(Value);
			}

			~DynamicString(void)
			{
				SetValue(nullptr, 0);
			}

			DynamicString<T> & operator = (const T Value)
			{
				SetValue(Value, 1);

				return *this;
			}

			DynamicString<T> & operator = (const T *Value)
			{
				SetValue(Value);

				return *this;
			}

			DynamicString<T> & operator = (const DynamicString<T> &Value)
			{
				if (m_String != Value.m_String)
					SetValue(Value.m_String);

				return *this;
			}

			DynamicString<T> & operator = (DynamicString<T> &&Value)
			{
				if (m_String != Value.m_String)
					Move(Value);

				return *this;
			}

			DynamicString<T> & operator += (const T Value)
			{
				Append(&Value, 1);

				return *this;
			}

			DynamicString<T> & operator += (const T *Value)
			{
				Append(Value);

				return *this;
			}

			DynamicString<T> & operator += (const DynamicString<T> &Value)
			{
				Append(Value.m_String);

				return *this;
			}

			bool operator == (const T *Value) const
			{
				if (m_String == Value)
					return true;

				uint32 length = StringUtils::GetLength(Value);

				if (m_Length != length)
					return false;

				return StringUtils::AreEquals(m_String, Value);
			}

			bool operator == (const DynamicString<T> &Value) const
			{
				if (m_String == Value.m_String)
					return true;

				if (m_Length != Value.m_Length)
					return false;

				return StringUtils::AreEquals(m_String, Value.m_String);
			}

			bool operator != (const T *Value) const
			{
				return !(*this == Value);
			}

			bool operator != (const DynamicString<T> &Value) const
			{
				return !(*this == Value);
			}

			const T *GetValue(void) const
			{
				return m_String;
			}

			uint32 GetLength(void) const
			{
				return m_Length;
			}

		private:
			void SetValue(const T *Value)
			{
				SetValue(Value, StringUtils::GetLength(Value));
			}

			void SetValue(const T *Value, uint32 Length)
			{
				if (Length == 0)
				{
					Deallocate();
					m_String = nullptr;
					m_Length = 0;
					m_Capacity = 0;
					return;
				}
				else if (Length > m_Capacity)
				{
					Deallocate();
					m_Capacity = Length;

					m_String = Allocate(sizeof(T) * (Length + 1));
				}

				m_Length = Length;

				PlatformMemory::Copy((byte*)Value, (byte*)m_String, sizeof(T) * m_Length);
				m_String[m_Length] = StringUtils::Character<T, '\0'>::Value;
			}

			void Move(DynamicString<T> &Value)
			{
				if (m_Allocator != Value.m_Allocator)
				{
					SetValue(Value.m_String, Value.m_Length);

					return;
				}

				Deallocate();

				m_String = Value.m_String;
				m_Length = Value.m_Length;
				m_Capacity = Value.m_Capacity;

				Value.m_String = nullptr;
				Value.m_Length = 0;
			}

			void Append(const T *Value)
			{
				Append(Value, StringUtils::GetLength(Value));
			}

			void Append(const T *Value, uint32 Length)
			{
				if (Length == 0)
					return;

				uint32 newLength = m_Length + Length;
				uint32 newSize = sizeof(T) * (newLength + 1);

				bool allocateNewBuffer = (newLength > m_Capacity);

				T *newMemory = (allocateNewBuffer ? Allocate(newSize) : m_String);

				uint32 size = sizeof(T) * m_Length;

				if (allocateNewBuffer)
					PlatformMemory::Copy((byte*)m_String, 0, (byte*)newMemory, 0, size);

				PlatformMemory::Copy((byte*)Value, 0, (byte*)newMemory, size, sizeof(T) * (Length));
				newMemory[newLength] = StringUtils::Character<T, '\0'>::Value;

				if (allocateNewBuffer)
				{
					Deallocate();

					m_String = newMemory;
					m_Capacity = newLength;
				}

				m_Length = newLength;
			}

			void Deallocate(void)
			{
				if (m_String != nullptr)
					m_Allocator->Deallocate((byte*)m_String);
			}

			T *Allocate(uint32 Size)
			{
				return reinterpret_cast<T*>(AllocateMemory(m_Allocator, Size));
			}

			template<typename T> friend DynamicString<T> operator + (const T LeftValue, const DynamicString<T> &RightValue)
			{
				DynamicString<T> value(RightValue.m_Allocator, LeftValue);
				value += RightValue;
				return value;
			}

			template<typename T> friend DynamicString<T> operator + (const T *LeftValue, const DynamicString<T> &RightValue)
			{
				DynamicString<T> value(RightValue.m_Allocator, LeftValue);
				value += RightValue;
				return value;
			}

			template<typename T> friend DynamicString<T> operator + (const DynamicString<T> &LeftValue, const T RightValue)
			{
				DynamicString<T> value(LeftValue.m_Allocator, LeftValue);
				value += RightValue;
				return value;
			}

			template<typename T> friend DynamicString<T> operator + (const DynamicString<T> &LeftValue, const T *RightValue)
			{
				DynamicString<T> value(LeftValue.m_Allocator, LeftValue);
				value += RightValue;
				return value;
			}

			template<typename T> friend DynamicString<T> operator + (const DynamicString<T> &LeftValue, const DynamicString<T> &RightValue)
			{
				DynamicString<T> value(LeftValue.m_Allocator, LeftValue);
				value += RightValue;
				return value;
			}

		private:
			T *m_String;
			uint32 m_Length;
			uint32 m_Capacity;
			AllocatorBase *m_Allocator;
		};
	}
}

#endif