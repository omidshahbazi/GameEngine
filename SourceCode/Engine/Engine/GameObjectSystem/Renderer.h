// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include <GameObjectSystem\Component.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		using namespace Data;

		class GAMEOBJECTSYSTEM_API Renderer : public Component
		{
		public:
			Renderer(IDType SceneID, IDType ID, GameObjectTypes Type);

			void SetMesh(MeshHandle *Mesh);

			void SetMaterial(Material *Material);
		};
	}
}

#endif