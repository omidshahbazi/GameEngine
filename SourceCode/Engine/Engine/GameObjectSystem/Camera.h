// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <GameObjectSystem\Data\DataContainer.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		using namespace Data;

		class GAMEOBJECTSYSTEM_API Camera
		{
		public:
			Camera(IDType SceneID, IDType CameraID);

			void SetFieldOfView(float32 Value);
			void SetAspectRatio(float32 Value);
			void SetNearClipDistance(float32 Value);
			void SetFarClipDistance(float32 Value);

		private:
			IDType m_SceneID;
			IDType m_ID;
		};
	}
}

#endif