// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <MemoryManagement\Singleton.h>
#include <ResourceSystem\ResourceHolder.h>
#include <ResourceCommon\Resource.h>

namespace Engine
{
	using namespace Containers;
	using namespace ResourceSystem;

	namespace ResourceManagement
	{
		class RESOURCEMANAGEMENT_API ResourceManager : public ResourceHolder
		{
		public:
			enum class PrimitiveMeshTypes
			{
				Quad,
				Cube,
				Sphere,
				Cone
			};

			SINGLETON_DECLARATION(ResourceManager)

		private:
			ResourceManager(void);

		public:
			~ResourceManager(void);

			TextureResource* GetWhiteTexture(void);
			SpriteResource* GetWhiteSprite(void);

			ProgramResource* GetDefaultShader(void);
			ProgramResource* GetSpriteRendererShader(void);

			MeshResource* GetPrimitiveMesh(PrimitiveMeshTypes Type);
		};
	}
}

#endif