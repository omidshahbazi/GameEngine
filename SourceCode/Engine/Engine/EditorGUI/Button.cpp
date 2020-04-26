// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Button.h>
#include <EditorGUI\Private\Resources.h>

#include <iostream>

namespace Engine
{
	namespace EditorGUI
	{
		using namespace Private;

		Button::Button(void)
		{
			SetDrawMode(SpriteRenderer::DrawModes::Simple);
		}

		void Button::Render(EditorRenderDeviceBase* Device) const
		{
			m_Sprite.Render(Device, GetRect().Position);
		}

		void Button::OnSizeChanged(void)
		{
			m_Sprite.SetElementDimension(GetRect().Size);
		}

		//void Button::OnMouseDown(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		//{
		//	std::cout << "OnMouseDown " << (int32)Key << " " << Position.X << ", " << Position.Y << std::endl;
		//}

		//void Button::OnMouseUp(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		//{
		//	std::cout << "OnMouseUp " << (int32)Key << " " << Position.X << ", " << Position.Y << std::endl;
		//}

		//void Button::OnMouseClick(PlatformWindow::VirtualKeys Key, const Vector2I& Position)
		//{
		//	std::cout << "OnMouseClick " << (int32)Key << " " << Position.X << ", " << Position.Y << std::endl;
		//}

		//void Button::OnMouseWheel(const Vector2I& Position, uint16 Delta)
		//{
		//	std::cout << "OnMouseWheel " << Position.X << ", " << Position.Y << " " << Delta << std::endl;
		//}

		//void Button::OnMouseMove(const Vector2I& Position)
		//{
		//	std::cout << "OnMouseMove " << Position.X << ", " << Position.Y << std::endl;
		//}

		void Button::OnMouseLeave(void)
		{
			std::cout << "Button OnMouseLeave" << std::endl;
		}
	}
}