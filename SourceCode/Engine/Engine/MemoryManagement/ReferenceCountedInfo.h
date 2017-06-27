// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once

#ifndef REFERENCE_COUNTED_INFO_H
#define REFERENCE_COUNTED_INFO_H

#include <Common\PrimitiveTypes.h>
#include <Debugging\Debug.h>

namespace Engine
{
	using namespace Common;
	using namespace Debugging;
	
	namespace MemoryManagement
	{
#if DEBUG_MODE
		typedef int32 ReferenceCounterType;
#else
		typedef uint32 ReferenceCounterType;
#endif

		class MEMORYMANAGEMENT_API ReferenceCountedInfo
		{
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

		public:
			ReferenceCounterType GetCount(void)
			{
				return m_Count;
			}

		protected:
			ReferenceCounterType m_Count;
		};

#define REFERENCE_COUNTED_INFO(Name) \
		public: \
			Name(void) : \
				m_Count(1) \
			{ } \
			void Grab(void) \
			{ \
				++m_Count; \
			} \
			void Drop(void) \
			{ \
				--m_Count; \
				Assert(m_Count >= 0, "Count cannot be negative"); \
			} \
		public: \
			MemoryManagement::ReferenceCounterType GetCount(void) \
			{ \
				return m_Count; \
			} \
		protected: \
			MemoryManagement::ReferenceCounterType m_Count;
	}
}

#endif