// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_POINTER_H
#define RESOURCE_POINTER_H

#include <Common\PrimitiveTypes.h>
#include <MemoryManagement\ReferenceCounted.h>

namespace Engine
{
	using namespace Common;

	namespace ResourceSystem
	{
		namespace Private
		{
			template<typename T>
			class ResourcePointer
			{
				REFERENCE_COUNTED_DEFINITION()

			public:
				ResourcePointer(T *Resource) :
					m_Resource(Resource),
					m_IsLocked(false)
				{
				}

				void Swap(T *Resource)
				{
					m_Resource = Resource;
				}

				T *GetData(void)
				{
					return m_Resource;
				}

				T *operator *(void)
				{
					return m_Resource;
				}

				T *operator ->(void)
				{
					return m_Resource;
				}

				void Lock(void)
				{
					m_IsLocked.exchange(true);
				}

				void Free(void)
				{
					m_IsLocked.exchange(false);
				}

				bool IsLocked(void) const
				{
					return m_IsLocked.load();
				}

			private:
				T * m_Resource;
				AtomicBool m_IsLocked;
			};

			typedef void* ResourceAnyPointer;
		}
	}
}

#endif