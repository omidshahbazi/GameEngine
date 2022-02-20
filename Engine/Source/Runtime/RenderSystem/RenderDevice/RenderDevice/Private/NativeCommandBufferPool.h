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
			template<typename DeviceType, typename CommandBufferType>
			class NativeCommandBufferPool
			{
			private:
				typedef Stack<CommandBufferType*> CommandStack;

			public:
				NativeCommandBufferPool(DeviceType* Device) :
					m_Device(Device),
					m_Commands(RenderSystemAllocators::ContainersAllocator)
				{
					CoreDebugAssert(Categories::RenderSystem, Device != nullptr, "Device cannot be null");
				}

				~NativeCommandBufferPool(void)
				{
					for (auto& command : m_Commands)
						RenderSystemAllocators::ResourceAllocator_Deallocate(command);

					m_Commands.Clear();
				}

				CommandBufferType* Get(void)
				{
					CommandBufferType* commandBuffer = nullptr;

					if (m_Commands.GetSize() == 0)
					{
						commandBuffer = RenderSystemAllocators::ResourceAllocator_Allocate<CommandBufferType>();
						Construct(commandBuffer, m_Device);
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

					m_Commands.Push(ReinterpretCast(CommandBufferType*, NativeCommandBuffer));
				}

			private:
				DeviceType* m_Device;
				CommandStack m_Commands;
			};
		}
	}
}

#endif