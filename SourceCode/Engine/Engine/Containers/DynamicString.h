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

			DynamicString(void) :
				m_String(nullptr),
				m_Length(0)
			{
			}

			DynamicString(const T Value) :
				m_String(nullptr),
				m_Length(0)
			{
				SetValue(&Value, 1);
			}

			DynamicString(const T *Value) :
				m_String(nullptr),
				m_Length(0)
			{
				SetValue(Value);
			}

			DynamicString(const DynamicString<T> &Value) :
				m_String(nullptr),
				m_Length(0)
			{
				SetValue(Value.m_String);
			}

			DynamicString(DynamicString<T> &&Value) :
				m_String(nullptr),
				m_Length(0)
			{
				Move(Value);
			}

			~DynamicString(void)
			{
				SetValue(nullptr);
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

				return (strcmp(m_String, Value) == 0);
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

		private:
			void SetValue(const T *Value)
			{
				SetValue(Value, StringUtils::GetLength(Value));
			}

			void SetValue(const T *Value, uint32 Length)
			{
				if (Length != m_Length)
					Deallocate();

				if (Length == 0)
				{
					m_String = nullptr;
					m_Length = 0;
					return;
				}

				m_Length = Length;

				m_String = Allocate(sizeof(T) * (m_Length + 1));

				PlatformMemory::Copy((byte*)Value, (byte*)m_String, sizeof(T) * m_Length);
				m_String[m_Length] = StringUtils::Character<T, '\0'>::Value;
			}

			void Move(DynamicString<T> &Value)
			{
				Deallocate();

				m_String = PlatformMemory::Move(Value.m_String);
				m_Length = Value.m_Length;

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

				T *newMemory = Allocate(newSize);

				uint32 size = sizeof(T) * m_Length;

				PlatformMemory::Copy((byte*)m_String, 0, (byte*)newMemory, 0, size);
				PlatformMemory::Copy((byte*)Value, 0, (byte*)newMemory, size, sizeof(T) * (Length));
				newMemory[newLength] = StringUtils::Character<T, '\0'>::Value;

				Deallocate();

				m_String = newMemory;
				m_Length = newLength;
			}

			void Deallocate(void)
			{
				if (m_String != nullptr)
					DefaultAllocator::GetInstance().Deallocate((byte*)m_String);
			}

			T *Allocate(uint32 Size)
			{
				return (T*)DefaultAllocator::GetInstance().Allocate(Size);
			}

		private:
			T *m_String;
			uint32 m_Length;
		};

		template<typename T> DynamicString<T> operator + (const T LeftValue, const DynamicString<T> &RightValue)
		{
			DynamicString<T> value(LeftValue);
			value += RightValue;
			return value;
		}

		template<typename T> DynamicString<T> operator + (const T *LeftValue, const DynamicString<T> &RightValue)
		{
			DynamicString<T> value(LeftValue);
			value += RightValue;
			return value;
		}

		template<typename T> DynamicString<T> operator + (const DynamicString<T> &LeftValue, const T RightValue)
		{
			DynamicString<T> value(LeftValue);
			value += RightValue;
			return value;
		}

		template<typename T> DynamicString<T> operator + (const DynamicString<T> &LeftValue, const T *RightValue)
		{
			DynamicString<T> value(LeftValue);
			value += RightValue;
			return value;
		}

		template<typename T> DynamicString<T> operator + (const DynamicString<T> &LeftValue, const DynamicString<T> &RightValue)
		{
			DynamicString<T> value(LeftValue);
			value += RightValue;
			return value;
		}
	}
}

#endif