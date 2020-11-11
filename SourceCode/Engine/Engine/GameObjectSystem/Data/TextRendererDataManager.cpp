// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Data\TextRendererDataManager.h>
#include <GameObjectSystem\Private\GameObjectSystemAllocators.h>
#include <GameObjectSystem\Data\SceneData.h>
#include <Rendering\RenderingManager.h>
#include <FontSystem\StringRenderer.h>

namespace Engine
{
	using namespace Rendering;
	using namespace MemoryManagement::Allocator;

	namespace GameObjectSystem
	{
		using namespace Private;

		namespace Data
		{
			TextRendererDataManager::TextRendererDataManager(SceneData* SceneData) :
				ComponentDataManager(SceneData),
				m_DataAllocator("Font Handles Allocator", GameObjectSystemAllocators::GameObjectSystemAllocator)
			{
				m_Data = DataContainer<ColdData>(&m_DataAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
			}

			TextRendererDataManager::~TextRendererDataManager(void)
			{
			}

			IDType TextRendererDataManager::Create(void)
			{
				auto id = ComponentDataManager::Create();

				auto& data = m_Data.Allocate();

				data.Font = nullptr;
				data.Material = nullptr;
				data.Size = 1.0F;
				data.OutlineThickness = 0.0F;
				data.Alignment = 1;

				return id;
			}

			void TextRendererDataManager::SetFont(IDType ID, FontResource* Font)
			{
				int32 index = GetIndex(ID);

				m_Data[index].Font = Font;
			}

			void TextRendererDataManager::SetMaterial(IDType ID, Material* Material)
			{
				int32 index = GetIndex(ID);

				m_Data[index].Material = Material;
			}

			void TextRendererDataManager::SetText(IDType ID, const WString& Text)
			{
				int32 index = GetIndex(ID);

				m_Data[index].Text = Text;
			}

			void TextRendererDataManager::SetRightToLeft(IDType ID, bool RightToLeft)
			{
				int32 index = GetIndex(ID);

				m_Data[index].Alignment = (RightToLeft ? -1 : 1);
			}

			void TextRendererDataManager::SetSize(IDType ID, float32 Size)
			{
				int32 index = GetIndex(ID);

				m_Data[index].Size = Size;
			}

			void TextRendererDataManager::SetOutlineThicknes(IDType ID, float32 OutlineThickness)
			{
				int32 index = GetIndex(ID);

				m_Data[index].OutlineThickness = OutlineThickness;
			}

			void TextRendererDataManager::Render(void)
			{
				DeviceInterface* device = RenderingManager::GetInstance()->GetActiveDevice();

				uint32 size = m_IDs.GetSize();

				if (size == 0)
					return;

				SceneData* sceneData = GetSceneData();

				ColdData* data = m_Data.GetData();

				Matrix4F* modelMat = sceneData->TextRenderables.Transforms.m_WorldMatrices.GetData();

				//int32 cameraIndex = 0;
				//const Matrix4F &view = sceneData->Cameras.Transforms.m_WorldMatrices[cameraIndex];
				//const Matrix4F &projection = sceneData->Cameras.Cameras.m_ProjectionMatrices[cameraIndex];
				//const Matrix4F &viewProjection = sceneData->Cameras.Cameras.m_ViewProjectionMatrices[cameraIndex];

				auto& frameSize = device->GetWindow()->GetClientSize();

				Matrix4F projection;
				projection.SetOrthographicProjection(frameSize.X, frameSize.Y, 0.1, 1000);

				for (uint32 i = 0; i < size; ++i)
				{
					ColdData& coldData = data[i];

					Font* font = **coldData.Font;

					if (font == nullptr)
						continue;

					Material* material = coldData.Material;

					int8 alignment = coldData.Alignment;

					float32 size = coldData.Size;

					float32 outlineThickness = coldData.OutlineThickness;

					static StringRenderer::Info info;
					info.Font = font;
					info.Alignment = alignment;
					info.LineSpacing = 0;

					if (outlineThickness != 0.0F)
					{
						info.Size = size + outlineThickness;
						StringRenderer::Render(device, modelMat[i], projection, coldData.Text, material, &info);
					}

					info.Size = size;
					StringRenderer::Render(device, modelMat[i], projection, coldData.Text, material, &info);
				}
			}
		}
	}
}
