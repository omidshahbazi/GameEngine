// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef NATIVE_COMMAND_BUFFER_POOL_H
#define NATIVE_COMMAND_BUFFER_POOL_H

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
			template<typename NativeCommandBufferType>
			class NativeCommandBufferPool
			{
			private:
				typedef Stack<NativeCommandBufferType*> CommandStack;

			public:
				NativeCommandBufferPool(void) :
					m_Commands(RenderSystemAllocators::ContainersAllocator)
				{
				}

				~NativeCommandBufferPool(void)
				{
					for (auto& command : m_Commands)
						RenderSystemAllocators::ResourceAllocator_Deallocate(command);

					m_Commands.Clear();
				}

				template<typename DeviceType>
				NativeCommandBufferType* Get(DeviceType* Device)
				{
					CoreDebugAssert(Categories::RenderSystem, Device != nullptr, "Device cannot be null");

					NativeCommandBufferType* commandBuffer = nullptr;

					if (m_Commands.GetSize() == 0)
					{
						commandBuffer = RenderSystemAllocators::ResourceAllocator_Allocate<NativeCommandBufferType>();
						Construct(commandBuffer, Device);
					}
					else
						m_Commands.Pop(&commandBuffer);

					commandBuffer->SetName(L"Command Buffer");
					commandBuffer->Clear();

					return commandBuffer;
				}

				void Back(ICommandBuffer* NativeCommandBuffer)
				{
					CoreDebugAssert(Categories::RenderSystem, NativeCommandBuffer != nullptr, "NativeCommandBuffer cannot be null");

					m_Commands.Push(ReinterpretCast(NativeCommandBufferType*, NativeCommandBuffer));
				}

			private:
				CommandStack m_Commands;
			};
		}
	}
}

#endif