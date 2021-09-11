// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDER_CONTEXT_H
#define RENDER_CONTEXT_H

namespace Engine
{
	namespace WindowUtility
	{
		class Window;
	}

	using namespace WindowUtility;

	namespace RenderSystem
	{
		class RENDERSYSTEM_API RenderContext
		{
			friend class DeviceInterface;

		private:
			RenderContext(ResourceHandle Handle, Window* Window) :
				m_Handle(Handle),
				m_Window(Window)
			{
			}

		public:
			Window* GetWindow(void) const
			{
				return m_Window;
			}

		private:
			ResourceHandle GetHandle(void) const
			{
				return m_Handle;
			}

		private:
			ResourceHandle m_Handle;
			Window* m_Window;
		};
	}
}

#endif