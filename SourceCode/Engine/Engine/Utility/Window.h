// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include <Containers\Strings.h>
#include <MathContainers\MathContainers.h>
#include <Containers\ListenerContainer.h>
#include <Platform\PlatformWindow.h>

namespace Engine
{
	using namespace Containers;
	using namespace MathContainers;
	using namespace Platform;

	namespace Utility
	{
		//TODO: Managing Taskbar Visibility
		//https://docs.microsoft.com/en-us/previous-versions//bb776822(v=vs.85)?redirectedfrom=MSDN
		//https://stackoverflow.com/questions/7219063/win32-how-to-hide-3rd-party-windows-in-taskbar-by-hwnd
		class UTILITY_API Window
		{
		public:
			class UTILITY_API IListener
			{
			public:
				virtual void OnWindowMoved(Window* Window) = 0;
				virtual void OnWindowResized(Window* Window) = 0;

				virtual void OnKeyDown(PlatformWindow::VirtualKeys Key) = 0;
				virtual void OnKeyUp(PlatformWindow::VirtualKeys Key) = 0;
				virtual void OnKeyPressed(PlatformWindow::VirtualKeys Key) = 0;

				virtual void OnMouseDown(PlatformWindow::VirtualKeys Key, Vector2I Position) = 0;
				virtual void OnMouseUp(PlatformWindow::VirtualKeys Key, Vector2I Position) = 0;
				virtual void OnMouseClick(PlatformWindow::VirtualKeys Key, Vector2I Position) = 0;
				virtual void OnMouseWheel(Vector2I Position, uint16 Delta) = 0;

				virtual void OnMouseMove(Vector2I Position) = 0;
				virtual void OnMouseLeave(void) = 0;

				virtual void OnClosing(void) = 0;
			};

			enum class States
			{
				Noraml = 0,
				Minimized,
				Maximized
			};

			enum class BorderStyles
			{
				Normal = 0,
				Tool
			};

			enum class SizableModes
			{
				Sizable = 0,
				Fixed
			};

			LISTENER_DECLARATION(IListener)

		public:
			Window(const String& Name);
			~Window(void);

		public:
			bool Initialize(void);

			INLINE const String& GetName(void) const
			{
				return m_Name;
			}

			INLINE const String& GetTitle(void) const
			{
				return m_Title;
			}
			void SetTitle(const String& Value);

			INLINE const Vector2I& GetPosition(void) const
			{
				return m_Position;
			}

			void SetPosition(const Vector2I& Value);

			INLINE const Vector2I& GetMinimumSize(void) const
			{
				return m_MinimumSize;
			}
			INLINE void SetMinimumSize(const Vector2I& Value)
			{
				m_MinimumSize = Value;
			}

			INLINE const Vector2I& GetMaximumSize(void) const
			{
				return m_MaximumSize;
			}
			INLINE void SetMaximumSize(const Vector2I& Value)
			{
				m_MaximumSize = Value;
			}

			INLINE const Vector2I& GetSize(void) const
			{
				return m_Size;
			}
			void SetSize(const Vector2I& Value);

			bool GetIsVisible(void) const;
			void SetIsVisible(bool Value);

			bool GetIsDisabled(void) const;
			void SetIsDisabled(bool Value);

			bool GetShowCaption(void) const;
			void SetShowCaption(bool Value);

			bool GetShowSystemMenu(void) const;
			void SetShowSystemMenu(bool Value);

			bool GetShowBorder(void) const;
			void SetShowBorder(bool Value);

			bool GetIsTabStop(void) const;
			void SetIsTabStop(bool Value);

			bool GetShowMinimizeBox(void) const;
			void SetShowMinimizeBox(bool Value);

			bool GetShowMaximizeBox(void) const;
			void SetShowMaximizeBox(bool Value);

			bool GetIsTopMost(void) const;
			void SetIsTopMost(bool Value);

			bool GetAcceptFiles(void) const;
			void SetAcceptFiles(bool Value);

			INLINE States GetState(void) const
			{
				return m_State;
			}
			void SetState(States Value);

			INLINE BorderStyles GetBorderStyle(void) const
			{
				return m_BorderStyle;
			}
			void SetBorderStyle(BorderStyles Value);

			bool GetShowFrame(void) const;
			void SetShowFrame(bool Value);

			INLINE SizableModes GetSizableMode(void) const
			{
				return m_SizableMode;
			}
			void SetSizableMode(SizableModes Value);

			INLINE const Vector2I& GetClientPosition(void) const
			{
				return m_ClientPosition;
			}

			INLINE const Vector2I& GetClientSize(void) const
			{
				return m_ClientSize;
			}

			INLINE bool ShouldClose(void) const
			{
				return m_ShouldClose;
			}

			INLINE PlatformWindow::WindowHandle GetHandle(void) const
			{
				return m_Handle;
			}

		private:
			void UpdateSize(bool Force);

			bool MessageProcedure(PlatformWindow::WindowMessages Message, void* Parameter);

		private:
			PlatformWindow::WindowHandle m_Handle;

			String m_Name;
			String m_Title;
			Vector2I m_Position;

			Vector2I m_MinimumSize;
			Vector2I m_MaximumSize;
			Vector2I m_Size;

			States m_State;
			BorderStyles m_BorderStyle;
			SizableModes m_SizableMode;

			Vector2I m_ClientPosition;
			Vector2I m_ClientSize;

			bool m_ShouldClose;
		};
	}
}

#endif