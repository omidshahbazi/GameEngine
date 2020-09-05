// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Data\LightDataManager.h>
#include <GameObjectSystem\Private\GameObjectSystemAllocators.h>
#include <GameObjectSystem\Data\SceneData.h>
#include <ResourceSystem\ResourceManager.h>
#include <Rendering\Private\Pipeline\PipelineManager.h>
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
			LightDataManager::LightDataManager(SceneData* SceneData) :
				ComponentDataManager(SceneData),
				m_ColdDataAllocator("Light ColdData Allocator", GameObjectSystemAllocators::GameObjectSystemAllocator)
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
				coldData.InnerCutOff = 12 * Mathematics::DEGREES_TO_RADIANS;
				coldData.OuterCutOff = 17 * Mathematics::DEGREES_TO_RADIANS;

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

				SceneData* sceneData = GetSceneData();

				ColdData* coldData = &m_ColdData[0];
				Matrix4F* worldMat = sceneData->Lightings.Transforms.m_WorldMatrices.GetData();

				int32 cameraIndex = 0;
				const Matrix4F& view = sceneData->Cameras.Transforms.m_WorldMatrices[cameraIndex];

				IPipeline* pipeline = PipelineManager::GetInstance()->GetSelectedPipeline();

				for (uint32 i = 0; i < size; ++i)
				{
					auto& passes = coldData[i].Material.GetPasses();
					auto& pass = passes[0];

					auto& data = coldData[i];

					pass.SetColor("color", data.Color);
					pass.SetFloat32("strength", data.Strength);
					pass.SetFloat32("radius", data.Radius);
					pass.SetFloat32("constantAttenuation", data.ConstantAttenuation);
					pass.SetFloat32("linearAttenuation", data.LinearAttenuation);
					pass.SetFloat32("quadraticAttenuation", data.QuadraticAttenuation);
					pass.SetFloat32("innerCutOff", data.InnerCutOff);
					pass.SetFloat32("outerCutOff", data.OuterCutOff);
					pass.SetVector3("worldPos", worldMat[i].GetTranslate());
					pass.SetVector3("viewPos", view.GetTranslate());
					pass.SetVector3("direction", worldMat[i].GetForward());

					pipeline->SetPassConstants(&pass);
				}
			}

			void LightDataManager::Render(void)
			{
				DeviceInterface* device = RenderingManager::GetInstance()->GetActiveDevice();

				uint32 size = m_IDs.GetSize();

				if (size == 0)
					return;

				SceneData* sceneData = GetSceneData();

				ColdData* coldData = &m_ColdData[0];
				Matrix4F* modelMat = sceneData->Lightings.Transforms.m_WorldMatrices.GetData();

				int32 cameraIndex = 0;
				const Matrix4F& viewProjection = sceneData->Cameras.Cameras.m_ViewProjectionMatrices[cameraIndex];

				for (uint32 i = 0; i < size; ++i)
				{
					ColdData& data = coldData[i];

					Matrix4F mvp = viewProjection * modelMat[i];

					device->DrawMesh(**data.Mesh, mvp, &data.Material);
				}
			}

			void LightDataManager::UpdateMesh(ColdData& ColdData)
			{
				ResourceManager* resMgr = ResourceManager::GetInstance();

				switch (ColdData.Type)
				{
				case LightTypes::Ambient:
				case LightTypes::Directional:
					ColdData.Mesh = resMgr->GetPrimitiveMesh(ResourceManager::PrimitiveMeshTypes::Quad).GetData();
					break;

				case LightTypes::Point:
					ColdData.Mesh = resMgr->GetPrimitiveMesh(ResourceManager::PrimitiveMeshTypes::Cube).GetData();
					break;

				case LightTypes::Spot:
					ColdData.Mesh = resMgr->GetPrimitiveMesh(ResourceManager::PrimitiveMeshTypes::Cone).GetData();
					break;
				}
			}

			void LightDataManager::UpdateMaterial(ColdData& ColdData)
			{
				IPipeline* pipeline = PipelineManager::GetInstance()->GetSelectedPipeline();

				ShaderHandle* program = nullptr;

				switch (ColdData.Type)
				{
				case LightTypes::Ambient:
					program = pipeline->GetAmbinetLightShader();
					break;

				case LightTypes::Directional:
					program = pipeline->GetDirectionalLightShader();
					break;

				case LightTypes::Point:
					program = pipeline->GetPointLightShader();
					break;

				case LightTypes::Spot:
					program = pipeline->GetSpotLightShader();
					break;
				}

				Pass* pass = nullptr;

				if (ColdData.Material.GetPasses().GetSize() == 0)
				{
					Pass p(program);
					p.SetQueue(RenderQueues::Lighting);
					auto state = p.GetRenderState();
					state.CullMode = IDevice::CullModes::None;
					state.DepthTestFunction = IDevice::TestFunctions::Never;
					state.BlendEquation = IDevice::BlendEquations::Add;
					state.BlendFunctionDestinationFactor = IDevice::BlendFunctions::One;
					state.BlendFunctionSourceFactor = IDevice::BlendFunctions::One;
					p.SetRenderState(state);
					ColdData.Material.AddPass(p);

					pass = &ColdData.Material.GetPasses()[0];
				}
				else
				{
					pass = &ColdData.Material.GetPasses()[0];
					pass->SetShader(program);
				}
			}
		}
	}
}
