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

					return ***m_QuadMesh;
				}

				static Material* GetTitleBarMaterial(void)
				{
					Initialize();

					return &m_TitleBarMaterial;
				}

			private:
				static void Initialize(void);

			public:
				static MeshResource m_QuadMesh;
				static Material m_TitleBarMaterial;
			};
		}
	}
}

#endif