// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include <Containers\Strings.h>
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
			Window(const String &Name);
			~Window(void);

		public:
			bool Initialize(void);

			void SetTitle(const String &Title);
			void SetSize(uint16 Width, uint16 Height);

			bool ShouldClose(void) const;

			INLINE PlatformWindow::WindowHandle GetHandle(void) const
			{
				return m_Handle;
			}

		private:
			int32 MessageProcedure(PlatformWindow::WindowHandle Handle, uint32 Message, uint32 *WParam, uint32 *LParam);

		private:
			PlatformWindow::WindowHandle m_Handle;
			String m_Name;
			String m_Title;
			uint16 m_Width;
			uint16 m_Height;
			bool m_ShouldClose;
		};
	}
}

#endif