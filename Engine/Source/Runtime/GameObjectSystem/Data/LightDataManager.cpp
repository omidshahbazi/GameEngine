// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Data\LightDataManager.h>
#include <GameObjectSystem\Private\GameObjectSystemAllocators.h>
#include <GameObjectSystem\Data\SceneData.h>
#include <ResourceManagement\ResourceManager.h>
#include <RenderSystem\RenderManager.h>
#include <RenderSystem\IPipeline.h>
#include <RenderCommon\GPUAlignedType.h>

namespace Engine
{
	using namespace Allocators;
	using namespace ResourceManagement;
	using namespace RenderCommon;

	namespace GameObjectSystem
	{
		using namespace Private;

		namespace Data
		{
			LightDataManager::LightDataManager(SceneData* SceneData) :
				ComponentDataManager(SceneData),
				m_ColdDataAllocator("Light ColdData Allocator", GameObjectSystemAllocators::GameObjectSystemAllocator, 128 * MegaByte),
				m_CommandBuffer("Light Pass")
			{
				m_ColdData = DataContainer<ColdData>(&m_ColdDataAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
			}

			IDType LightDataManager::Create(void)
			{
				auto id = ComponentDataManager::Create();

				auto& coldData = m_ColdData.Allocate();

				Construct(&coldData);

				coldData.Type = LightTypes::Ambient;
				coldData.Color = ColorUI8::White;
				coldData.Strength = 1;
				coldData.Radius = 1.0F;
				coldData.ConstantAttenuation = 1.0F;
				coldData.LinearAttenuation = 0.7F;
				coldData.QuadraticAttenuation = 1.8F;
				coldData.InnerCutOff = 12 * Math::DEGREES_TO_RADIANS;
				coldData.OuterCutOff = 17 * Math::DEGREES_TO_RADIANS;

				UpdateMesh(coldData);
				UpdateMaterial(coldData);

				return id;
			}

			void LightDataManager::SetType(IDType ID, LightTypes Value)
			{
				int32 index = GetIndex(ID);

				auto& coldData = m_ColdData[index];

				if (coldData.Type == Value)
					return;

				coldData.Type = Value;

				UpdateMesh(coldData);
				UpdateMaterial(coldData);
			}

			void LightDataManager::SetColor(IDType ID, const ColorUI8& Value)
			{
				int32 index = GetIndex(ID);

				auto& coldData = m_ColdData[index];

				coldData.Color = Value;
			}

			void LightDataManager::SetStrength(IDType ID, float32 Value)
			{
				int32 index = GetIndex(ID);

				auto& coldData = m_ColdData[index];

				coldData.Strength = Value;
			}

			void LightDataManager::SetRadius(IDType ID, float32 Value)
			{
				int32 index = GetIndex(ID);

				auto& coldData = m_ColdData[index];

				coldData.Radius = Value;
			}

			void LightDataManager::SetConstantAttenuation(IDType ID, float32 Value)
			{
				int32 index = GetIndex(ID);

				auto& coldData = m_ColdData[index];

				coldData.ConstantAttenuation = Value;
			}

			void LightDataManager::SetLinearAttenuation(IDType ID, float32 Value)
			{
				int32 index = GetIndex(ID);

				auto& coldData = m_ColdData[index];

				coldData.LinearAttenuation = Value;
			}

			void LightDataManager::SetQuadraticAttenuation(IDType ID, float32 Value)
			{
				int32 index = GetIndex(ID);

				auto& coldData = m_ColdData[index];

				coldData.QuadraticAttenuation = Value;
			}

			void LightDataManager::SetInnerCutOff(IDType ID, float32 Value)
			{
				int32 index = GetIndex(ID);

				auto& coldData = m_ColdData[index];

				coldData.InnerCutOff = Value;
			}

			void LightDataManager::SetOuterCutOff(IDType ID, float32 Value)
			{
				int32 index = GetIndex(ID);

				auto& coldData = m_ColdData[index];

				coldData.OuterCutOff = Value;
			}

			void LightDataManager::Update(void)
			{
				uint32 size = m_IDs.GetSize();

				if (size == 0)
					return;

				static const Pass::ConstantHash ConstantHash_data = Pass::GetHash("data");

				SceneData* sceneData = GetSceneData();

				ColdData* coldData = &m_ColdData[0];
				Matrix4F* worldMat = sceneData->Lightings.Transforms.m_WorldMatrices.GetData();

				int32 cameraIndex = 0;
				const Matrix4F& view = sceneData->Cameras.Transforms.m_WorldMatrices[cameraIndex];

				IPipeline* pipeline = RenderManager::GetInstance()->GetDevice()->GetPipeline();
				if (pipeline == nullptr)
					return;

				for (uint32 i = 0; i < size; ++i)
				{
					auto& data = coldData[i];

					struct Data
					{
					public:
						GPUAlignedVector3F WorldPosition;
						GPUAlignedVector3F ViewPosition;
						GPUAlignedColorF Color;
						GPUAlignedFloat32 Strength;
						GPUAlignedFloat32 Radius;
						GPUAlignedFloat32 ConstantAttenuation;
						GPUAlignedFloat32 LinearAttenuation;
						GPUAlignedFloat32 QuadraticAttenuation;
						GPUAlignedFloat32 InnerCutOff;
						GPUAlignedFloat32 OuterCutOff;
						GPUAlignedVector3F Direction;
					};

					Data structData;
					(ColorF&)structData.Color << data.Color;
					structData.Strength = data.Strength;
					structData.Radius = data.Radius;
					structData.ConstantAttenuation = data.ConstantAttenuation;
					structData.LinearAttenuation = data.LinearAttenuation;
					structData.QuadraticAttenuation = data.QuadraticAttenuation;
					structData.InnerCutOff = data.InnerCutOff;
					structData.OuterCutOff = data.OuterCutOff;
					structData.WorldPosition = worldMat[i].GetTranslate();
					structData.ViewPosition = view.GetTranslate();
					structData.Direction = worldMat[i].GetForward();

					data.Material.SetBuffer(ConstantHash_data, &structData);

					pipeline->SetPassConstants(&data.Material);
				}
			}

			void LightDataManager::Render(void)
			{
				uint32 size = m_IDs.GetSize();

				if (size == 0)
					return;

				SceneData* sceneData = GetSceneData();

				ColdData* coldData = &m_ColdData[0];
				Matrix4F* modelMat = sceneData->Lightings.Transforms.m_WorldMatrices.GetData();

				int32 cameraIndex = 0;
				const Matrix4F& viewProjection = sceneData->Cameras.Cameras.m_ViewProjectionMatrices[cameraIndex];

				m_CommandBuffer.Clear();

				for (uint32 i = 0; i < size; ++i)
				{
					ColdData& data = coldData[i];

					Matrix4F mvp = viewProjection * modelMat[i];

					m_CommandBuffer.DrawMesh(**data.Mesh, mvp, &data.Material);
				}

				RenderManager::GetInstance()->GetDevice()->SubmitCommandBuffer(&m_CommandBuffer);
			}

			void LightDataManager::UpdateMesh(ColdData& ColdData)
			{
				ResourceManager* resMgr = ResourceManager::GetInstance();

				switch (ColdData.Type)
				{
				case LightTypes::Ambient:
				case LightTypes::Directional:
					ColdData.Mesh = resMgr->GetPrimitiveMesh(ResourceManager::PrimitiveMeshTypes::Quad);
					break;

				case LightTypes::Point:
					ColdData.Mesh = resMgr->GetPrimitiveMesh(ResourceManager::PrimitiveMeshTypes::Cube);
					break;

				case LightTypes::Spot:
					ColdData.Mesh = resMgr->GetPrimitiveMesh(ResourceManager::PrimitiveMeshTypes::Cone);
					break;
				}
			}

			void LightDataManager::UpdateMaterial(ColdData& ColdData)
			{
				IPipeline* pipeline = RenderManager::GetInstance()->GetDevice()->GetPipeline();
				if (pipeline == nullptr)
					return;

				ProgramResource* program = nullptr;

				switch (ColdData.Type)
				{
				case LightTypes::Ambient:
					program = pipeline->GetAmbinetLightProgram();
					break;

				case LightTypes::Directional:
					program = pipeline->GetDirectionalLightProgram();
					break;

				case LightTypes::Point:
					program = pipeline->GetPointLightProgram();
					break;

				case LightTypes::Spot:
					program = pipeline->GetSpotLightProgram();
					break;
				}

				if (ColdData.Material.GetPasses().GetSize() == 0)
				{
					Pass p(program);
					p.SetQueue(RenderQueues::Lighting);
					auto state = p.GetRenderState();
					state.CullMode = CullModes::None;
					state.DepthTestFunction = TestFunctions::Never;
					state.BlendEquation = BlendEquations::Add;
					state.BlendFunctionDestinationFactor = BlendFunctions::One;
					state.BlendFunctionSourceFactor = BlendFunctions::One;
					p.SetRenderState(state);
					ColdData.Material.AddPass(p);
				}
				else
				{
					ColdData.Material.GetPasses()[0].SetProgram(program);
				}
			}
		}
	}
}
