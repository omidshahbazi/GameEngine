// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_H
#define RESOURCE_H

#include <ResourceSystem\Private\ResourceHandle.h>

namespace Engine
{
	namespace Rendering
	{
		class Texture;
		class Program;
		class Mesh;
	}

	namespace ResourceSystem
	{
		class Text;

		template<typename T>
		class Resource
		{
		public:
			Resource(void) :
				m_Resource(nullptr)
			{
			}

			Resource(Private::ResourceHandle<T> *Resource) :
				m_Resource(Resource)
			{
				m_Resource->Grab();
			}

			~Resource(void)
			{
				m_Resource->Drop();
			}

			T *GetData(void)
			{
				return m_Resource->GetData();
			}

			T *operator *(void)
			{
				if (m_Resource == nullptr)
					return nullptr;

				return **m_Resource;
			}

			T *operator ->(void)
			{
				return **m_Resource;
			}

		private:
			Private::ResourceHandle<T> *m_Resource;
		};

		typedef Resource<Rendering::Texture> TextureResource;
		typedef Resource<Rendering::Program> ProgramResource;
		typedef Resource<Rendering::Mesh> MeshResource;
		typedef Resource<Text> TextResource;
	}
}

#endif