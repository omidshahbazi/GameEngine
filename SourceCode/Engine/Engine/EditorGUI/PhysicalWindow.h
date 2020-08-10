// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PHYSICAL_WINDOW_H
#define PHYSICAL_WINDOW_H

#include <EditorGUI\RenderableWindow.h>

namespace Engine
{
	using namespace Rendering;
	using namespace ResourceSystem;

	namespace EditorGUI
	{
		class EDITORGUI_API PhysicalWindow : public RenderableWindow
		{
		private:
			class RenderWindowListener : public RenderWindow::IListener
			{
			public:
				RenderWindowListener(PhysicalWindow* Window) :
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
					m_Window->UpdateSizeFromRenderWindow();
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
				PhysicalWindow* m_Window;
			};

		public:
			PhysicalWindow(void);
			virtual ~PhysicalWindow(void)
			{
			}

			void RenderAll(EditorRenderDeviceBase* Device) override;

			virtual void SetPosition(const Vector2I& Value) override
			{
				m_RenderWindow.SetPosition(Value);
			}

		protected:
			virtual void OnPositionChanged(void) override;
			virtual void OnSizeChanged(void) override;

			virtual void OnClosing(void) override;
			virtual void OnMaximizeRestore(void) override;
			virtual void OnMinimize(void) override;

		private:
			void UpdateSizeFromRenderWindow(void);
			void UpdateTitleSize(void);

		private:
			RenderWindow m_RenderWindow;
			RenderContext* m_RenderContext;
			RenderWindowListener m_RenderWindowListener;

			bool m_ShouldUpdateSizeFromRenderableWindow;
			bool m_ShouldUpdateSizeFromRenderWindow;
		};
	}
}

#endif