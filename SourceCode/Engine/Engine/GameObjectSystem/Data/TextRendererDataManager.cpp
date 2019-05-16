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
				m_DataAllocator("Font Handles Allocator", &GameObjectSystemAllocators::GameObjectSystemAllocator, sizeof(ColdData) * GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT)
			{
				m_Data = DataContainer<ColdData>(&m_DataAllocator, GameObjectSystemAllocators::MAX_GAME_OBJECT_COUNT);
			}

			IDType TextRendererDataManager::Create(void)
			{
				auto id = ComponentDataManager::Create();

				auto &data = m_Data.Allocate();

				data.Font = nullptr;
				data.Material = nullptr;
				data.Size = 1.0F;
				data.OutlineThickness = 0.0F;
				data.Alignment = 1;

				return id;
			}

			void TextRendererDataManager::SetFont(IDType ID, FontHandle * Font)
			{
				int32 index = GetIndex(ID);

				m_Data[index].Font = Font;
			}

			void TextRendererDataManager::SetMaterial(IDType ID, Material * Material)
			{
				int32 index = GetIndex(ID);

				m_Data[index].Material = Material;
			}

			void TextRendererDataManager::SetText(IDType ID, const WString &Text)
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

			void RenderText(DeviceInterface *Device, const Matrix4F &Model, const Matrix4F &Projection, const char16 *Text, uint32 TextLength, Font *Font, Material *Material, float32 Size, float32 Alignment)
			{
				static Matrix4F view;
				view.MakeIdentity();

				view.SetScale(Size, Size, 0);

				float32 sumAdvance = 0.0F;
				for (uint32 j = 0; j < TextLength; ++j)
				{
					Font::Character *ch = Font->GetMesh(Text[j]);

					if (ch == nullptr)
						continue;

					Vector2F bearing = ch->GetBearing() * Size * Alignment;
					Vector2F advance = ch->GetAdvance() * Size * Alignment;

					auto mesh = ch->GetMesh();
					if (mesh != nullptr)
					{
						view.SetPosition(sumAdvance + bearing.X, 0, 0);
						Matrix4F mvp = Projection * view * Model;

						Device->DrawMesh(ch->GetMesh(), Model, view, Projection, mvp, Material);
					}

					sumAdvance += advance.X;
				}
			}

			void TextRendererDataManager::Render(void)
			{
				DeviceInterface *device = RenderingManager::GetInstance()->GetActiveDevice();

				uint32 size = m_IDs.GetSize();

				if (size == 0)
					return;

				SceneData *sceneData = GetSceneData();

				ColdData *data = m_Data.GetData();

				Matrix4F *modelMat = sceneData->TextRenderables.Transforms.m_WorldMatrices.GetData();

				//int32 cameraIndex = 0;
				//const Matrix4F &view = sceneData->Cameras.Transforms.m_WorldMatrices[cameraIndex];
				//const Matrix4F &projection = sceneData->Cameras.Cameras.m_ProjectionMatrices[cameraIndex];
				//const Matrix4F &viewProjection = sceneData->Cameras.Cameras.m_ViewProjectionMatrices[cameraIndex];

				Matrix4F projection;
				projection.MakeOrthographicProjectionMatrix(1024, 768, 0.1, 1000);

				for (uint32 i = 0; i < size; ++i)
				{
					ColdData &coldData = data[i];

					Font *font = **coldData.Font;
					Material *material = coldData.Material;

					const char16 *currText = coldData.Text.GetValue();
					uint32 len = coldData.Text.GetLength();

					int8 alignment = coldData.Alignment;

					float32 size = coldData.Size;

					float32 outlineThicknes = coldData.OutlineThickness;

					if (outlineThicknes != 0.0F)
						RenderText(device, modelMat[i], projection, currText, len, font, material, size + outlineThicknes, alignment);

					RenderText(device, modelMat[i], projection, currText, len, font, material, size, alignment);
				}
			}
		}
	}
}
