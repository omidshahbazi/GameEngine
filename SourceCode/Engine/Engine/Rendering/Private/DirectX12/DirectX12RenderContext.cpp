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
				DirectX12RenderContext::DirectX12RenderContext(PlatformWindow::WindowHandle WindowHandle, ID3D12DescriptorHeap* DescriptorHeap, IDXGISwapChain4* SwapChain, ID3D12Resource** BackBuffers, uint8 BackBufferCount, ID3D12GraphicsCommandList* CommandList) :
					RenderContext(WindowHandle),
					m_DescriptorHeap(DescriptorHeap),
					m_SwapChain(SwapChain),
					m_BackBufferCount(BackBufferCount),
					m_CommandList(CommandList),
					m_CurrentBackBufferIndex(0)
				{
					PlatformMemory::Copy(BackBuffers, m_BackBuffers, m_BackBufferCount);
				}

				void DirectX12RenderContext::UpdateCurrentBackBufferIndex(void)
				{
					m_CurrentBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
				}
			}
		}
	}
}