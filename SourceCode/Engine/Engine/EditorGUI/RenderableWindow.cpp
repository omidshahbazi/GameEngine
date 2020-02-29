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
			m_QuadMesh = Resources::GetQuadMesh();
			m_TitleBarMaterial = Resources::GetTitleBarMaterial();
		}

		void RenderableWindow::Render(DeviceInterface* Device) const
		{
			Matrix4F projectionMat;
			projectionMat.MakeOrthographicProjectionMatrix(800, 600, -1, 1);

			Matrix4F view;
			view.MakeIdentity();

			Matrix4F model;
			model.MakeIdentity();
			model.SetScale(100);

			Matrix4F mvp = projectionMat * view * model;

			Device->DrawMesh(m_QuadMesh, model, view, projectionMat, mvp, m_TitleBarMaterial);
		}
	}
}