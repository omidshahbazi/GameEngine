// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef NATIVE_FENCE_POOL_H
#define NATIVE_FENCE_POOL_H

#include <RenderDevice\IDevice.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <Containers\Stack.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace Containers;
	using namespace Debugging;
	using namespace RenderCommon::Private;

	namespace RenderDevice
	{
		namespace Private
		{
			template<typename DeviceType, typename FenceType>
			class NativeFencePool
			{
			private:
				typedef Stack<FenceType*> FenceStack;

			public:
				NativeFencePool(DeviceType* Device) :
					m_Device(Device),
					m_Fences(RenderSystemAllocators::ContainersAllocator)
				{
					CoreDebugAssert(Categories::RenderSystem, Device != nullptr, "Device cannot be null");
				}

				~NativeFencePool(void)
				{
					for (auto& fence : m_Fences)
						RenderSystemAllocators::ResourceAllocator_Deallocate(fence);

					m_Fences.Clear();
				}

				FenceType* Get(void)
				{
					FenceType* fence = nullptr;

					if (m_Fences.GetSize() == 0)
					{
						fence = RenderSystemAllocators::ResourceAllocator_Allocate<FenceType>();
						Construct(fence, m_Device);
					}
					else
						m_Fences.Pop(&fence);

					return fence;
				}

				void Back(IFence* Fence)
				{
					CoreDebugAssert(Categories::RenderSystem, Fence != nullptr, "NativeFence cannot be null");

					m_Fences.Push(ReinterpretCast(FenceType*, Fence));
				}

			private:
				DeviceType* m_Device;
				FenceStack m_Fences;
			};
		}
	}
}

#endif