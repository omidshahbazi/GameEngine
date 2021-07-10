// Copyright 2016-2020 ?????????????. All Rights Reserved.
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
			class GAMEOBJECTSYSTEM_API LightDataManager : public ComponentDataManager
			{
			private:
				struct ColdData
				{
				public:
					LightTypes Type;
					ColorUI8 Color;
					float32 Strength;
					float32 Radius;
					float32 ConstantAttenuation;
					float32 LinearAttenuation;
					float32 QuadraticAttenuation;
					float32 InnerCutOff;
					float32 OuterCutOff;
					MeshResource* Mesh;
					Material Material;
				};

			public:
				LightDataManager(SceneData* SceneData);

				IDType Create(void) override;

				void SetType(IDType ID, LightTypes Value);
				void SetColor(IDType ID, const ColorUI8& Value);
				void SetStrength(IDType ID, float32 Value);
				void SetRadius(IDType ID, float32 Value);
				void SetConstantAttenuation(IDType ID, float32 Value);
				void SetLinearAttenuation(IDType ID, float32 Value);
				void SetQuadraticAttenuation(IDType ID, float32 Value);
				void SetInnerCutOff(IDType ID, float32 Value);
				void SetOuterCutOff(IDType ID, float32 Value);

				void Update(void) override;
				void Render(void) override;

			private:
				static void UpdateMesh(ColdData& ColdData);
				static void UpdateMaterial(ColdData& ColdData);

			private:
				DataContainer<ColdData> m_ColdData;

				DynamicSizeAllocator m_ColdDataAllocator;
			};
		}
	}
}

#endif