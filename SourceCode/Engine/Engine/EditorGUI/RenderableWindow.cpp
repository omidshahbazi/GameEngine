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

		RenderableWindow::RenderableWindow(void)
		{
			AddListener(this);

			m_QuadMesh = Resources::GetQuadMesh();
			m_TitleBarMaterial = Resources::GetTitleBarMaterial();

			m_ProjMat.MakeIdentity();
			auto &rect = GetRect();
			m_ProjMat.MakeOrthographicProjectionMatrix(rect.Size.X, rect.Size.Y, -1, 1);
		}

		void RenderableWindow::Render(DeviceInterface* Device) const
		{
			Matrix4F view;
			view.MakeIdentity();
			view.SetPosition(0, 270, 0);

			Matrix4F model;
			model.MakeIdentity();
			//model.SetRotation(0, 0, 45);
			model.SetScale(800, 25, 0);

			Matrix4F mvp = m_ProjMat * view * model;

			Device->DrawMesh(m_QuadMesh, model, view, m_ProjMat, mvp, m_TitleBarMaterial);
		}

		void RenderableWindow::OnRectChanged(Control* Control, const RectI& Rect)
		{
			m_ProjMat.MakeOrthographicProjectionMatrix(Rect.Size.X, Rect.Size.Y, -1, 1);
		}
	}
}