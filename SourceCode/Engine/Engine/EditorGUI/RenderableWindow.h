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
					m_Window->OnInternalMouseDown(Key, Position);
				}

				virtual void OnMouseUp(Window* Window, PlatformWindow::VirtualKeys Key, const Vector2I& Position) override
				{
					m_Window->OnInternalMouseUp(Key, Position);
				}

				virtual void OnMouseClick(Window* Window, PlatformWindow::VirtualKeys Key, const Vector2I& Position) override
				{
					m_Window->OnInternalMouseClick(Key, Position);
				}

				virtual void OnMouseWheel(Window* Window, const Vector2I& Position, uint16 Delta) override
				{
					m_Window->OnInternalMouseWheel(Position, Delta);
				}

				virtual void OnMouseMove(Window* Window, const Vector2I& Position) override
				{
					m_Window->OnInternalMouseMove(Position);
				}

				virtual void OnMouseLeave(Window* Window) override
				{
					m_Window->OnInternalMouseLeave();
				}

				virtual void OnClosing(Window* Window) override
				{
				}

			private:
				RenderableWindow* m_Window;
			};

			class ButtonListener : public Button::IListener
			{
			public:
				ButtonListener(RenderableWindow* Window) :
					m_Window(Window)
				{
				}

			private:
				virtual void OnClicked(Button* Button) override
				{
					m_Window->OnInternalClosing();
				}

			private:
				RenderableWindow* m_Window;
			};

		public:
			class IListener : public Control::IListener
			{
			public:
				virtual void OnClosing(RenderableWindow* Window)
				{
				}
			};

			LISTENER_DECLARATION(IListener)

		public:
			RenderableWindow(void);
			virtual ~RenderableWindow(void)
			{
			}

			virtual void Render(EditorRenderDeviceBase* Device) const override;

			virtual void OnPositionChanged(void)
			{
				OnSizeChanged();
			}
			virtual void OnSizeChanged(void);

			virtual const RectI& GetClientRect(void) const override
			{
				return m_ClientRect;
			}

			INLINE RenderWindow::IListener* GetWindowListener(void)
			{
				return &m_RenderWindowListener;
			}

		protected:
			virtual void OnClosing(void)
			{
			}

		private:
			void OnInternalClosing(void)
			{
				OnClosing();

				CALL_CALLBACK(IListener, OnClosing, this);
			}

		private:
			RectI m_ClientRect;

			RenderWindowListener m_RenderWindowListener;
			ButtonListener m_ButtonListener;

			SpriteRenderer m_BackgroundSprite;

			Button m_CloseButton;
		};
	}
}

#endif