// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DIRECTX12_RENDER_CONTEXT_H
#define DIRECTX12_RENDER_CONTEXT_H

#include <Rendering\RenderContext.h>
#include <Rendering\Private\DirectX12\DirectX12Device.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace DirectX12
			{
				class RENDERING_API DirectX12RenderContext : public RenderContext
				{
				public:
					DirectX12RenderContext(PlatformWindow::WindowHandle WindowHandle, IDXGISwapChain4* SwapChain);

					IDXGISwapChain4* GetSwapChain(void) const
					{
						return m_SwapChain;
					}

				private:
					IDXGISwapChain4* m_SwapChain;
				};
			}
		}
	}
}

#endif