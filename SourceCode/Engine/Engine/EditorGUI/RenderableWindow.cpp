// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\RenderableWindow.h>
#include <EditorGUI\Private\Resources.h>

namespace Engine
{
	using namespace ResourceSystem;

	namespace EditorGUI
	{
		using namespace Private;

		const int32 TITLE_BAR_HEIGHT = 25;

		RenderableWindow::RenderableWindow(void) :
			m_RenderWindowListener(this)
		{
			m_BackgroundSprite.SetColor({ 30, 12, 12, 255 });
			m_TitlebarSprite.SetColor({ 255, 0, 255, 255 });
		}

		void RenderableWindow::Render(EditorRenderDeviceBase* Device) const
		{
			m_BackgroundSprite.Render(Device);
			m_TitlebarSprite.Render(Device);
		}

		void RenderableWindow::OnSizeChanged(void)
		{
			auto& rect = GetRect();

			m_BackgroundSprite.SetElementDimension(rect.Size);
			m_TitlebarSprite.SetElementDimension({ rect.Size.X, TITLE_BAR_HEIGHT });
		}
	}
}