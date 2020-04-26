// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\RenderableWindow.h>
#include <EditorGUI\Private\Resources.h>

#include <iostream>

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
			m_RenderWindowListener(this)
		{
			m_BackgroundSprite.SetSprite(Resources::GetGetSprite("WindowBackground.png"));
			m_BackgroundSprite.SetDrawMode(SpriteRenderer::DrawModes::Tiled);

			m_CloseButton.SetSprite(Resources::GetGetSprite("WindowButton_Close_Normal.png"));

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




		void RenderableWindow::OnMouseDown(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		{
			std::cout << "OnMouseDown " << (int32)Key << " " << Position.X << ", " << Position.Y << std::endl;
		}

		void RenderableWindow::OnMouseUp(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		{
			std::cout << "OnMouseUp " << (int32)Key << " " << Position.X << ", " << Position.Y << std::endl;
		}

		void RenderableWindow::OnMouseClick(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		{
			std::cout << "OnMouseClick " << (int32)Key << " " << Position.X << ", " << Position.Y << std::endl;
		}

		void RenderableWindow::OnMouseWheel(const Vector2I& Position, uint16 Delta)
		{
			std::cout << "OnMouseWheel " << Position.X << ", " << Position.Y << " " << Delta << std::endl;
		}

		void RenderableWindow::OnMouseEnter(const Vector2I& Position)
		{
			std::cout << "OnMouseEnter " << Position.X << ", " << Position.Y << std::endl;
		}

		void RenderableWindow::OnMouseMove(const Vector2I& Position)
		{
			std::cout << "OnMouseMove " << Position.X << ", " << Position.Y << std::endl;
		}

		void RenderableWindow::OnMouseLeave(void)
		{
			std::cout << "OnMouseLeave" << std::endl;
		}
	}
}