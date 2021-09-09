// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDERABLE_WINDOW_H
#define RENDERABLE_WINDOW_H

#include <EditorGUI\Control.h>
#include <EditorGUI\Button.h>

namespace Engine
{
	using namespace RenderSystem;
	using namespace ResourceSystem;

	namespace EditorGUI
	{
		class EDITORGUI_API RenderableWindow : public Control
		{
		public:
			typedef Delegate<RenderableWindow*> ClosingEventHandler;
			typedef Delegate<RenderableWindow*> MaximizeEventHandler;
			typedef Delegate<RenderableWindow*> RestoreEventHandler;
			typedef Delegate<RenderableWindow*> MinimizeEventHandler;

		public:
			RenderableWindow(void);
			virtual ~RenderableWindow(void)
			{
			}

			virtual void Update(void) override;

			virtual void Render(EditorRenderDeviceBase* Device) const override;

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

				OnClosingEvent(this);
			}

			void OnInternalMaximize(void)
			{
				OnMaximize();

				OnMaximizeEvent(this);
			}

			void OnInternalRestore(void)
			{
				OnRestore();

				OnRestoreEvent(this);
			}

			void OnInternalMinimize(void)
			{
				OnMinimize();

				OnMinimizeEvent(this);
			}

			void OnControlButtonClicked(Button* Button)
			{
				if (&m_CloseButton == Button)
					OnInternalClosing();
				else if (&m_MaximizeButton == Button)
					OnInternalMaximize();
				else if (&m_RestoreButton == Button)
					OnInternalRestore();
				else if (&m_MinimizeButton == Button)
					OnInternalMinimize();
			}
			DECLARE_MEMBER_EVENT_LISTENER(RenderableWindow, OnControlButtonClicked);

		public:
			ClosingEventHandler OnClosingEvent;
			MaximizeEventHandler OnMaximizeEvent;
			RestoreEventHandler OnRestoreEvent;
			MinimizeEventHandler OnMinimizeEvent;

		private:
			RectI m_ClientRect;

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