// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef TRANSFORM_DATA_MANAGER_H
#define TRANSFORM_DATA_MANAGER_H

#include <GameObjectSystem\Data\ComponentDataManager.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		namespace Data
		{
			class GAMEOBJECTSYSTEM_API TransformDataManager : public ComponentDataManager
			{
			public:
				TransformDataManager(SceneData *SceneData);

				IDType Create(void) override;

				Matrix4F &GetWorldMatrixByGameObjectID(IDType ID);

				void Update(void) override;

				void Render(void) override
				{
				}

			private:
				Matrix4FList m_LocalMatrices;
				Matrix4FList m_WorldMatrices;

				DynamicSizeAllocator m_LocalMatrixAllocator;
				DynamicSizeAllocator m_WorldMatrixAllocator;
			};
		}
	}
}

#endif