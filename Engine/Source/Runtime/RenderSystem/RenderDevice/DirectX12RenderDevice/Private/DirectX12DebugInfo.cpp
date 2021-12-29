// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifdef DEBUG_MODE
#include <DirectX12RenderDevice\Private\DirectX12DebugInfo.h>

namespace Engine
{
	namespace DirectX12RenderDevice
	{
		namespace Private
		{
			SINGLETON_DEFINITION(DirectX12DebugInfo);

			DirectX12DebugInfo::DirectX12DebugInfo(void) :
				m_InfoQueue(nullptr),
				m_DebugCallback(nullptr)
			{
			}

			bool DirectX12DebugInfo::Initialize(DirectX12Device* Device)
			{
				if (!DirectX12Wrapper::Debugging::GetInfoQueue(Device->m_Device, &m_InfoQueue))
					return false;

				m_DebugCallback = Device->m_DebugCallback;

				return false;
			}

			bool DirectX12DebugInfo::Deinitialize(void)
			{
				return DirectX12Wrapper::DestroyInstance(m_InfoQueue);;
			}

			bool RaiseDebugMessages(DirectX12DebugInfo* Info)
			{
				IDevice::DebugFunction procedure = Info->m_DebugCallback;
				if (procedure == nullptr)
					return true;

				return DirectX12Wrapper::Debugging::IterateOverDebugMessages(Info->m_InfoQueue,
					[procedure](D3D12_MESSAGE_ID ID, D3D12_MESSAGE_CATEGORY Category, cstr Message, D3D12_MESSAGE_SEVERITY Severity)
					{
						IDevice::DebugSources source;
						switch (Category)
						{
						case D3D12_MESSAGE_CATEGORY_APPLICATION_DEFINED:	source = IDevice::DebugSources::Application; break;
						case D3D12_MESSAGE_CATEGORY_MISCELLANEOUS:			source = IDevice::DebugSources::API; break;
						case D3D12_MESSAGE_CATEGORY_INITIALIZATION:			source = IDevice::DebugSources::API; break;
						case D3D12_MESSAGE_CATEGORY_CLEANUP:				source = IDevice::DebugSources::API; break;
						case D3D12_MESSAGE_CATEGORY_COMPILATION:			source = IDevice::DebugSources::API; break;
						case D3D12_MESSAGE_CATEGORY_STATE_CREATION:			source = IDevice::DebugSources::API; break;
						case D3D12_MESSAGE_CATEGORY_STATE_SETTING:			source = IDevice::DebugSources::API; break;
						case D3D12_MESSAGE_CATEGORY_STATE_GETTING:			source = IDevice::DebugSources::API; break;
						case D3D12_MESSAGE_CATEGORY_RESOURCE_MANIPULATION:	source = IDevice::DebugSources::API; break;
						case D3D12_MESSAGE_CATEGORY_EXECUTION:				source = IDevice::DebugSources::API; break;
						case D3D12_MESSAGE_CATEGORY_SHADER:					source = IDevice::DebugSources::ProgramCompiler; break;
						}

						IDevice::DebugSeverities severity;
						switch (Severity)
						{
						case D3D12_MESSAGE_SEVERITY_CORRUPTION:	severity = IDevice::DebugSeverities::High; break;
						case D3D12_MESSAGE_SEVERITY_ERROR:		severity = IDevice::DebugSeverities::High; break;
						case D3D12_MESSAGE_SEVERITY_WARNING:	severity = IDevice::DebugSeverities::Medium; break;
						case D3D12_MESSAGE_SEVERITY_INFO:		severity = IDevice::DebugSeverities::Low; break;
						case D3D12_MESSAGE_SEVERITY_MESSAGE:	severity = IDevice::DebugSeverities::Notification; break;
						}

						procedure(ID, source, Message, IDevice::DebugTypes::Other, severity);
					});
			}
		}
	}
}
#endif