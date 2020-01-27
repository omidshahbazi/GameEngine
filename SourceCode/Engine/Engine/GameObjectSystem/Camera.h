// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <GameObjectSystem\Component.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		using namespace Data;

		class GAMEOBJECTSYSTEM_API Camera : public Component
		{
		public:
			Camera(IDType SceneID, IDType ID, GameObjectTypes Type);

			void SetFieldOfView(float32 Value);
			void SetAspectRatio(float32 Value);
			void SetNearClipDistance(float32 Value);
			void SetFarClipDistance(float32 Value);
		};
	}
}

#endif