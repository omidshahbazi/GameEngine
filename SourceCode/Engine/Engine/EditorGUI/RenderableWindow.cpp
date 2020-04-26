// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\RenderableWindow.h>
#include <EditorGUI\Private\Resources.h>

namespace Engine
{
	using namespace ResourceSystem;

	namespace EditorGUI
	{
		using namespace Private;

		const int32 TITLE_BAR_HEIGHT = 30;
		const int32 CONTROL_BUTTON_MARGIN = 2;
		const Vector2I CLOSE_BUTTON_SIZE = { 44, 18 };

		RenderableWindow::RenderableWindow(void) :
			m_RenderWindowListener(this),
			m_ButtonListener(this)
		{
			m_BackgroundSprite.SetSprite(Resources::GetGetSprite("WindowBackground.png"));
			m_BackgroundSprite.SetDrawMode(SpriteRenderer::DrawModes::Tiled);

			m_CloseButton.AddListener(&m_ButtonListener);
			m_CloseButton.SetNormalSprite(Resources::GetGetSprite("WindowButton_Close_Normal.png"));
			m_CloseButton.SetHoveredSprite(Resources::GetGetSprite("WindowButton_Close_Hovered.png"));
			m_CloseButton.SetPressedSprite(Resources::GetGetSprite("WindowButton_Close_Pressed.png"));

			AddChild(&m_CloseButton);
		}

		void RenderableWindow::Render(EditorRenderDeviceBase* Device) const
		{
			auto& rect = GetRect();

			m_BackgroundSprite.Render(Device, rect.Position);
		}

		void RenderableWindow::OnSizeChanged(void)
		{
			auto& rect = GetRect();

			m_ClientRect = rect;
			m_ClientRect.Position.Y += TITLE_BAR_HEIGHT;
			m_ClientRect.Size.Y -= TITLE_BAR_HEIGHT;

			m_BackgroundSprite.SetElementDimension(rect.Size);

			m_CloseButton.SetRect({ rect.Size.X - (CONTROL_BUTTON_MARGIN + CLOSE_BUTTON_SIZE.X), -TITLE_BAR_HEIGHT, CLOSE_BUTTON_SIZE.X, CLOSE_BUTTON_SIZE.Y });
		}
	}
}