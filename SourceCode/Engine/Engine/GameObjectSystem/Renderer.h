// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include <GameObjectSystem\Data\DataContainer.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		using namespace Data;

		class GAMEOBJECTSYSTEM_API Renderer
		{
		public:
			Renderer(IDType SceneID, IDType RendererID);

			void SetMesh(MeshHandle *Mesh);

			void SetMaterial(Material *Material);

		private:
			IDType m_SceneID;
			IDType m_ID;
		};
	}
}

#endif