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
		const Vector2I TITLE_TEXT_SIZE = { 33, 100 };
		const Vector2I TITLE_TEXT_MARGIN = { 15, 2 };
		const int32 CONTROL_BUTTON_MARGIN = 2;
		const Vector2I CLOSE_BUTTON_SIZE = { 44, 18 };
		const Vector2I SIZE_BUTTON_SIZE = { 23, 18 };
		const Vector2I MINIMIZE_BUTTON_SIZE = { 27, 18 };

		RenderableWindow::RenderableWindow(void) :
			m_ButtonListener(this)
		{
			m_BackgroundSprite.SetSprite(Resources::GetGetSprite("WindowBackground.png"));
			m_BackgroundSprite.SetDrawMode(SpriteRenderer::DrawModes::Tiled);

			m_TitleText.SetText("Test Window Title");
			m_TitleText.SetSize(15);

			AddChild(&m_CloseButton);
			m_CloseButton.AddListener(&m_ButtonListener);
			m_CloseButton.SetNormalSprite(Resources::GetGetSprite("WindowButton_Close_Normal.png"));
			m_CloseButton.SetHoveredSprite(Resources::GetGetSprite("WindowButton_Close_Hovered.png"));
			m_CloseButton.SetPressedSprite(Resources::GetGetSprite("WindowButton_Close_Pressed.png"));
			m_CloseButton.SetSize({ CLOSE_BUTTON_SIZE.X, CLOSE_BUTTON_SIZE.Y });

			AddChild(&m_MaximizeButton);
			m_MaximizeButton.AddListener(&m_ButtonListener);
			m_MaximizeButton.SetNormalSprite(Resources::GetGetSprite("WindowButton_Maximize_Normal.png"));
			m_MaximizeButton.SetHoveredSprite(Resources::GetGetSprite("WindowButton_Maximize_Hovered.png"));
			m_MaximizeButton.SetPressedSprite(Resources::GetGetSprite("WindowButton_Maximize_Pressed.png"));
			m_MaximizeButton.SetDisabledSprite(Resources::GetGetSprite("WindowButton_Maximize_Disabled.png"));
			m_MaximizeButton.SetSize({ SIZE_BUTTON_SIZE.X, SIZE_BUTTON_SIZE.Y });

			AddChild(&m_RestoreButton);
			m_RestoreButton.AddListener(&m_ButtonListener);
			m_RestoreButton.SetNormalSprite(Resources::GetGetSprite("WindowButton_Restore_Normal.png"));
			m_RestoreButton.SetHoveredSprite(Resources::GetGetSprite("WindowButton_Restore_Hovered.png"));
			m_RestoreButton.SetPressedSprite(Resources::GetGetSprite("WindowButton_Restore_Pressed.png"));
			m_RestoreButton.SetSize({ SIZE_BUTTON_SIZE.X, SIZE_BUTTON_SIZE.Y });
			m_RestoreButton.SetIsVisible(false);

			AddChild(&m_MinimizeButton);
			m_MinimizeButton.AddListener(&m_ButtonListener);
			m_MinimizeButton.SetNormalSprite(Resources::GetGetSprite("WindowButton_Minimize_Normal.png"));
			m_MinimizeButton.SetHoveredSprite(Resources::GetGetSprite("WindowButton_Minimize_Hovered.png"));
			m_MinimizeButton.SetPressedSprite(Resources::GetGetSprite("WindowButton_Minimize_Pressed.png"));
			m_MinimizeButton.SetDisabledSprite(Resources::GetGetSprite("WindowButton_Minimize_Disabled.png"));
			m_MinimizeButton.SetSize({ MINIMIZE_BUTTON_SIZE.X, MINIMIZE_BUTTON_SIZE.Y });
		}

		void RenderableWindow::Render(EditorRenderDeviceBase* Device)
		{
			auto& rect = GetRect();

			m_BackgroundSprite.Render(Device, rect.Position);
			m_TitleText.Render(Device, Vector2I(rect.Position.X, (int32)(rect.Position.Y + m_TitleText.GetSize())) + TITLE_TEXT_MARGIN);
		}

		void RenderableWindow::OnSizeChanged(void)
		{
			Control::OnSizeChanged();

			auto& rect = GetRect();

			m_ClientRect = rect;
			m_ClientRect.Position.Y += TITLE_BAR_HEIGHT;
			m_ClientRect.Size.Y -= TITLE_BAR_HEIGHT;

			m_BackgroundSprite.SetDimension(rect.Size);
			m_TitleText.SetDimension(TITLE_TEXT_SIZE);

			m_CloseButton.SetPosition({ rect.Size.X - (CONTROL_BUTTON_MARGIN + CLOSE_BUTTON_SIZE.X), -TITLE_BAR_HEIGHT });

			m_MaximizeButton.SetPosition({ rect.Size.X - (CONTROL_BUTTON_MARGIN + CLOSE_BUTTON_SIZE.X + SIZE_BUTTON_SIZE.X), -TITLE_BAR_HEIGHT });

			m_RestoreButton.SetPosition({ rect.Size.X - (CONTROL_BUTTON_MARGIN + CLOSE_BUTTON_SIZE.X + SIZE_BUTTON_SIZE.X), -TITLE_BAR_HEIGHT });

			m_MinimizeButton.SetPosition({ rect.Size.X - (CONTROL_BUTTON_MARGIN + CLOSE_BUTTON_SIZE.X + SIZE_BUTTON_SIZE.X + MINIMIZE_BUTTON_SIZE.X), -TITLE_BAR_HEIGHT });
		}
	}
}