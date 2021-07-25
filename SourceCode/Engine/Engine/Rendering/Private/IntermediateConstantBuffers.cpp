// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\IntermediateConstantBuffers.h>
#include <Rendering\Private\RenderingAllocators.h>
#include <Rendering\Private\ThreadedDevice.h>
#include <Rendering\ConstantBuffer.h>
#include <Rendering\RenderingManager.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			const uint16 MAX_BUFFER_COUNT = 2048;

			IntermediateConstantBuffers::IntermediateConstantBuffers(void) :
				m_Buffers(nullptr),
				m_InitializedCap(0),
				m_BufferIndex(0)
			{
				m_Buffers = RenderingAllocators::ContainersAllocator_AllocateArray<ConstantBuffer>(MAX_BUFFER_COUNT);
			}

			IntermediateConstantBuffers::~IntermediateConstantBuffers(void)
			{
				RenderingAllocators::ContainersAllocator_Deallocate(m_Buffers);
			}

			void IntermediateConstantBuffers::Reset(void)
			{
				m_BufferIndex = 0;
			}

			ConstantBuffer* IntermediateConstantBuffers::Get(uint16 Size)
			{
				static ThreadedDevice* threadedDevice = RenderingManager::GetInstance()->GetActiveDevice()->GetThreadedDevice();
				static const byte EMPTY_BUFFER[2048] = {};

				while (m_BufferIndex < m_InitializedCap)
				{
					ConstantBuffer* buffer = &m_Buffers[m_BufferIndex++];

					if (buffer->GetSize() < Size)
						continue;

					return buffer;
				}

				if (m_InitializedCap++ == MAX_BUFFER_COUNT)
					return nullptr;


				GPUBuffer::Handle handle;
				if (!threadedDevice->CreateBuffer(handle).Wait())
					return nullptr;

				if (!threadedDevice->InitializeConstantBuffer(handle, EMPTY_BUFFER, Size).Wait())
					return nullptr;

				ConstantBuffer* buffer = &m_Buffers[m_BufferIndex++];
				ConstructMacro(ConstantBuffer, buffer, threadedDevice, Size, handle);

				return buffer;
			}
		}
	}
}