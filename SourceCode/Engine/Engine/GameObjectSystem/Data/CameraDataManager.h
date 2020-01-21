// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CAMERA_DATA_MANAGER_H
#define CAMERA_DATA_MANAGER_H

#include <GameObjectSystem\Data\ComponentDataManager.h>
#include <GameObjectSystem\GameObjectSystemCommon.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		namespace Data
		{
			class GAMEOBJECTSYSTEM_API CameraDataManager : public ComponentDataManager
			{
				friend class RendererDataManager;
				friend class LightDataManager;
				friend class TextRendererDataManager;

			private:
				struct ColdData
				{
				public:
					ProjectionTypes Type;
					float32 FieldOfView;
					float32 AspectRatio;
					float32 NearClipDistance;
					float32 FarClipDistance;
				};

			public:
				CameraDataManager(SceneData *SceneData);

				IDType Create(void) override;

				void SetFieldOfView(IDType ID, float32 Value);
				void SetAspectRatio(IDType ID, float32 Value);
				void SetNearClipDistance(IDType ID, float32 Value);
				void SetFarClipDistance(IDType ID, float32 Value);

				static void UpdateProjectionMatrix(Matrix4F &Matrix, const ColdData &ColdData);

				void Update(void) override;

				void Render(void) override
				{
				}

			private:
				Matrix4FList m_ProjectionMatrices;
				Matrix4FList m_ViewProjectionMatrices;
				DataContainer<ColdData> m_ColdData;

				DynamicSizeAllocator m_ProjectionMatricesAllocator;
				DynamicSizeAllocator m_ViewProjectionMatricesAllocator;
				DynamicSizeAllocator m_ColdDataAllocator;
			};
		}
	}
}

#endif