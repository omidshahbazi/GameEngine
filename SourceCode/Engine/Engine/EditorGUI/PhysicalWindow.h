// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PHYSICAL_WINDOW_H
#define PHYSICAL_WINDOW_H

#include <EditorGUI\RenderableWindow.h>
#include <Rendering\RenderWindow.h>

namespace Engine
{
	using namespace Rendering;
	using namespace ResourceSystem;

	namespace EditorGUI
	{
		class EDITORGUI_API PhysicalWindow : public RenderWindow, public RenderableWindow
		{
		public:
			PhysicalWindow(void);
			virtual ~PhysicalWindow(void);

			void RenderAll(EditorRenderDeviceBase* Device) const override;

			virtual void SetSize(const Vector2I& Value)
			{
				RenderableWindow::SetSize(Value);
			}
			virtual const Vector2I& GetSize(void) const
			{
				return RenderableWindow::GetSize();
			}

			virtual const RectI& GetClientRect(void) const override
			{
				return RenderableWindow::GetClientRect();
			}

			virtual const WString& GetTitle(void) const
			{
				return RenderableWindow::GetTitle();
			}
			virtual void SetTitle(const String& Value)
			{
				RenderableWindow::SetTitle(Value);
			}
			virtual void SetTitle(const WString& Value)
			{
				RenderableWindow::SetTitle(Value);
			}

		protected:
			virtual void OnSizeChanged(void) override;

			virtual void OnClosing(void) override;
			virtual void OnMaximize(void) override;
			virtual void OnRestore(void) override;
			virtual void OnMinimize(void) override;

		private:
			void UpdateSizeFromRenderWindow(void);
			void UpdateTitleSize(void);

			void OnSizeChangedHandler(Window* Window)
			{
				UpdateSizeFromRenderWindow();

				SetIsMaximized(Window->GetState() == Window::States::Maximized);
			}
			DECLARE_MEMBER_EVENT_LISTENER(PhysicalWindow, OnSizeChangedHandler);

			void OnKeyDownHandler(Window* Window, PlatformWindow::VirtualKeys Key)
			{
				OnInternalKeyDown(Key);
			}
			DECLARE_MEMBER_EVENT_LISTENER(PhysicalWindow, OnKeyDownHandler);

			void OnKeyUpHandler(Window* Window, PlatformWindow::VirtualKeys Key)
			{
				OnInternalKeyUp(Key);
			}
			DECLARE_MEMBER_EVENT_LISTENER(PhysicalWindow, OnKeyUpHandler);

			void OnKeyPressedHandler(Window* Window, PlatformWindow::VirtualKeys Key)
			{
				OnInternalKeyPressed(Key);
			}
			DECLARE_MEMBER_EVENT_LISTENER(PhysicalWindow, OnKeyPressedHandler);

			void OnMouseDownHandler(Window* Window, PlatformWindow::VirtualKeys Key, const Vector2I& Position)
			{
				OnInternalMouseDown(Key, Position);
			}
			DECLARE_MEMBER_EVENT_LISTENER(PhysicalWindow, OnMouseDownHandler);

			void OnMouseUpHandler(Window* Window, PlatformWindow::VirtualKeys Key, const Vector2I& Position)
			{
				OnInternalMouseUp(Key, Position);
			}
			DECLARE_MEMBER_EVENT_LISTENER(PhysicalWindow, OnMouseUpHandler);

			void OnMouseClickHandler(Window* Window, PlatformWindow::VirtualKeys Key, const Vector2I& Position)
			{
				OnInternalMouseClick(Key, Position);
			}
			DECLARE_MEMBER_EVENT_LISTENER(PhysicalWindow, OnMouseClickHandler);

			void OnMouseWheelHandler(Window* Window, const Vector2I& Position, uint16 Delta)
			{
				OnInternalMouseWheel(Position, Delta);
			}
			DECLARE_MEMBER_EVENT_LISTENER(PhysicalWindow, OnMouseWheelHandler);

			void OnMouseMoveHandler(Window* Window, const Vector2I& Position)
			{
				OnInternalMouseMove(Position);
			}
			DECLARE_MEMBER_EVENT_LISTENER(PhysicalWindow, OnMouseMoveHandler);

			void OnMouseLeaveHandler(Window* Window)
			{
				OnInternalMouseLeave();
			}
			DECLARE_MEMBER_EVENT_LISTENER(PhysicalWindow, OnMouseLeaveHandler);

		private:
			RenderContext* m_RenderContext;

			bool m_ShouldUpdateSizeFromRenderableWindow;
			bool m_ShouldUpdateSizeFromRenderWindow;
		};
	}
}

#endif