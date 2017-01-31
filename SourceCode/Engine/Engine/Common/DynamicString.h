// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\StringUtils.h>
#include <Common\StringUtils.h>

#ifndef DYNAMIC_STRING_H
#define DYNAMIC_STRING_H

namespace Engine
{
	using namespace Allocator;

	namespace Common
	{
		template<typename T, AllocatorBase A> class DynamicString
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

		private:
			void SetValue(const T *Value)
			{
				uint32 length = StringUtils::GetLength(Value);

				if (length != m_Length)
					A->Deallocate((byte*)m_String);
			}

		private:
			T *m_String;
			uint32 m_Length;
		};
	}
}

#endif