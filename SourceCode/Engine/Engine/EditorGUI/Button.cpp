// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Button.h>
#include <EditorGUI\Private\Resources.h>

#include <iostream>

namespace Engine
{
	namespace EditorGUI
	{
		using namespace Private;

		Button::Button(void) :
			m_IsFirstTimeSet(true),
			m_NormalSprite(nullptr),
			m_HoveredSprite(nullptr),
			m_PressedSprite(nullptr)
		{
			m_NormalSprite = m_Sprite.GetSprite();

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
			std::cout << "Button OnMouseEnter " << Position.X << ", " << Position.Y << " " << (int)this << std::endl;

			SpriteHandle* sprite = m_NormalSprite;
			if (m_HoveredSprite != nullptr)
				sprite = m_HoveredSprite;

			m_Sprite.SetSprite(sprite);
		}

		void Button::OnMouseLeave(void)
		{
			std::cout << "Button OnMouseLeave " << " " << (int)this << std::endl;

			m_Sprite.SetSprite(m_NormalSprite);
		}
	}
}