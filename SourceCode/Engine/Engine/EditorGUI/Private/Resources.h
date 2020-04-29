// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCES_H
#define RESOURCES_H

#include <Rendering\Material.h>
#include <FontSystem\Font.h>
#include <ResourceSystem\Resource.h>
#include <Containers\Map.h>

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
	using namespace FontSystem;
	using namespace ResourceSystem;
	using namespace ResourceSystem::Private;
	using namespace Containers;

	namespace EditorGUI
	{
		namespace Private
		{
			class EDITORGUI_API Resources
			{
			private:
				typedef Map<String, SpriteHandle*> SpriteMap;

			public:
				static Mesh* GetQuadMesh(void)
				{
					Initialize();

					return m_QuadMesh;
				}

				static Font* GetFont(void)
				{
					Initialize();

					return m_Font;
				}

				static Material* GetSpriteRendererMaterial(void)
				{
					Initialize();

					return &m_SpriteRendererMaterial;
				}

				static Material* GetTextRendererMaterial(void)
				{
					Initialize();

					return &m_TextRendererMaterial;
				}

				static SpriteHandle* GetGetSprite(const String& Name);

			private:
				static void Initialize(void);

			public:
				static ResourceHolder* m_ResourceHolder;
				static Mesh* m_QuadMesh;
				static Font* m_Font;
				static Material m_SpriteRendererMaterial;
				static Material m_TextRendererMaterial;
				static SpriteMap m_Sprites;
			};
		}
	}
}

#endif