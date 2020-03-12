// Copyright 2016-2020 ?????????????. All Rights Reserved.
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
				friend class LightDataManager;
				friend class TextRendererDataManager;

			public:
				TransformDataManager(SceneData* SceneData);

				IDType Create(void) override;

				const Vector3F& GetForward(IDType ID);

				const Vector3F& GetLocalPosition(IDType ID);
				void SetLocalPosition(IDType ID, const Vector3F& Value);

				void SetLocalRotation(IDType ID, const QuaternionF& Value);

				void SetForward(IDType ID, const Vector3F& Value);

				void SetLocalScale(IDType ID, const Vector3F& Value);

				void Update(void) override;

				void Render(void) override
				{
				}

			private:
				Matrix4FList m_LocalMatrices;
				Matrix3FList m_RotationMatrices;
				Matrix4FList m_WorldMatrices;

				DynamicSizeAllocator m_LocalMatrixAllocator;
				DynamicSizeAllocator m_RotationMatrixAllocator;
				DynamicSizeAllocator m_WorldMatrixAllocator;
			};
		}
	}
}

#endif