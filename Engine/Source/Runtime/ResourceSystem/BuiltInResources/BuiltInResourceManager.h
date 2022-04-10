// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef BUILT_IN_RESOURCE_MANAGER_H
#define BUILT_IN_RESOURCE_MANAGER_H

#include <MemoryManagement\Singleton.h>
#include <ResourceSystem\ResourceHolder.h>
#include <ResourceCommon\Resource.h>

namespace Engine
{
	using namespace Containers;
	using namespace ResourceSystem;

	namespace BuiltInResources
	{
		class BUILTINRESOURCES_API BuiltInResourceManager : public ResourceHolder
		{
			SINGLETON_DECLARATION(BuiltInResourceManager)

		private:
			BuiltInResourceManager(void);

		public:
			TextureResource* GetWhiteTexture(void);
			SpriteResource* GetWhiteSprite(void);

			ProgramResource* GetDefaultShader(void);
			ProgramResource* GetSpriteRendererShader(void);

			MeshResource* GetQuad(void);
			MeshResource* GetCube(void);
			MeshResource* GetSphere(void);
			MeshResource* GetCone(void);

		private:
			void CreateDefaultResources(void);
		};
	}
}

#endif