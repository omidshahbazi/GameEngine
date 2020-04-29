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
					else if (&m_Window->m_SizeButton == Button)
						m_Window->OnInternalMaximizeRestore();
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

				virtual void OnMaximizeRestore(RenderableWindow* Window)
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

			virtual void OnPositionChanged(void)
			{
				OnSizeChanged();
			}
			virtual void OnSizeChanged(void);

			virtual const RectI& GetClientRect(void) const override
			{
				return m_ClientRect;
			}

		protected:
			virtual void OnClosing(void)
			{
			}

			virtual void OnMaximizeRestore(void)
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

			void OnInternalMaximizeRestore(void)
			{
				OnMaximizeRestore();

				CALL_CALLBACK(IListener, OnMaximizeRestore, this);
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

			Button m_CloseButton;
			Button m_SizeButton;
			Button m_MinimizeButton;


			Button m_SampleButton;
		};
	}
}

#endif