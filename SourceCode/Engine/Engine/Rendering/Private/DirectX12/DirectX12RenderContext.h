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
					DirectX12RenderContext(PlatformWindow::WindowHandle WindowHandle, ID3D12DescriptorHeap* DescriptorHeap, IDXGISwapChain4* SwapChain, ID3D12GraphicsCommandList* CommandList);

					ID3D12DescriptorHeap* GetDescriptorHeap(void) const
					{
						return m_DescriptorHeap;
					}

					IDXGISwapChain4* GetSwapChain(void) const
					{
						return m_SwapChain;
					}

					ID3D12GraphicsCommandList* GetCommandList(void) const
					{
						return m_CommandList;
					}

					uint32 GetCurrentBackBufferIndex(void) const
					{
						return m_CurrentBackBufferIndex;
					}

					void UpdateCurrentBackBufferIndex(void);

				private:
					ID3D12DescriptorHeap* m_DescriptorHeap;
					IDXGISwapChain4* m_SwapChain;
					ID3D12GraphicsCommandList* m_CommandList;
					uint32 m_CurrentBackBufferIndex;
				};
			}
		}
	}
}

#endif