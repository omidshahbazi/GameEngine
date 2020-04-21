// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCES_H
#define RESOURCES_H

#include <Rendering\Material.h>
#include <ResourceSystem\Resource.h>

namespace Engine
{
	namespace ResourceSystem
	{
		namespace Private
		{
			class ResourceHolder;
		}
	}

	using namespace Rendering;
	using namespace ResourceSystem;
	using namespace ResourceSystem::Private;

	namespace EditorGUI
	{
		namespace Private
		{
			class EDITORGUI_API Resources
			{
			public:
				static Mesh* GetQuadMesh(void)
				{
					Initialize();

					return m_QuadMesh;
				}

				static Material* GetBackgroundMatrial(void)
				{
					Initialize();

					return &m_BackgroundMaterial;
				}

				static Material* GetTitleBarMaterial(void)
				{
					Initialize();

					return &m_TitleBarMaterial;
				}

				static Material* GetSpriteRendererMaterial(void)
				{
					Initialize();

					return &m_SpriteRendererMaterial;
				}

				static SpriteHandle* GetButtonTexture(void)
				{
					Initialize();

					return m_ButtonTexture;
				}

			private:
				static void Initialize(void);

			public:
				static ResourceHolder* m_ResourceHolder;
				static Mesh* m_QuadMesh;
				static Material m_BackgroundMaterial;
				static Material m_TitleBarMaterial;
				static Material m_SpriteRendererMaterial;
				static SpriteHandle* m_ButtonTexture;
			};
		}
	}
}

#endif