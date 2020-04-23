// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\RenderableWindow.h>
#include <EditorGUI\Private\Resources.h>

namespace Engine
{
	using namespace ResourceSystem;

	namespace EditorGUI
	{
		using namespace Private;

		const float32 TITLE_BAR_HEIGHT = 25;
		const float32 HALF_TITLE_BAR_HEIGHT = TITLE_BAR_HEIGHT / 2.0F;

		RenderableWindow::RenderableWindow(void) :
			m_RenderWindowListener(this)
		{
			m_BackgroundMaterial = Resources::GetBackgroundMatrial();
			m_TitleBarMaterial = Resources::GetTitleBarMaterial();
		}

		void RenderableWindow::Render(EditorRenderDeviceBase* Device) const
		{
			auto& rect = GetRect();

			Device->DrawMesh(Vector3F::Zero, Vector3F::Zero, Vector3F(rect.Size.X, rect.Size.Y, 0), m_BackgroundMaterial);

			Device->DrawMesh(Vector3F::Zero, Vector3F::Zero, { (float32)rect.Size.X, TITLE_BAR_HEIGHT, 0 }, m_TitleBarMaterial);
		}
	}
}