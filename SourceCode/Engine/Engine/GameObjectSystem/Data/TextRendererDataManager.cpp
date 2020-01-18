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
			void RenderText(DeviceInterface *Device, const Matrix4F &Model, const Matrix4F &Projection, const char16 *Text, uint32 TextLength, Font *Font, Material *Material, float32 Size, float32 Alignment)
			{
				static Matrix4F view;
				view.MakeIdentity();

				view.SetScale(Size, Size, 0);

				float32 maxYAdvance = 0.0F;
				float32 sumYAdvance = 0.0F;
				float32 sumXAdvance = 0.0F;
				for (uint32 j = 0; j < TextLength; ++j)
				{
					char16 charCode = Text[j];

					if (charCode == '\n' || charCode == '\r')
					{
						sumYAdvance -= maxYAdvance;
						sumXAdvance = 0;
						maxYAdvance = 0;

						continue;
					}

					Font::Character *ch = Font->GetCharacter(Text[j]);

					if (ch == nullptr)
					{
						ch = Font->GetCharacter('?');

						if (ch == nullptr)
							continue;
					}

					Vector2F bearing = ch->GetBearing() * Size * Alignment;
					Vector2F advance = ch->GetAdvance() * Size * Alignment;

					auto mesh = ch->GetMesh();
					if (mesh != nullptr)
					{
						view.SetPosition(sumXAdvance + bearing.X, sumYAdvance, 0);
						Matrix4F mvp = Projection * view * Model;

						Device->DrawMesh(ch->GetMesh(), Model, view, Projection, mvp, Material);
					}

					sumXAdvance += advance.X;

					if (maxYAdvance < ch->GetSize().Y)
						maxYAdvance = ch->GetSize().Y;
				}
			}

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

				auto &frameSize = device->GetWindow()->GetClientSize();

				Matrix4F projection;
				projection.MakeOrthographicProjectionMatrix(frameSize.X, frameSize.Y, 0.1, 1000);

				for (uint32 i = 0; i < size; ++i)
				{
					ColdData &coldData = data[i];

					Font *font = **coldData.Font;
					Material *material = coldData.Material;

					const char16 *currText = coldData.Text.GetValue();
					uint32 len = coldData.Text.GetLength();

					int8 alignment = coldData.Alignment;

					float32 size = coldData.Size;

					float32 outlineThickness = coldData.OutlineThickness;

					if (outlineThickness != 0.0F)
						RenderText(device, modelMat[i], projection, currText, len, font, material, size + outlineThickness, alignment);

					RenderText(device, modelMat[i], projection, currText, len, font, material, size, alignment);
				}
			}
		}
	}
}
