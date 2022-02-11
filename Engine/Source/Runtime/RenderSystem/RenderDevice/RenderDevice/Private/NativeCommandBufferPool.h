// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef NATIVE_COMMAND_BUFFER_POOL_H
#define NATIVE_COMMAND_BUFFER_POOL_H

#include <RenderDevice\IDevice.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <Containers\Map.h>
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
				typedef Map<ICommandBuffer::Types, CommandStack> CommandMap;

			public:
				NativeCommandBufferPool(void) :
					m_Commands(RenderSystemAllocators::ContainersAllocator)
				{
				}

				~NativeCommandBufferPool(void)
				{
					for (auto& pair : m_Commands)
					{
						for (auto& command : pair.GetSecond())
							RenderSystemAllocators::ResourceAllocator_Deallocate(command);

						pair.GetSecond().Clear();
					}

					m_Commands.Clear();
				}

				void InitializeType(ICommandBuffer::Types Type)
				{
					if (m_Commands.Contains(Type))
						return;

					m_Commands[Type] = CommandStack(RenderSystemAllocators::ContainersAllocator);
				}

				template<typename DeviceType>
				NativeCommandBufferType* Get(DeviceType* Device, ICommandBuffer::Types Type)
				{
					CoreDebugAssert(Categories::RenderSystem, m_Commands.Contains(Type), "Command pool for this type didn't initialized");

					CommandStack& stack = m_Commands[Type];

					NativeCommandBufferType* commandBuffer = nullptr;

					if (stack.GetSize() == 0)
					{
						commandBuffer = RenderSystemAllocators::ResourceAllocator_Allocate<NativeCommandBufferType>();
						Construct(commandBuffer, Device, Type);
					}
					else
					{
						stack.Pop(&commandBuffer);
						commandBuffer->Clear();
					}

					commandBuffer->SetName(L"Command Buffer");

					return commandBuffer;
				}

				void Back(ICommandBuffer* NativeCommandBuffer)
				{
					CoreDebugAssert(Categories::RenderSystem, NativeCommandBuffer != nullptr, "NativeCommandBuffer cannot be null");

					ICommandBuffer::Types type = NativeCommandBuffer->GetType();

					CoreDebugAssert(Categories::RenderSystem, m_Commands.Contains(type), "Command pool for this type didn't initialized");

					m_Commands[type].Push(ReinterpretCast(NativeCommandBufferType*, NativeCommandBuffer));
				}

			private:
				CommandMap m_Commands;
			};
		}
	}
}

#endif