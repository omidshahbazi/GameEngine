// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <GameObjectSystem\Data\DataContainer.h>
#include <GameObjectSystem\Transform.h>
#include <GameObjectSystem\Camera.h>
#include <GameObjectSystem\Renderer.h>
#include <GameObjectSystem\Light.h>
#include <GameObjectSystem\TextRenderer.h>
#include <GameObjectSystem\GameObjectSystemCommon.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		using namespace Data;

		class GAMEOBJECTSYSTEM_API GameObject
		{
		public:
			GameObject(void);

			GameObject(IDType SceneID, IDType ID, GameObjectTypes Type);

			Transform GetTransform(void);
			Camera GetCamera(void);
			Renderer GetRenderer(void);
			Light GetLight(void);
			TextRenderer GetTextRenderer(void);

		private:
			IDType m_SceneID;
			IDType m_ID;
			GameObjectTypes m_Type;
		};
	}
}

#endif