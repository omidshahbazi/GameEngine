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
		public:
			ResourceHandle(void) :
				m_Resource(nullptr),
				m_IsReady(false)
			{
			}

			ResourceHandle(T* Resource) :
				m_Resource(Resource),
				m_IsReady(false)
			{
			}

			ResourceHandle(const ResourceHandle<T>& Other)
			{
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
				bool expected = false;
				m_IsReady.compare_exchange_weak(expected, true);
			}

			void Free(void)
			{
				bool expected = true;
				m_IsReady.compare_exchange_weak(expected, false);
			}

			bool IsReady(void) const
			{
				return m_IsReady.load();
			}

			bool IsNull(void) const
			{
				return (m_Resource == nullptr);
			}

			ResourceHandle<T>& operator = (const ResourceHandle<T>& Other)
			{
				m_Resource = Other.m_Resource;
				m_IsReady.store(Other.m_IsReady.load());

				return *this;
			}

		private:
			T* m_Resource;
			AtomicBool m_IsReady;
		};
	}
}

#endif