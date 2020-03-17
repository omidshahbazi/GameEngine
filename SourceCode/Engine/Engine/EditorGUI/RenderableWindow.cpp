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

			m_ProjMat = Matrix4F::Identity;
			m_ViewMat = Matrix4F::Identity;
			m_ModelMat = Matrix4F::Identity;

			OnRectChanged(nullptr, GetRect());
		}

		void RenderableWindow::Render(DeviceInterface* Device) const
		{
			QuaternionF quat = QuaternionF::Identity;

			Device->DrawMesh(m_QuadMesh, m_ModelMat, m_ViewMat, m_ProjMat, m_TitleBarMaterial);
		}

		void RenderableWindow::OnRectChanged(Control* Control, const RectI& Rect)
		{
			m_ProjMat.SetOrthographicProjection(Rect.Size.X, Rect.Size.Y, -1, 1);

			//m_ViewMat.SetPosition(Rect.Size.X / -2.0F, Rect.Size.Y / 2.0F, 0);

			m_ModelMat.SetTranslate({ Rect.Size.X / -3.0F, TITLE_BAR_HEIGHT / -2.0F, 0 });

			QuaternionF rot = QuaternionF::FromEuler(Vector3F(0, 0, 30) * Mathematics::DEGREES_TO_RADIANS);
			Matrix4F rotMat;
			rot.ToMatrix(rotMat);

			Matrix4F scaleMat;
			scaleMat.SetScale({ (float32)Rect.Size.X, TITLE_BAR_HEIGHT, 0 });

			m_ModelMat *= rotMat * scaleMat;
		}
	}
}