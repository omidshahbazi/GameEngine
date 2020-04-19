// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCES_H
#define RESOURCES_H

#include <Rendering\Material.h>
#include <ResourceSystem\Resource.h>

namespace Engine
{
	using namespace Rendering;
	using namespace ResourceSystem;

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

				static Material* GetTitleBarMaterial(void)
				{
					Initialize();

					return &m_TitleBarMaterial;
				}

				static Material* GetNineSliceMaterial(void)
				{
					Initialize();

					return &m_NineSliceMaterial;
				}

				static Material* GetSimpleMaterial(void)
				{
					Initialize();

					return &m_SimpleMaterial;
				}

				static SpriteHandle* GetButtonTexture(void)
				{
					Initialize();

					return m_ButtonTexture;
				}

			private:
				static void Initialize(void);

			public:
				static Mesh* m_QuadMesh;
				static Material m_TitleBarMaterial;
				static Material m_NineSliceMaterial;
				static Material m_SimpleMaterial;
				static SpriteHandle* m_ButtonTexture;
			};
		}
	}
}

#endif