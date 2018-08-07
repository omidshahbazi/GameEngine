// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_H
#define RESOURCE_H

namespace Engine
{
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
				Resource(nullptr)
			{
			}

			Resource(T *Resource) :
				m_Resource(Resource)
			{
			}

		private:
			T * m_Resource;
		};

		typedef Resource<Rendering::Texture> TextureResource;

	}
}

#endif