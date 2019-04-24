// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Data\TextRendererDataManager.h>
#include <GameObjectSystem\Private\GameObjectSystemAllocators.h>
#include <GameObjectSystem\Data\SceneData.h>
#include <Rendering\RenderingManager.h>

namespace Engine
{
	using namespace Rendering;
	using namespace MemoryManagement::Allocator;

	namespace GameObjectSystem
	{
		using namespace Private;

		namespace Data
		{
			TextRendererDataManager::TextRendererDataManager(SceneData *SceneData) :
				ComponentDataManager(SceneData),
				m_FontHandlesAllocator("Font Handles Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(FontHandle*) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT),
				m_TextsAllocator("Text Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(WString) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT),
				m_MaterialsAllocator("Materials Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(MaterialList::ItemType) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT)
			{
				m_Fonts = DataContainer<FontHandle*>(&m_FontHandlesAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
				m_Texts = DataContainer<WString>(&m_TextsAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
				m_Materials = MaterialList(&m_MaterialsAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
			}

			IDType TextRendererDataManager::Create(void)
			{
				auto id = ComponentDataManager::Create();

				auto &font = m_Fonts.Allocate();
				font = nullptr;

				m_Texts.Allocate();

				auto &material = m_Materials.Allocate();
				material = nullptr;

				return id;
			}

			void TextRendererDataManager::SetFont(IDType ID, FontHandle * Font)
			{
				int32 index = GetIndex(ID);

				m_Fonts[index] = Font;
			}

			void TextRendererDataManager::SetText(IDType ID, const WString &Text)
			{
				int32 index = GetIndex(ID);

				m_Texts[index] = Text;
			}

			void TextRendererDataManager::SetMaterial(IDType ID, Material * Material)
			{
				int32 index = GetIndex(ID);

				m_Materials[index] = Material;
			}

			void TextRendererDataManager::Render(void)
			{
				DeviceInterface *device = RenderingManager::GetInstance()->GetActiveDevice();

				uint32 size = m_IDs.GetSize();

				if (size == 0)
					return;

				SceneData *sceneData = GetSceneData();

				FontHandle **font = m_Fonts.GetData();
				WString *text = m_Texts.GetData();
				Material **material = m_Materials.GetData();
				Matrix4F *modelMat = sceneData->Renderables.Transforms.m_WorldMatrices.GetData();

				//int32 cameraIndex = 0;
				//const Matrix4F &view = sceneData->Cameras.Transforms.m_WorldMatrices[cameraIndex];
				//const Matrix4F &projection = sceneData->Cameras.Cameras.m_ProjectionMatrices[cameraIndex];
				//const Matrix4F &viewProjection = sceneData->Cameras.Cameras.m_ViewProjectionMatrices[cameraIndex];

				Matrix4F id;
				id.MakeIdentity();

				for (uint32 i = 0; i < size; ++i)
				{
					Matrix4F mvp = modelMat[i];

					Font *currFont = **font[i];
					const char16 *currText = text[i].GetValue();
					uint32 len = text[i].GetLength();
					
					for (uint32 j = 0; j < len; ++j)
					{
						Mesh *mesh = currFont->GetMesh(currText[j]);

						if (mesh == nullptr)
							continue;

						device->DrawMesh(mesh, modelMat[i], id, id, mvp, material[i]);
					}
				}
			}
		}
	}
}
