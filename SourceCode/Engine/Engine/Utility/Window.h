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
		class UTILITY_API Window
		{
		public:
			class UTILITY_API IListener
			{
			public:
				virtual void OnWindowPositioned(Window* Window) = 0;

				virtual void OnWindowResized(Window* Window) = 0;
			};

			enum class States
			{
				Noraml = 0,
				Minimized,
				Maximized
			};

			enum class BorderStyles
			{
				None = 0,
				Normal,
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

			const String& GetName(void) const
			{
				return m_Name;
			}

			const String& GetTitle(void) const
			{
				return m_Title;
			}
			void SetTitle(const String& Value);

			const Vector2I& GetPosition(void) const
			{
				return m_Position;
			}

			void SetPosition(const Vector2I& Value);

			const Vector2I& GetMinimumSize(void) const
			{
				return m_MinimumSize;
			}
			void SetMinimumSize(const Vector2I& Value)
			{
				m_MinimumSize = Value;
			}

			const Vector2I& GetMaximumSize(void) const
			{
				return m_MaximumSize;
			}
			void SetMaximumSize(const Vector2I& Value)
			{
				m_MaximumSize = Value;
			}

			const Vector2I& GetSize(void) const
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

			bool GetSystemMenu(void) const;
			void SetSystemMenu(bool Value);

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

			States GetState(void) const
			{
				return m_State;
			}
			void SetState(States Value);

			BorderStyles GetBorderStyle(void) const
			{
				return m_BorderStyle;
			}
			void SetBorderStyle(BorderStyles Value);

			SizableModes GetSizableMode(void) const
			{
				return m_SizableMode;
			}
			void SetSizableMode(SizableModes Value);

			const Vector2I& GetClientPosition(void) const
			{
				return m_ClientPosition;
			}

			const Vector2I& GetClientSize(void) const
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
			bool MessageProcedure(PlatformWindow::WindowMessages Message, void* Parameter);

			void UpdateStyle(void);

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