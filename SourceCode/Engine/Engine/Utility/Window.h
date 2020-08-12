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
				virtual void OnPositionChanged(Window* Window) = 0;
				virtual void OnSizeChanged(Window* Window) = 0;

				virtual void OnKeyDown(Window* Window, PlatformWindow::VirtualKeys Key) = 0;
				virtual void OnKeyUp(Window* Window, PlatformWindow::VirtualKeys Key) = 0;
				virtual void OnKeyPressed(Window* Window, PlatformWindow::VirtualKeys Key) = 0;

				virtual void OnMouseDown(Window* Window, PlatformWindow::VirtualKeys Key, const Vector2I& Position) = 0;
				virtual void OnMouseUp(Window* Window, PlatformWindow::VirtualKeys Key, const Vector2I& Position) = 0;
				virtual void OnMouseClick(Window* Window, PlatformWindow::VirtualKeys Key, const Vector2I& Position) = 0;
				virtual void OnMouseWheel(Window* Window, const Vector2I& Position, uint16 Delta) = 0;

				virtual void OnMouseMove(Window* Window, const Vector2I& Position) = 0;
				virtual void OnMouseLeave(Window* Window) = 0;

				virtual void OnClosing(Window* Window) = 0;
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

			INLINE bool GetIsFocused(void) const
			{
				return m_IsFocused;
			}

			INLINE const Vector2I& GetPosition(void) const
			{
				return m_Position;
			}

			INLINE const Vector2I& GetClientPosition(void) const
			{
				return m_ClientPosition;
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

			INLINE const Vector2I& GetClientSize(void) const
			{
				return m_ClientSize;
			}

			bool GetIsFixedSize(void) const
			{
				return m_IsFixedSize;
			}
			void SetIsFixedSize(bool Value)
			{
				m_IsFixedSize = Value;
			}

			bool GetSystemMenuEnabled(void) const
			{
				return m_SystemMenuEnabled;
			}
			void SetSystemMenuEnabled(bool Value)
			{
				m_SystemMenuEnabled = Value;
			}

			float GetSystemMenuWidth(void) const
			{
				return m_SystemMenuWidth;
			}
			void SetSystemMenuWidth(float Value)
			{
				m_SystemMenuWidth = Value;
			}

			bool GetTitleBarEnabled(void) const
			{
				return m_TitleBarEnabled;
			}
			void SetTitleBarEnabled(bool Value)
			{
				m_TitleBarEnabled = Value;
			}

			const Vector2I& GetTitleBarSize(void) const
			{
				return m_TitleBarSize;
			}
			void SetTitleBarSize(const Vector2I& Value)
			{
				m_TitleBarSize = Value;
			}

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

			INLINE bool GetShowFrame(void) const
			{
				return m_ShowFrame;
			}
			void SetShowFrame(bool Value);

			INLINE bool ShouldClose(void) const
			{
				return m_ShouldClose;
			}
			void Close(void) const;

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

			bool m_IsFocused;

			Vector2I m_Position;
			Vector2I m_ClientPosition;

			Vector2I m_MinimumSize;
			Vector2I m_MaximumSize;
			Vector2I m_Size;
			Vector2I m_ClientSize;

			bool m_IsFixedSize;
			bool m_SystemMenuEnabled;
			float m_SystemMenuWidth;
			bool m_TitleBarEnabled;
			Vector2I m_TitleBarSize;

			bool m_ShowFrame;

			States m_State;
			BorderStyles m_BorderStyle;

			bool m_ShouldClose;
		};
	}
}

#endif