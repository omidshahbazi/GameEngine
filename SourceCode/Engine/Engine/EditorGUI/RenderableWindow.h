// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDERABLE_WINDOW_H
#define RENDERABLE_WINDOW_H

#include <EditorGUI\Control.h>
#include <EditorGUI\Button.h>
#include <Rendering\RenderWindow.h>

namespace Engine
{
	using namespace Rendering;
	using namespace ResourceSystem;

	namespace EditorGUI
	{
		class EDITORGUI_API RenderableWindow : public Control
		{
		private:
			class RenderWindowListener : public RenderWindow::IListener
			{
#define CHECK_POSITION() if (!m_Window->GetRect().Contains(Position)) return;

			public:
				RenderWindowListener(RenderableWindow* Window) :
					m_Window(Window)
				{
				}

			private:
				virtual void OnPositionChanged(Window* Window) override
				{
					// Nothing
				}

				virtual void OnSizeChanged(Window* Window) override
				{
					// Nothing
				}

				virtual void OnKeyDown(Window* Window, PlatformWindow::VirtualKeys Key) override
				{
					m_Window->OnInternalKeyDown(Key);
				}

				virtual void OnKeyUp(Window* Window, PlatformWindow::VirtualKeys Key) override
				{
					m_Window->OnInternalKeyUp(Key);
				}

				virtual void OnKeyPressed(Window* Window, PlatformWindow::VirtualKeys Key) override
				{
					m_Window->OnInternalKeyPressed(Key);
				}

				virtual void OnMouseDown(Window* Window, PlatformWindow::VirtualKeys Key, const Vector2I& Position) override
				{
					CHECK_POSITION();

					m_Window->OnInternalMouseDown(Key, Position);
				}

				virtual void OnMouseUp(Window* Window, PlatformWindow::VirtualKeys Key, const Vector2I& Position) override
				{
					CHECK_POSITION();

					m_Window->OnInternalMouseUp(Key, Position);
				}

				virtual void OnMouseClick(Window* Window, PlatformWindow::VirtualKeys Key, const Vector2I& Position) override
				{
					CHECK_POSITION();

					m_Window->OnInternalMouseClick(Key, Position);
				}

				virtual void OnMouseWheel(Window* Window, const Vector2I& Position, uint16 Delta) override
				{
					CHECK_POSITION();

					m_Window->OnInternalMouseWheel(Position, Delta);
				}

				virtual void OnMouseMove(Window* Window, const Vector2I& Position) override
				{
					CHECK_POSITION();

					m_Window->OnInternalMouseMove(Position);
				}

				virtual void OnMouseLeave(Window* Window) override
				{
				}

				virtual void OnClosing(Window* Window) override
				{
				}

			private:
				RenderableWindow* m_Window;

#undef CHECK_AND_CONVERT_POSITION()
			};

		public:
			RenderableWindow(void);
			virtual ~RenderableWindow(void)
			{
			}

			virtual void Render(EditorRenderDeviceBase* Device) const override;

			virtual void OnSizeChanged(void);

			virtual const RectI& GetClientRect(void) const override
			{
				return m_ClientRect;
			}

			INLINE RenderWindow::IListener* GetWindowListener(void)
			{
				return &m_RenderWindowListener;
			}

		private:
			RectI m_ClientRect;

			RenderWindowListener m_RenderWindowListener;

			SpriteRenderer m_BackgroundSprite;
			SpriteRenderer m_TitlebarSprite;

			Button m_CloseButton;
		};
	}
}

#endif