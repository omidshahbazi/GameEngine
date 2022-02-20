// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <DirectX12RenderDevice\Private\DirectX12Fence.h>
#include <DirectX12RenderDevice\Private\DirectX12DebugInfo.h>

namespace Engine
{
	namespace DirectX12RenderDevice
	{
		namespace Private
		{
			DirectX12Fence::DirectX12Fence(DirectX12Device* Device) :
				m_NativeDevice(Device->GetDevice()),
				m_Fence(nullptr),
				m_Value(0),
				m_Event(0)
			{
				if (!CHECK_CALL(DirectX12Wrapper::Fence::Create(m_NativeDevice, &m_Fence)))
					return;

				m_Event = PlatformOS::CreateSignalEvent();
			}

			DirectX12Fence::~DirectX12Fence(void)
			{
				PlatformOS::DestroySignalEvent(m_Event);

				CHECK_CALL(DirectX12Wrapper::DestroyInstance(m_Fence));
			}

			bool DirectX12Fence::GetIsPassed(void)
			{
				return (m_Fence->GetCompletedValue() >= m_Value);
			}

			bool DirectX12Fence::Wait(void)
			{
				if (GetIsPassed())
					return true;

				if (!SUCCEEDED(m_Fence->SetEventOnCompletion(m_Value, m_Event)))
					return false;

				PlatformOS::WaitForSignalEvent(m_Event);

				return true;
			}

			bool DirectX12Fence::Wait(ID3D12CommandQueue* Queue)
			{
				return CHECK_CALL(DirectX12Wrapper::Fence::Wait(Queue, m_Fence, m_Value));
			}

			bool DirectX12Fence::Signal(ID3D12CommandQueue* Queue)
			{
				return CHECK_CALL(DirectX12Wrapper::Fence::Signal(Queue, m_Fence, ++m_Value));
			}
		}
	}
}