// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_POINTER_H
#define RESOURCE_POINTER_H

namespace Engine
{
	namespace ResourceSystem
	{
		template<typename T>
		class ResourcePointer
		{
		public:
			ResourcePointer(T *Resource) :
				m_Resource(Resource)
			{
			}

			void Swap(T *Resource)
			{
				m_Resource = Resource;
			}

			T *operator *(void)
			{
				return m_Resource;
			}

			T *operator ->(void)
			{
				return m_Resource;
			}

		private:
			T *m_Resource;
		};

		typedef void* ResourceAnyPointer;
	}
}

#endif