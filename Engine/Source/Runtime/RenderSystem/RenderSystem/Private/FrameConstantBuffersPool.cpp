// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Private\FrameConstantBuffersPool.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>

namespace Engine
{
	using namespace RenderCommon::Private;

	namespace RenderSystem
	{
		namespace Private
		{
			FrameConstantBuffersPool::FrameConstantBuffersPool(ThreadedDevice* Device) :
				m_Device(Device),
				m_Buffers(RenderSystemAllocators::ContainersAllocator)
			{
			}

			FrameConstantBuffers* FrameConstantBuffersPool::Get(void)
			{
				m_Lock.Lock();

				FrameConstantBuffers* buffers = nullptr;
				if (m_Buffers.GetSize() == 0)
				{
					buffers = RenderSystemAllocators::ContainersAllocator_Allocate<FrameConstantBuffers>();
					Construct(buffers, m_Device);
				}
				else
				{
					m_Buffers.Pop(&buffers);

					buffers->Reset();
				}

				m_Lock.Release();

				return buffers;
			}

			void FrameConstantBuffersPool::Back(FrameConstantBuffers* Buffers)
			{
				m_Lock.Lock();

				m_Buffers.Push(Buffers);

				m_Lock.Release();
			}
		}
	}
}