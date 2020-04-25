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
		const int32 CONTROL_BUTTON_MARGIN = 10;
		const Vector2I CLOSE_BUTTON_SIZE = { 35, 15 };

		RenderableWindow::RenderableWindow(void) :
			m_RenderWindowListener(this)
		{
			//m_BackgroundSprite.SetColor({ 30, 12, 12, 255 });
			//m_TitlebarSprite.SetColor({ 255, 0, 255, 255 });

			AddChild(&m_CloseButton);
		}

		void RenderableWindow::Render(EditorRenderDeviceBase* Device) const
		{
			auto& rect = GetRect();

			//m_BackgroundSprite.Render(Device, rect.Position);
			//m_TitlebarSprite.Render(Device, rect.Position);
		}

		void RenderableWindow::OnSizeChanged(void)
		{
			auto& rect = GetRect();

			m_ClientRect = rect;
			m_ClientRect.Position.Y += TITLE_BAR_HEIGHT;
			m_ClientRect.Size.Y -= TITLE_BAR_HEIGHT;

			//m_BackgroundSprite.SetElementDimension(rect.Size);
			//m_TitlebarSprite.SetElementDimension({ rect.Size.X, TITLE_BAR_HEIGHT });

			//m_CloseButton.SetRect({ rect.Size.X - (CONTROL_BUTTON_MARGIN + CLOSE_BUTTON_SIZE.X), -TITLE_BAR_HEIGHT, CLOSE_BUTTON_SIZE.X, CLOSE_BUTTON_SIZE.Y });
			m_CloseButton.SetRect({ 0, -32,32, 32 });
		}
	}
}