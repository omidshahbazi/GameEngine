// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Data\LighDataManager.h>
#include <GameObjectSystem\Private\GameObjectSystemAllocators.h>
#include <GameObjectSystem\Data\SceneData.h>
#include <ResourceSystem\ResourceManager.h>
#include <Rendering\Private\Pipeline\DeferredRendering.h>
#include <Rendering\RenderingManager.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;
	using namespace ResourceSystem;
	using namespace Rendering::Private::Pipeline;

	namespace GameObjectSystem
	{
		using namespace Private;

		namespace Data
		{
			LighDataManager::LighDataManager(SceneData *SceneData) :
				ComponentDataManager(SceneData),
				m_ColdDataAllocator("Light ColdData Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(ColdData) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT)
			{
				m_ColdData = DataContainer<ColdData>(&m_ColdDataAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
			}

			IDType LighDataManager::Create(void)
			{
				auto id = ComponentDataManager::Create();

				auto &coldData = m_ColdData.Allocate();

				Construct(&coldData);

				coldData.Type = LightTypes::Ambient;
				coldData.Color = Color(255, 255, 255);
				coldData.Strength = 1;
				coldData.Material.SetQueue(RenderQueues::Lighting);

				UpdateMesh(coldData);
				UpdateMaterial(coldData);

				return id;
			}

			void LighDataManager::SetType(IDType ID, LightTypes Value)
			{
				int32 index = GetIndex(ID);

				auto &coldData = m_ColdData[index];

				if (coldData.Type == Value)
					return;

				coldData.Type = Value;

				UpdateMesh(coldData);
				UpdateMaterial(coldData);
			}

			void LighDataManager::SetColor(IDType ID, Color Value)
			{
				int32 index = GetIndex(ID);

				auto &coldData = m_ColdData[index];

				coldData.Color = Value;
			}

			void LighDataManager::SetStrength(IDType ID, float Value)
			{
				int32 index = GetIndex(ID);

				auto &coldData = m_ColdData[index];

				coldData.Strength = Value;
			}

			void LighDataManager::Update(void)
			{
				uint32 size = m_IDs.GetSize();

				if (size == 0)
					return;

				SceneData *sceneData = GetSceneData();

				ColdData *coldData = &m_ColdData[0];
				Matrix4F *worldMat = sceneData->Lightings.Transforms.m_WorldMatrices.GetData();

				for (uint32 i = 0; i < size; ++i)
				{
					auto &passes = coldData[i].Material.GetPasses();
					auto &pass = passes[0];

					auto &data = coldData[i];

					pass.SetColor("color", data.Color);
					pass.SetFloat32("strength", data.Strength);
					pass.SetVector3("direction", worldMat[i].GetForward());
				}
			}

			void LighDataManager::Render(void)
			{
				DeviceInterface *device = RenderingManager::GetInstance()->GetActiveDevice();

				uint32 size = m_IDs.GetSize();

				if (size == 0)
					return;

				ColdData *coldData = &m_ColdData[0];

				Matrix4F mat;
				mat.MakeIdentity();

				for (uint32 i = 0; i < size; ++i)
				{
					ColdData &data = coldData[i];

					device->DrawMesh(**data.Mesh, mat, &data.Material);
				}
			}

			void LighDataManager::UpdateMesh(ColdData & ColdData)
			{
				ResourceManager *resMgr = ResourceManager::GetInstance();

				switch (ColdData.Type)
				{
				case LightTypes::Ambient:
				case LightTypes::Directional:
					ColdData.Mesh = resMgr->Load(PrimitiveMeshTypes::Quad).GetData();
					break;
				}
			}

			void LighDataManager::UpdateMaterial(ColdData & ColdData)
			{
				DeferredRendering *def = DeferredRendering::GetInstance();

				ProgramHandle *program;

				switch (ColdData.Type)
				{
				case LightTypes::Ambient:
					program = def->GetAmbinetLightProgram();
					break;

				case LightTypes::Directional:
					program = def->GetDirectionalLightProgram();
					break;
				}

				Pass *pass = nullptr;

				if (ColdData.Material.GetPasses().GetSize() == 0)
				{
					Pass p(program);
					auto state = p.GetRenderState();
					state.DepthTestFunction = IDevice::TestFunctions::Never;
					state.BlendFunctionDestinationFactor = IDevice::BlendFunctions::One;
					state.BlendFunctionSourceFactor = IDevice::BlendFunctions::One;
					p.SetRenderState(state);
					ColdData.Material.AddPass(p);

					pass = &ColdData.Material.GetPasses()[0];
				}
				else
				{
					pass = &ColdData.Material.GetPasses()[0];
					pass->SetProgram(program);
				}

				pass->SetTexture("PosTex", def->GetPositionTexture());
				pass->SetTexture("NormTex", def->GetNormalTexture());
				pass->SetTexture("AlbedoSpec", def->GetAlbedoSpecularTexture());
			}
		}
	}
}
