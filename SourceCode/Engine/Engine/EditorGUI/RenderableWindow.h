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
					if (&m_Window->m_CloseButton == Button)
						m_Window->OnInternalClosing();
					else if (&m_Window->m_MaximizeButton == Button)
						m_Window->OnInternalMaximize();
					else if (&m_Window->m_RestoreButton == Button)
						m_Window->OnInternalRestore();
					else if (&m_Window->m_MinimizeButton == Button)
						m_Window->OnInternalMinimize();
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

				virtual void OnMaximize(RenderableWindow* Window)
				{
				}

				virtual void OnRestore(RenderableWindow* Window)
				{
				}

				virtual void OnMinimize(RenderableWindow* Window)
				{
				}
			};

			LISTENER_DECLARATION(IListener)

		public:
			RenderableWindow(void);
			virtual ~RenderableWindow(void)
			{
			}

			virtual void Render(EditorRenderDeviceBase* Device) override;

		protected:
			virtual void OnPositionChanged(void) override
			{
				Control::OnPositionChanged();

				OnSizeChanged();
			}
			virtual void OnSizeChanged(void) override;

		public:
			virtual const RectI& GetClientRect(void) const override
			{
				return m_ClientRect;
			}

			virtual const WString& GetTitle(void) const
			{
				return m_TitleText.GetText();
			}
			virtual void SetTitle(const String& Value)
			{
				m_TitleText.SetText(Value);
			}
			virtual void SetTitle(const WString& Value)
			{
				m_TitleText.SetText(Value);
			}

		protected:
			void SetIsMaximized(bool Value)
			{
				m_MaximizeButton.SetIsVisible(!Value);
				m_RestoreButton.SetIsVisible(Value);
			}

			virtual void OnClosing(void)
			{
			}

			virtual void OnMaximize(void)
			{
			}

			virtual void OnRestore(void)
			{
			}

			virtual void OnMinimize(void)
			{
			}

		private:
			void OnInternalClosing(void)
			{
				OnClosing();

				CALL_CALLBACK(IListener, OnClosing, this);
			}

			void OnInternalMaximize(void)
			{
				OnMaximize();

				CALL_CALLBACK(IListener, OnMaximize, this);
			}

			void OnInternalRestore(void)
			{
				OnRestore();

				CALL_CALLBACK(IListener, OnRestore, this);
			}

			void OnInternalMinimize(void)
			{
				OnMinimize();

				CALL_CALLBACK(IListener, OnMinimize, this);
			}

		private:
			RectI m_ClientRect;

			ButtonListener m_ButtonListener;

			SpriteRenderer m_BackgroundSprite;
			TextRenderer m_TitleText;

			Button m_CloseButton;
			Button m_MaximizeButton;
			Button m_RestoreButton;
			Button m_MinimizeButton;
		};
	}
}

#endif