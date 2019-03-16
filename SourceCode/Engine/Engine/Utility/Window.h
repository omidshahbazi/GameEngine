// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include <Containers\Strings.h>
#include <Containers\MathContainers.h>
#include <Platform\PlatformWindow.h>

namespace Engine
{
	using namespace Containers;
	using namespace Platform;

	namespace Utility
	{
		class UTILITY_API Window
		{
		public:
			class UTILITY_API IListener
			{
			public:
				virtual void OnWindowResized(Window *Window) = 0;
			};

		private:
			typedef Vector<IListener*> ListernerList;

		public:
			Window(const String &Name);
			~Window(void);

		public:
			bool Initialize(void);

			void SetTitle(const String &Value);

			const Vector2I &GetSize(void) const
			{
				return m_Size;
			}

			void SetSize(const Vector2I &Value);

			void AddListener(IListener *Listener)
			{
				if (m_Listeners.Contains(Listener))
					return;

				m_Listeners.Add(Listener);
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
			bool MessageProcedure(PlatformWindow::WindowMessages Message);

		private:
			PlatformWindow::WindowHandle m_Handle;
			String m_Name;
			String m_Title;
			Vector2I m_Size;
			bool m_ShouldClose;

			ListernerList m_Listeners;
		};
	}
}

#endif