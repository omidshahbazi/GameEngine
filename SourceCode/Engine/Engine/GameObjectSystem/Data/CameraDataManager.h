// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef CAMERA_DATA_MANAGER_H
#define CAMERA_DATA_MANAGER_H

#include <GameObjectSystem\Data\ComponentDataManager.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		namespace Data
		{
			class GAMEOBJECTSYSTEM_API CameraDataManager : public ComponentDataManager
			{
			public:
				CameraDataManager(void);

				IDType Create(void) override;

				void Update(void) override;

				void Render(void) override
				{
				}

			private:
				Matrix4FList m_ViewMatrices;

				DynamicSizeAllocator m_ViewMatricesAllocator;
			};
		}
	}
}

#endif