// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\RenderableWindow.h>
#include <EditorGUI\Private\Resources.h>
#include <Rendering\DeviceInterface.h>

namespace Engine
{
	using namespace ResourceSystem;

	namespace EditorGUI
	{
		//input handling

		using namespace Private;

		const float32 TITLE_BAR_HEIGHT = 25;
		const float32 HALF_TITLE_BAR_HEIGHT = TITLE_BAR_HEIGHT / 2.0F;

		RenderableWindow::RenderableWindow(void)
		{
			m_QuadMesh = Resources::GetQuadMesh();
			m_BackgroundMaterial = Resources::GetBackgroundMatrial();
			m_TitleBarMaterial = Resources::GetTitleBarMaterial();
		}

		void RenderableWindow::Render(EditorRenderDeviceBase* Device) const
		{
			auto& rect = GetRect();

			Device->SetPivot(Vector3F(rect.Position.X, rect.Position.Y, 0));

			Device->DrawMesh(m_QuadMesh, Vector3F::Zero, Vector3F::Zero, Vector3F(rect.Size.X, rect.Size.Y, 0), m_BackgroundMaterial);

			//Device->DrawMesh(m_QuadMesh, { 0, (rect.Size.Y / 2) - HALF_TITLE_BAR_HEIGHT, 0 }, Vector3F::Zero, { (float32)rect.Size.X, TITLE_BAR_HEIGHT, 0 }, m_TitleBarMaterial);
			Device->DrawMesh(m_QuadMesh, Vector3F::Zero, Vector3F::Zero, { (float32)rect.Size.X, TITLE_BAR_HEIGHT, 0 }, m_TitleBarMaterial);

			Vector3F buttonSize(300, 400, 0);

			Material* buttonMat = Resources::GetSpriteRendererMaterial();
			Pass& pass = buttonMat->GetPasses()[0];
			pass.SetVector2("elemDim", Vector2F(buttonSize.X, buttonSize.Y));

			SpriteHandle* sprite = Resources::GetButtonTexture();
			pass.SetSprite("difTex", sprite);
			auto& dimension = sprite->GetData()->GetDimension();
			pass.SetVector2("texDim", Vector2F(dimension.X, dimension.Y));
			auto& borders = sprite->GetData()->GetBorders();
			pass.SetVector4("texBorder", Vector4F(borders.X, borders.Y, borders.Z, borders.W));

			pass.SetFloat32("drawMode", 0);
			Device->DrawMesh(m_QuadMesh, { 50, 50, 0 }, Vector3F::Zero, { 64, 64, 0 }, buttonMat);

			//pass.SetFloat32("drawMode", 1);
			//Device->DrawMesh(m_QuadMesh, { 0, 0, 0 }, Vector3F::Zero, buttonSize, buttonMat);
		}
	}
}