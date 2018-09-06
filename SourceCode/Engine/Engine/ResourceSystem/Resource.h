// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_H
#define RESOURCE_H

namespace Engine
{
	template<typename T>
	class ResourcePointer;

	namespace Rendering
	{
		class Texture;
	}

	namespace ResourceSystem
	{
		template<typename T>
		class Resource
		{
		public:
			Resource(void) :
				m_Resource(nullptr)
			{
			}

			Resource(ResourcePointer<T> *Resource) :
				m_Resource(Resource)
			{
			}

		private:
			ResourcePointer<T> *m_Resource;
		};

		typedef Resource<Rendering::Texture> TextureResource;
	}
}

#endif