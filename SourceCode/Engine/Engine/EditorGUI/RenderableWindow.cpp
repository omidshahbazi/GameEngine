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
		const Vector2I SIZE_BUTTON_SIZE = { 23, 18 };
		const Vector2I MINIMIZE_BUTTON_SIZE = { 27, 18 };

		RenderableWindow::RenderableWindow(void) :
			m_RenderWindowListener(this),
			m_ButtonListener(this)
		{
			m_BackgroundSprite.SetSprite(Resources::GetGetSprite("WindowBackground.png"));
			m_BackgroundSprite.SetDrawMode(SpriteRenderer::DrawModes::Tiled);

			AddChild(&m_CloseButton);
			m_CloseButton.AddListener(&m_ButtonListener);
			m_CloseButton.SetNormalSprite(Resources::GetGetSprite("WindowButton_Close_Normal.png"));
			m_CloseButton.SetHoveredSprite(Resources::GetGetSprite("WindowButton_Close_Hovered.png"));
			m_CloseButton.SetPressedSprite(Resources::GetGetSprite("WindowButton_Close_Pressed.png"));

			AddChild(&m_SizeButton);
			m_SizeButton.AddListener(&m_ButtonListener);
			m_SizeButton.SetNormalSprite(Resources::GetGetSprite("WindowButton_Maximize_Normal.png"));
			m_SizeButton.SetHoveredSprite(Resources::GetGetSprite("WindowButton_Maximize_Hovered.png"));
			m_SizeButton.SetPressedSprite(Resources::GetGetSprite("WindowButton_Maximize_Pressed.png"));
			m_SizeButton.SetDisabledSprite(Resources::GetGetSprite("WindowButton_Maximize_Disabled.png"));

			//AddChild(&m_MinimizeButton);
			m_MinimizeButton.AddListener(&m_ButtonListener);
			m_MinimizeButton.SetNormalSprite(Resources::GetGetSprite("WindowButton_Minimize_Normal.png"));
			m_MinimizeButton.SetHoveredSprite(Resources::GetGetSprite("WindowButton_Minimize_Hovered.png"));
			m_MinimizeButton.SetPressedSprite(Resources::GetGetSprite("WindowButton_Minimize_Pressed.png"));
			m_MinimizeButton.SetDisabledSprite(Resources::GetGetSprite("WindowButton_Minimize_Disabled.png"));
		}

		void RenderableWindow::Render(EditorRenderDeviceBase* Device)
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
			m_SizeButton.SetRect({ rect.Size.X - (CONTROL_BUTTON_MARGIN + CLOSE_BUTTON_SIZE.X + SIZE_BUTTON_SIZE.X), -TITLE_BAR_HEIGHT, SIZE_BUTTON_SIZE.X, SIZE_BUTTON_SIZE.Y });
			m_MinimizeButton.SetRect({ rect.Size.X - (CONTROL_BUTTON_MARGIN + CLOSE_BUTTON_SIZE.X + SIZE_BUTTON_SIZE.X + MINIMIZE_BUTTON_SIZE.X), -TITLE_BAR_HEIGHT, MINIMIZE_BUTTON_SIZE.X, MINIMIZE_BUTTON_SIZE.Y });
		}
	}
}