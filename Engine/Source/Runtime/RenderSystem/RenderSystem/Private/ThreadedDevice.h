// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef THREADED_DEVICE_H
#define THREADED_DEVICE_H

#include <RenderDevice\IDevice.h>
#include <RenderCommon\RenderCommon.h>
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
			class FrameConstantBuffers;

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

				Promise<bool> CreateBuffer(GPUBufferTypes Type, uint32 Size, ResourceHandle& Handle);
				Promise<bool> DestroyBuffer(ResourceHandle Handle);
				Promise<bool> LockBuffer(ResourceHandle Handle, GPUBufferAccess Access, byte** Buffer);
				Promise<bool> UnlockBuffer(ResourceHandle Handle);

				Promise<bool> CreateProgram(const IDevice::CompiledShaders* Shaders, ResourceHandle& Handle, cstr* ErrorMessage);
				Promise<bool> DestroyProgram(ResourceHandle Handle);

				Promise<bool> CreateTexture(const TextureInfo* Info, ResourceHandle& Handle);
				Promise<bool> DestroyTexture(ResourceHandle Handle);
				Promise<bool> SetTextureVerticalWrapping(ResourceHandle Handle, TextureWrapModes Mode);
				Promise<bool> SetTextureHorizontalWrapping(ResourceHandle Handle, TextureWrapModes Mode);
				Promise<bool> SetTextureMinifyFilter(ResourceHandle Handle, TextureMinifyFilters Filter);
				Promise<bool> SetTextureMagnifyFilter(ResourceHandle Handle, TextureMagnfyFilters Filter);

				Promise<bool> CreateRenderTarget(const RenderTargetInfo* Info, ResourceHandle& Handle, IDevice::TextureList& Textures);
				Promise<bool> DestroyRenderTarget(ResourceHandle Handle);

				Promise<bool> CreateMesh(const SubMeshInfo* Info, ResourceHandle& Handle);
				Promise<bool> DestroyMesh(ResourceHandle Handle);

				Promise<bool> CreateCommandBuffer(ICommandBuffer*& Buffer);
				Promise<bool> DestroyCommandBuffer(ICommandBuffer** Buffers, uint16 Count);
				Promise<bool> SubmitCommandBuffer(ICommandBuffer* const* Buffers, uint16 Count);
				Promise<bool> SubmitCommandBufferAsync(ICommandBuffer* const* Buffers, uint16 Count);

				Promise<bool> SyncConstantBuffers(const FrameConstantBuffers* Buffers);

				Promise<bool> SwapBuffers(void);

				Promise<bool> SetResourceName(ResourceHandle Handle, IDevice::ResourceTypes Type, cwstr Name);
				Promise<bool> SetDebugCallback(IDevice::DebugFunction Callback);

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
			};
		}
	}
}

#endif