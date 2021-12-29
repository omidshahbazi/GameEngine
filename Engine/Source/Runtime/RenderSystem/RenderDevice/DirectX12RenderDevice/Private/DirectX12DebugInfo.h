// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifdef DEBUG_MODE
#ifndef DIRECTX_12_DEBUG_INFO_H
#define DIRECTX_12_DEBUG_INFO_H

#include <DirectX12RenderDevice\Private\DirectX12Device.h>
#include <MemoryManagement\Singleton.h>

namespace Engine
{
	using namespace RenderDevice;

	namespace DirectX12RenderDevice
	{
		namespace Private
		{
			class DirectX12DebugInfo
			{
				friend bool RaiseDebugMessages(DirectX12DebugInfo* Info);

				SINGLETON_DECLARATION(DirectX12DebugInfo)

			private:
				DirectX12DebugInfo(void);
				~DirectX12DebugInfo(void);

			public:
				bool Initialize(DirectX12Device* Device);
				bool Deinitialize(void);

			private:
				ID3D12InfoQueue* m_InfoQueue;
				IDevice::DebugFunction m_DebugCallback;
			};

			bool RaiseDebugMessages(DirectX12DebugInfo* Info);
		}
	}
}

#endif
#define CHECK_CALL(Expr) (!(!(Expr) && RaiseDebugMessages(DirectX12DebugInfo::GetInstance())))
#else
#define CHECK_CALL(Expr) (Expr)
#endif