// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef BUTTON_H
#define BUTTON_H

#include <EditorGUI\Control.h>
#include <EditorGUI\Private\SpriteRenderer.h>
#include <Rendering\RenderWindow.h>

namespace Engine
{
	using namespace Rendering;
	using namespace ResourceSystem;

	namespace EditorGUI
	{
		using namespace Private;

		class EDITORGUI_API Button : public Control
		{
		public:
			Button(void);
			virtual ~Button(void)
			{
			}

			virtual void Render(EditorRenderDeviceBase* Device) const override;

			virtual void OnSizeChanged(void) override;

			virtual void OnMouseEnter(const Vector2I& Position) override;
			virtual void OnMouseLeave(void) override;

			const ColorUI8& GetColor(void) const
			{
				return m_Sprite.GetColor();
			}
			void SetColor(const ColorUI8& Value)
			{
				m_Sprite.SetColor(Value);
			}

			SpriteRenderer::DrawModes GetDrawMode(void) const
			{
				return m_Sprite.GetDrawMode();
			}
			void SetDrawMode(SpriteRenderer::DrawModes Value)
			{
				m_Sprite.SetDrawMode(Value);
			}

			SpriteHandle* GetSprite(void) const
			{
				return m_Sprite.GetSprite();
			}
			void SetSprite(SpriteHandle* Value)
			{
				m_Sprite.SetSprite(Value);
			}

		private:
			SpriteRenderer m_Sprite;
		};
	}
}

#endif