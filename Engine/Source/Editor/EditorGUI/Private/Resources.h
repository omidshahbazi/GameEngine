// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCES_H
#define RESOURCES_H

#include <Rendering\Material.h>
#include <FontSystem\Font.h>
#include <Containers\Map.h>
#include <MemoryManagement\Singleton.h>

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
	using namespace MemoryManagement::Allocator;
	using namespace Containers;

	namespace EditorGUI
	{
		namespace Private
		{
			class EDITORGUI_API Resources
			{
				SINGLETON_DECLARATION(Resources);

			private:
				typedef Map<String, SpriteResource*> SpriteMap;

			private:
				Resources(void);
				virtual ~Resources(void);

			public:
				Mesh* GetQuadMesh(void)
				{
					return m_QuadMesh;
				}

				FontResource* GetFont(void)
				{
					return m_Font;
				}

				Material* GetSpriteRendererMaterial(void)
				{
					return &m_SpriteRendererMaterial;
				}

				Material* GetTextRendererMaterial(void)
				{
					return &m_TextRendererMaterial;
				}

				SpriteResource* GetSprite(const String& Name);

				ProgramResource* CreateProgram(const String& Source);

			private:
				ResourceHolder* m_ResourceHolder;
				Mesh* m_QuadMesh;
				FontResource* m_Font;
				Material m_SpriteRendererMaterial;
				Material m_TextRendererMaterial;
				SpriteMap m_Sprites;
			};
		}
	}
}

#endif