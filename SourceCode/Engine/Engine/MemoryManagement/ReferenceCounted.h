// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef REFERENCE_COUNTED_H
#define REFERENCE_COUNTED_H

namespace Engine
{
	namespace MemoryManagement
	{
#define REFERENCE_COUNTED_DEFINITION() \
		public: \
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
			int32 GetReferenceCount(void) \
			{ \
				return m_Count; \
			} \
		protected: \
			int32 m_Count;
	}
}

#endif