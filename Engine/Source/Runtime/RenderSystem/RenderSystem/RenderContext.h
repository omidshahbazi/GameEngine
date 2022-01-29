// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDER_CONTEXT_H
#define RENDER_CONTEXT_H

#include <RenderCommon\RenderCommon.h>

namespace Engine
{
	namespace WindowUtility
	{
		class Window;
	}

	using namespace RenderCommon;
	using namespace WindowUtility;

	namespace RenderSystem
	{
		class RENDERSYSTEM_API RenderContext
		{
			friend class DeviceInterface;

		private:
			RenderContext(ResourceHandle Handle, Window* Window);

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