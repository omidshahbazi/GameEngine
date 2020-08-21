// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_HANDLE_H
#define RESOURCE_HANDLE_H

#include <Common\PrimitiveTypes.h>
#include <MemoryManagement\ReferenceCounted.h>

namespace Engine
{
	using namespace Common;

	namespace ResourceSystem
	{
		class ResourceHandleBase
		{
		public:
			virtual ~ResourceHandleBase(void)
			{
			}
		};

		template<typename T>
		class ResourceHandle : public ResourceHandleBase
		{
			REFERENCE_COUNTED_DEFINITION()

		public:
			ResourceHandle(void) :
				m_Resource(nullptr),
				m_IsLocked(false)
			{
				REFERENCE_COUNTED_INITIALIZE()
			}

			ResourceHandle(T* Resource) :
				m_Resource(Resource),
				m_IsLocked(false)
			{
				REFERENCE_COUNTED_INITIALIZE();
			}

			ResourceHandle(const ResourceHandle<T>& Other)
			{
				REFERENCE_COUNTED_INITIALIZE();

				*this = Other;
			}

			virtual ~ResourceHandle(void)
			{
			}

			void Swap(T* Resource)
			{
				m_Resource = Resource;
			}

			T* GetData(void)
			{
				return m_Resource;
			}

			T* operator *(void)
			{
				return m_Resource;
			}

			T* operator ->(void)
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

			bool IsNull(void) const
			{
				return (m_Resource == nullptr);
			}

			ResourceHandle<T>& operator = (const ResourceHandle<T>& Other)
			{
				m_Resource = Other.m_Resource;
				m_IsLocked.store(Other.m_IsLocked.load());

				Grab();

				return *this;
			}

		private:
			T* m_Resource;
			AtomicBool m_IsLocked;
		};
	}
}

#endif