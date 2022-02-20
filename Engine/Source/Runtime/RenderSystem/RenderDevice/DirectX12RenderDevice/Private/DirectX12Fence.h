// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DIRECTX_12_FENCE_H
#define DIRECTX_12_FENCE_H

#include <RenderDevice\IFence.h>
#include <DirectX12RenderDevice\Private\DirectX12Wrapper.h>
#include <Platform\PlatformOS.h>

namespace Engine
{
	using namespace RenderDevice;
	using namespace Platform;

	namespace DirectX12RenderDevice
	{
		namespace Private
		{
			class DirectX12Device;

			class DirectX12Fence : public IFence
			{
			public:
				DirectX12Fence(DirectX12Device* Device);
				~DirectX12Fence(void);

				bool GetIsPassed(void) override;

				bool Wait(void) override;

				bool Wait(ID3D12CommandQueue* Queue);

				bool Signal(ID3D12CommandQueue* Queue);

			private:
				ID3D12Device5* m_NativeDevice;

				ID3D12Fence* m_Fence;
				uint64 m_Value;
				PlatformOS::Handle m_Event;
			};
		}
	}
}

#endif