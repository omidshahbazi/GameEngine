// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Private\FrameConstantBuffers.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <RenderSystem\RenderManager.h>
#include <RenderSystem\Private\ThreadedDevice.h>
#include <RenderSystem\Private\Commands\GPUConstantBuffer.h>

namespace Engine
{
	using namespace RenderCommon::Private;

	namespace RenderSystem
	{
		namespace Private
		{
			const uint16 MAX_BUFFER_COUNT = 2048;

			FrameConstantBuffers::FrameConstantBuffers(void) :
				m_Buffers(nullptr),
				m_InitializedCap(0),
				m_BufferIndex(0)
			{
				m_Buffers = RenderSystemAllocators::ContainersAllocator_AllocateArray<GPUConstantBuffer>(MAX_BUFFER_COUNT);
			}

			FrameConstantBuffers::~FrameConstantBuffers(void)
			{
				RenderSystemAllocators::ContainersAllocator_DeallocateArray(m_Buffers, m_InitializedCap);
			}

			void FrameConstantBuffers::Reset(void)
			{
				m_BufferIndex = 0;
			}

			GPUConstantBuffer* FrameConstantBuffers::Get(uint16 Size)
			{
				static ThreadedDevice* threadedDevice = RenderManager::GetInstance()->GetActiveDevice()->GetThreadedDevice();
				static const byte EMPTY_BUFFER[2048] = {};

				while (m_BufferIndex < m_InitializedCap)
				{
					GPUConstantBuffer* buffer = &m_Buffers[m_BufferIndex++];

					if (buffer->GetSize() < Size)
						continue;

					return buffer;
				}

				if (m_InitializedCap++ == MAX_BUFFER_COUNT)
					return nullptr;

				ResourceHandle handle;
				if (!threadedDevice->CreateBuffer(handle).Wait())
					return nullptr;

				if (!threadedDevice->InitializeConstantBuffer(handle, EMPTY_BUFFER, Size).Wait())
					return nullptr;

				GPUConstantBuffer* buffer = &m_Buffers[m_BufferIndex++];
				ConstructMacro(GPUConstantBuffer, buffer, threadedDevice, Size, handle);

				return buffer;
			}
		}
	}
}