// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\PrimitiveTypes.h>
#include <Debugging\Debug.h>

#ifndef REFERENCE_COUNTED_INFO_H
#define REFERENCE_COUNTED_INFO_H

namespace Engine
{
	using namespace Common;
	using namespace Debugging;
	
	namespace MemoryManagement
	{
		class ReferenceCountedInfo
		{
#if _DEBUG
			typedef int32 Count;
#else
			typedef uint32 Count;
#endif

		public:
			ReferenceCountedInfo(void) :
				m_Count(1)
			{
			}

			virtual void Grab(void)
			{
				++m_Count;
			}

			virtual void Drop(void)
			{
				--m_Count;

				Assert(m_Count >= 0, "Count cannot be negative");
			}

		protected:
			Count GetCount(void)
			{
				return m_Count;
			}

		protected:
			Count m_Count;
		};
	}
}

#endif