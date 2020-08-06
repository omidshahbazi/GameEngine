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
		const int32 TITLE_TEXT_MARGIN = 15;
		const int32 CONTROL_BUTTON_MARGIN = 2;
		const Vector2I CLOSE_BUTTON_SIZE = { 44, 18 };
		const Vector2I SIZE_BUTTON_SIZE = { 23, 18 };
		const Vector2I MINIMIZE_BUTTON_SIZE = { 27, 18 };

		float size = 16;

		RenderableWindow::RenderableWindow(void) :
			m_ButtonListener(this)
		{
			m_BackgroundSprite.SetSprite(Resources::GetGetSprite("WindowBackground.png"));
			m_BackgroundSprite.SetDrawMode(SpriteRenderer::DrawModes::Tiled);

			m_TitleText.SetText("Test Window Title\n\t!@#$%^&*)(~[];'\\/.,<>?|\"{}_+:}\n1234567890");
			m_TitleText.SetSize(8);

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

			AddChild(&m_MinimizeButton);
			m_MinimizeButton.AddListener(&m_ButtonListener);
			m_MinimizeButton.SetNormalSprite(Resources::GetGetSprite("WindowButton_Minimize_Normal.png"));
			m_MinimizeButton.SetHoveredSprite(Resources::GetGetSprite("WindowButton_Minimize_Hovered.png"));
			m_MinimizeButton.SetPressedSprite(Resources::GetGetSprite("WindowButton_Minimize_Pressed.png"));
			m_MinimizeButton.SetDisabledSprite(Resources::GetGetSprite("WindowButton_Minimize_Disabled.png"));



			//AddChild(&m_SampleButton);
			//m_SampleButton.SetSize({ 100, 100 });
			//m_SampleButton.SetPosition({ 100, 100 });
			//m_SampleButton.SetRotation(30);
			//m_SampleButton.SetNormalSprite(Resources::GetGetSprite("WindowButton_Minimize_Normal.png"));
		}

		void RenderableWindow::Render(EditorRenderDeviceBase* Device)
		{
			m_TitleText.SetSize(size);


			auto& rect = GetRect();

			m_BackgroundSprite.Render(Device, rect.Position);
			//m_TitleText.Render(Device, { rect.Position.X + TITLE_TEXT_MARGIN, rect.Position.Y });

			m_TitleText.Render(Device, { 100, 100 });
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
			m_CloseButton.SetSize({ CLOSE_BUTTON_SIZE.X, CLOSE_BUTTON_SIZE.Y });

			m_SizeButton.SetPosition({ rect.Size.X - (CONTROL_BUTTON_MARGIN + CLOSE_BUTTON_SIZE.X + SIZE_BUTTON_SIZE.X), -TITLE_BAR_HEIGHT });
			m_SizeButton.SetSize({ SIZE_BUTTON_SIZE.X, SIZE_BUTTON_SIZE.Y });

			m_MinimizeButton.SetPosition({ rect.Size.X - (CONTROL_BUTTON_MARGIN + CLOSE_BUTTON_SIZE.X + SIZE_BUTTON_SIZE.X + MINIMIZE_BUTTON_SIZE.X), -TITLE_BAR_HEIGHT });
			m_MinimizeButton.SetSize({ MINIMIZE_BUTTON_SIZE.X, MINIMIZE_BUTTON_SIZE.Y });
		}

		void RenderableWindow::OnKeyUp(PlatformWindow::VirtualKeys Key)
		{
			if (Key == PlatformWindow::VirtualKeys::Up)
				++size;

			else if (Key == PlatformWindow::VirtualKeys::Down)
				--size;
		}
	}
}