// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef THREADED_DEVICE_H
#define THREADED_DEVICE_H

#include <RenderDevice\IDevice.h>
#include <RenderCommon\RenderCommon.h>
#include <RenderSystem\Private\FrameDataChain.h>
#include <Common\SpinLock.h>
#include <Containers\Queue.h>
#include <Containers\Strings.h>
#include <Containers\Promise.h>
#include <Containers\Map.h>
#include <Threading\Thread.h>
#include <memory>

namespace Engine
{
	using namespace Containers;
	using namespace Threading;
	using namespace RenderDevice;

	namespace RenderSystem
	{
		class GPUBuffer;

		namespace Private
		{
			class ThreadedDevice
			{
				friend class GPUBuffer;

			private:
				typedef std::function<void(bool ForceQuit)> Task;
				typedef std::shared_ptr<Task> TaskPtr;
				typedef Queue<TaskPtr> TaskQueue;

			public:
				ThreadedDevice(IDevice* Device, DeviceTypes DeviceType);
				~ThreadedDevice(void);

				Promise<bool> Initialize(void);

				Promise<cstr> GetVersion(void);
				Promise<cstr> GetVendorName(void);
				Promise<cstr> GetRendererName(void);
				Promise<cstr> GetShadingLanguageVersion(void);

				Promise<bool> CreateContext(PlatformWindow::WindowHandle WindowHandle, ResourceHandle& Handle);
				Promise<bool> DestroyContext(ResourceHandle Handle);
				Promise<bool> SetContext(ResourceHandle Handle);

				Promise<bool> CreateBuffer(ResourceHandle& Handle);
				Promise<bool> DestroyBuffer(ResourceHandle Handle);
				Promise<bool> LockBuffer(ResourceHandle Handle, GPUBufferTypes Type, GPUBufferAccess Access, byte** Buffer);
				Promise<bool> UnlockBuffer(ResourceHandle Handle, GPUBufferTypes Type);
				Promise<bool> InitializeConstantBuffer(ResourceHandle Handle, const byte* Data, uint32 Size);
				Promise<bool> CopyFromVertexToBuffer(ResourceHandle Handle, ResourceHandle FromMeshHandle, uint32 Size);
				Promise<bool> CopyFromBufferToVertex(ResourceHandle Handle, ResourceHandle ToMeshHandle, uint32 Size);
				Promise<bool> CopyFromIndexToBuffer(ResourceHandle Handle, ResourceHandle FromMeshHandle, uint32 Size);
				Promise<bool> CopyFromBufferToIndex(ResourceHandle Handle, ResourceHandle ToMeshHandle, uint32 Size);
				Promise<bool> CopyFromTextureToBuffer(ResourceHandle Handle, ResourceHandle FromTextureHandle, uint32 Size, TextureTypes TextureType, Formats TextureFormat, uint32 Level);
				Promise<bool> CopyFromBufferToTexture(ResourceHandle Handle, ResourceHandle ToTextureHandle, TextureTypes TextureType, uint32 Width, uint32 Height, Formats TextureFormat);

				Promise<bool> CreateProgram(const IDevice::CompiledShaders* Shaders, ResourceHandle& Handle, cstr* ErrorMessage);
				Promise<bool> DestroyProgram(ResourceHandle Handle);

				Promise<bool> CreateTexture(const TextureInfo* Info, ResourceHandle& Handle);
				Promise<bool> DestroyTexture(ResourceHandle Handle);
				Promise<bool> SetTextureVerticalWrapping(ResourceHandle Handle, TextureTypes Type, TextureWrapModes Mode);
				Promise<bool> SetTextureHorizontalWrapping(ResourceHandle Handle, TextureTypes Type, TextureWrapModes Mode);
				Promise<bool> SetTextureMinifyFilter(ResourceHandle Handle, TextureTypes Type, TextureMinifyFilters Filter);
				Promise<bool> SetTextureMagnifyFilter(ResourceHandle Handle, TextureTypes Type, TextureMagnfyFilters Filter);

				Promise<bool> CreateRenderTarget(const RenderTargetInfo* Info, ResourceHandle& Handle, IDevice::TextureList& Textures);
				Promise<bool> DestroyRenderTarget(ResourceHandle Handle);

				Promise<bool> CreateMesh(const SubMeshInfo* Info, ResourceHandle& Handle);
				Promise<bool> DestroyMesh(ResourceHandle Handle);

				Promise<bool> CreateCommandBuffer(ICommandBuffer::Types Type, ICommandBuffer*& Buffer);
				Promise<bool> DestroyCommandBuffer(ICommandBuffer* Buffer);
				Promise<bool> SubmitCommandBuffer(ICommandBuffer* const* Buffers, uint16 Count);
				Promise<bool> SubmitCommandBufferAsync(ICommandBuffer* const* Buffers, uint16 Count);

				Promise<bool> SetResourceName(ResourceHandle Handle, IDevice::ResourceTypes Type, cwstr Name);
				Promise<bool> SetDebugCallback(IDevice::DebugFunction Callback);

				FrameDataChain* GetFrameDataChain(void)
				{
					return m_FrameDataChain;
				}

			private:
				void Worker(void);

				IDevice* GetDevice(void) const
				{
					return m_Device;
				}

			private:
				Thread m_Thread;
				TaskQueue m_Tasks;
				SpinLock m_TasksLock;
				IDevice* m_Device;
				bool m_IsInitialized;
				DeviceTypes m_DeviceType;
				FrameDataChain* m_FrameDataChain;
			};
		}
	}
}

#endif