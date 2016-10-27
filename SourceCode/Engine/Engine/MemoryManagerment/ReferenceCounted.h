// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Common\PrimitiveTypes.h>

#ifndef REFERENCE_COUNTED_H
#define REFERENCE_COUNTED_H

namespace Engine
{
	using namespace Common;
	
	namespace MemoryManagement
	{
		class ReferenceCounted
		{
		public:
			ReferenceCounted(void) :
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
			}

			//implement destroy as callback

		protected:
			uint32 m_Count;
		};
	}
}

#endif