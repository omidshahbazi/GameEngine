// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Camera.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		Camera::Camera(IDType SceneID, IDType RendererID) :
			m_SceneID(SceneID),
			m_ID(RendererID)
		{
		}
	}
}