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
			Matrix4F mat;
			mat.MakeIdentity();

			Device->DrawMesh(m_QuadMesh, mat, m_TitleBarMaterial);
		}
	}
}