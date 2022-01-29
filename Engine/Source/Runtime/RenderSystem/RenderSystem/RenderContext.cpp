// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\RenderContext.h>
#include <RenderSystem\Private\FrameDataChain.h>

namespace Engine
{
	namespace RenderSystem
	{
		RenderContext::RenderContext(ResourceHandle Handle, Window* Window) :
			m_Handle(Handle),
			m_Window(Window)
		{
		}
	}
}