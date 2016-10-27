// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Common\PrimitiveTypes.h>

//using namespace Engine::Debugging;

#ifndef REFERENCE_COUNTED_H
#define REFERENCE_COUNTED_H

//namespace Engine::MemoryManagement
//{
	template<typename Allocator> class ReferenceCounted
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

			//Allocator::Deallocate((byte*)const_cast<ReferenceCounted<Allocator>*>(this));
		}

	private:
		uint32 m_Count;
	};
//}

#endif