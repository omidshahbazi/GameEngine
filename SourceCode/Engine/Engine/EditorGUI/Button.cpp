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

		void Button::OnMouseEnter(const Vector2I& Position)
		{
			std::cout << "Button OnMouseEnter " << Position.X << ", " << Position.Y << std::endl;
		}

		void Button::OnMouseLeave(void)
		{
			std::cout << "Button OnMouseLeave" << std::endl;
		}
	}
}