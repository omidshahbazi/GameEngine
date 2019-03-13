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
				friend class CameraDataManager;
				friend class RendererDataManager;

			public:
				TransformDataManager(SceneData *SceneData);

				IDType Create(void) override;

				void SetLocalPosition(IDType ID, const Vector3F & Value);

				void SetLocalRotation(IDType ID, const Vector3F & Value);

				void SetLocalScale(IDType ID, const Vector3F & Value);

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