// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef LIGHT_DATA_MANAGER_H
#define LIGHT_DATA_MANAGER_H

#include <GameObjectSystem\Data\ComponentDataManager.h>
#include <GameObjectSystem\GameObjectSystemCommon.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		namespace Data
		{
			class GAMEOBJECTSYSTEM_API LighDataManager : public ComponentDataManager
			{
			private:
				struct ColdData
				{
				public:
					LightTypes Type;
					Color Color;
					float Strength;
					MeshHandle *Mesh;
					Material Material;
				};

			public:
				LighDataManager(SceneData *SceneData);

				IDType Create(void) override;

				void SetType(IDType ID, LightTypes Value);
				void SetColor(IDType ID, Color Value);
				void SetStrength(IDType ID, float Value);

				void Update(void) override;
				void Render(void) override;

			private:
				static void UpdateMesh(ColdData &ColdData);
				static void UpdateMaterial(ColdData &ColdData);

			private:
				DataContainer<ColdData> m_ColdData;

				DynamicSizeAllocator m_ColdDataAllocator;
			};
		}
	}
}

#endif