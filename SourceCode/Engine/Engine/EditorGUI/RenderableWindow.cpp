// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\RenderableWindow.h>
#include <EditorGUI\Private\Resources.h>

#include <EditorGUI\Private\EditorGUIAllocators.h>

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
			EditorGUIAllocators::Create();

			m_BackgroundSprite.SetSprite(Resources::GetInstance()->GetSprite("WindowBackground.png"));
			m_BackgroundSprite.SetDrawMode(SpriteRenderer::DrawModes::Tiled);

			m_TitleText.SetDimension(TITLE_TEXT_SIZE);
			m_TitleText.SetSize(25);

			AddChild(&m_CloseButton);
			m_CloseButton.AddListener(&m_ButtonListener);
			m_CloseButton.SetNormalSprite(Resources::GetInstance()->GetSprite("WindowButton_Close_Normal.png"));
			m_CloseButton.SetHoveredSprite(Resources::GetInstance()->GetSprite("WindowButton_Close_Hovered.png"));
			m_CloseButton.SetPressedSprite(Resources::GetInstance()->GetSprite("WindowButton_Close_Pressed.png"));
			m_CloseButton.SetSize({ CLOSE_BUTTON_SIZE.X, CLOSE_BUTTON_SIZE.Y });

			AddChild(&m_MaximizeButton);
			m_MaximizeButton.AddListener(&m_ButtonListener);
			m_MaximizeButton.SetNormalSprite(Resources::GetInstance()->GetSprite("WindowButton_Maximize_Normal.png"));
			m_MaximizeButton.SetHoveredSprite(Resources::GetInstance()->GetSprite("WindowButton_Maximize_Hovered.png"));
			m_MaximizeButton.SetPressedSprite(Resources::GetInstance()->GetSprite("WindowButton_Maximize_Pressed.png"));
			m_MaximizeButton.SetDisabledSprite(Resources::GetInstance()->GetSprite("WindowButton_Maximize_Disabled.png"));
			m_MaximizeButton.SetSize({ SIZE_BUTTON_SIZE.X, SIZE_BUTTON_SIZE.Y });

			AddChild(&m_RestoreButton);
			m_RestoreButton.AddListener(&m_ButtonListener);
			m_RestoreButton.SetNormalSprite(Resources::GetInstance()->GetSprite("WindowButton_Restore_Normal.png"));
			m_RestoreButton.SetHoveredSprite(Resources::GetInstance()->GetSprite("WindowButton_Restore_Hovered.png"));
			m_RestoreButton.SetPressedSprite(Resources::GetInstance()->GetSprite("WindowButton_Restore_Pressed.png"));
			m_RestoreButton.SetSize({ SIZE_BUTTON_SIZE.X, SIZE_BUTTON_SIZE.Y });
			m_RestoreButton.SetIsVisible(false);

			AddChild(&m_MinimizeButton);
			m_MinimizeButton.AddListener(&m_ButtonListener);
			m_MinimizeButton.SetNormalSprite(Resources::GetInstance()->GetSprite("WindowButton_Minimize_Normal.png"));
			m_MinimizeButton.SetHoveredSprite(Resources::GetInstance()->GetSprite("WindowButton_Minimize_Hovered.png"));
			m_MinimizeButton.SetPressedSprite(Resources::GetInstance()->GetSprite("WindowButton_Minimize_Pressed.png"));
			m_MinimizeButton.SetDisabledSprite(Resources::GetInstance()->GetSprite("WindowButton_Minimize_Disabled.png"));
			m_MinimizeButton.SetSize({ MINIMIZE_BUTTON_SIZE.X, MINIMIZE_BUTTON_SIZE.Y });



			Button* button = EditorGUIAllocators::TypesAllocator_Allocate<Button>();
			Construct(button);

			AddChild(button);
			button->SetNormalSprite(Resources::GetInstance()->GetSprite("Button.png"));
			button->SetSize({ 275, 80 });
			button->SetPosition({ 100, 100 });
			button->SetDrawMode(SpriteRenderer::DrawModes::Sliced);
			button->SetBackColor(ColorUI8::Red);
			button->SetText("Tysg Windyw\n TTTTTTTT5555531313151313\n\n!");
			button->SetTextSize(20);
			button->SetIsMultiLine(true);
		}

		void RenderableWindow::Render(EditorRenderDeviceBase* Device)
		{
			auto& rect = GetRect();

			m_BackgroundSprite.Render(Device, rect.Position);
			m_TitleText.Render(Device, rect.Position);
		}

		void RenderableWindow::OnSizeChanged(void)
		{
			Control::OnSizeChanged();

			auto& rect = GetRect();

			m_ClientRect = rect;
			m_ClientRect.Position.Y += TITLE_BAR_HEIGHT;
			m_ClientRect.Size.Y -= TITLE_BAR_HEIGHT;

			m_BackgroundSprite.SetDimension(rect.Size);

			m_CloseButton.SetPosition({ rect.Size.X - (CONTROL_BUTTON_MARGIN + CLOSE_BUTTON_SIZE.X), -TITLE_BAR_HEIGHT });

			m_MaximizeButton.SetPosition({ rect.Size.X - (CONTROL_BUTTON_MARGIN + CLOSE_BUTTON_SIZE.X + SIZE_BUTTON_SIZE.X), -TITLE_BAR_HEIGHT });

			m_RestoreButton.SetPosition({ rect.Size.X - (CONTROL_BUTTON_MARGIN + CLOSE_BUTTON_SIZE.X + SIZE_BUTTON_SIZE.X), -TITLE_BAR_HEIGHT });

			m_MinimizeButton.SetPosition({ rect.Size.X - (CONTROL_BUTTON_MARGIN + CLOSE_BUTTON_SIZE.X + SIZE_BUTTON_SIZE.X + MINIMIZE_BUTTON_SIZE.X), -TITLE_BAR_HEIGHT });
		}
	}
}