// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\RenderableWindow.h>
#include <EditorGUI\Private\Resources.h>
#include <Rendering\DeviceInterface.h>

namespace Engine
{
	using namespace ResourceSystem;

	namespace EditorGUI
	{
		using namespace Private;

		//TODO: Use this to render 9-Slice
		//https://gamedev.stackexchange.com/questions/153848/how-do-i-set-up-9-slicing-in-opengl

		const float32 TITLE_BAR_HEIGHT = 25;
		const float32 HALF_TITLE_BAR_HEIGHT = TITLE_BAR_HEIGHT / 2.0F;

		RenderableWindow::RenderableWindow(void)
		{
			m_QuadMesh = Resources::GetQuadMesh();
			m_TitleBarMaterial = Resources::GetTitleBarMaterial();
		}

		void RenderableWindow::Render(EditorRenderDeviceBase* Device) const
		{
			auto& rect = GetRect();

			Device->DrawMesh(m_QuadMesh, { 0, (rect.Size.Y / 2) - HALF_TITLE_BAR_HEIGHT, 0 }, Vector3F::Zero, { (float32)rect.Size.X, TITLE_BAR_HEIGHT, 0 }, m_TitleBarMaterial);


			Vector3F buttonSize(300, 150, 0);

			Material* buttonMat = Resources::GetNineSliceMaterial();
			Pass& pass = buttonMat->GetPasses()[0];

			TextureHandle* tex = Resources::GetButtonTexture();

			pass.SetVector2("elemDim", Vector2F(buttonSize.X, buttonSize.Y));
			pass.SetVector2("texDim", Vector2F(tex->GetData()->GetDimension().X, tex->GetData()->GetDimension().Y));
			pass.SetVector2("texBorder", Vector2F(2, 2));

			pass.SetTexture("difTex", tex);

			Device->DrawMesh(m_QuadMesh, { 0, 0, 0 }, Vector3F::Zero, buttonSize, buttonMat);



			Material* buttonMat1 = Resources::GetSimpleMaterial();
			Pass& pass1 = buttonMat1->GetPasses()[0];
			pass1.SetTexture("difTex", tex);
			Device->DrawMesh(m_QuadMesh, { 300, 0, 0 }, Vector3F::Zero, { 32, 32, 0 }, buttonMat1);
		}
	}
}