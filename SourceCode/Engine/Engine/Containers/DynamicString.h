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

			DynamicString(const T *Value)
			{
				SetValue(Value);
			}

			~DynamicString(void)
			{
				SetValue(nullptr);
			}

		private:
			void SetValue(const T *Value)
			{
				uint32 length = StringUtils::GetLength(Value);

				if (length != m_Length && m_String != nullptr)
					DefaultAllocator::GetInstance().Deallocate((byte*)m_String);

				if (length == 0)
				{
					m_String = nullptr;
					m_Length = 0;
					return;
				}

				m_Length = length;
				uint32 size = sizeof(T) * (m_Length + 1);

				m_String = (T*)DefaultAllocator::GetInstance().Allocate(size);

				PlatformMemory::Copy((byte*)Value, (byte*)m_String, size);
			}

		private:
			T *m_String;
			uint32 m_Length;
		};
	}
}

#endif