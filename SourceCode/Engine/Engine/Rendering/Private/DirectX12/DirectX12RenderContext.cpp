// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\DirectX12\DirectX12RenderContext.h>
#include <Common\BitwiseUtils.h>

namespace Engine
{
	using namespace Common;

	namespace Rendering
	{
		namespace Private
		{
			namespace DirectX12
			{
				DirectX12RenderContext::DirectX12RenderContext(PlatformWindow::WindowHandle WindowHandle, IDXGISwapChain4* SwapChain) :
					RenderContext(WindowHandle),
					m_SwapChain(SwapChain)
				{
				}
			}
		}
	}
}