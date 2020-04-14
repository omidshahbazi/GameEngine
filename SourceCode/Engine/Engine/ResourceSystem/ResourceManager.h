// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <MemoryManagement\Singleton.h>
#include <ResourceSystem\Private\ResourceHolder.h>

namespace Engine
{
	using namespace Containers;

	namespace ResourceSystem
	{
		using namespace Private;

		class RESOURCESYSTEM_API ResourceManager : public ResourceHolder
		{
			SINGLETON_DECLARATION(ResourceManager)

		private:
			ResourceManager(void);
			~ResourceManager(void);

		public:
			TextureResource GetWhiteTexture(void);
			ShaderResource GetDefaultShader(void);

		private:
			void CreateDefaultResources(void);
		};
	}
}

#endif