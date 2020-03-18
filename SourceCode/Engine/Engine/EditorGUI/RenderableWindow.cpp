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

		RenderableWindow::RenderableWindow(void)
		{
			AddListener(this);

			m_QuadMesh = Resources::GetQuadMesh();
			m_TitleBarMaterial = Resources::GetTitleBarMaterial();
		}

		float angle = 0;
		void RenderableWindow::Render(EditorRenderDeviceBase* Device) const
		{
			angle++;

			auto& rect = GetRect();

			Device->DrawMesh(m_QuadMesh, { rect.Size.X / -3.0F, TITLE_BAR_HEIGHT / -2.0F, 0 }, Vector3F(0, 0, angle) * Mathematics::DEGREES_TO_RADIANS, { (float32)rect.Size.X, TITLE_BAR_HEIGHT, 0 }, m_TitleBarMaterial);

			//RectI Rect(800, 600);

			//Matrix4F ModelMat = Matrix4F::Identity;
			//ModelMat.SetTranslate();

			//Matrix4F rotMat = Matrix4F::Identity;
			//QuaternionF rot = QuaternionF::FromEuler(Vector3F(0, 0, angle) * Mathematics::DEGREES_TO_RADIANS);
			//rot.ToMatrix(rotMat);

			//ModelMat *= rotMat;

			//Matrix4F scaleMat = Matrix4F::Identity;
			//scaleMat.SetScale({ (float32)Rect.Size.X, TITLE_BAR_HEIGHT, 0 });

			//ModelMat *= scaleMat;

			//Device->DrawMesh(m_QuadMesh, ModelMat, m_TitleBarMaterial);
		}
	}
}