// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef THREADED_DEVICE_H
#define THREADED_DEVICE_H

#include <Rendering\IDevice.h>
#include <Rendering\RenderingCommon.h>
#include <Rendering\Private\CommandsHolder.h>
#include <Common\SpinLock.h>
#include <Containers\Queue.h>
#include <Containers\Strings.h>
#include <Containers\Promise.h>
#include <Containers\Map.h>
#include <Utility\Window.h>
#include <Threading\Thread.h>
#include <memory>

namespace Engine
{
	using namespace Containers;
	using namespace Utility;
	using namespace Threading;

	namespace Rendering
	{
		class GPUBuffer;

		namespace Private
		{
			class RENDERING_API ThreadedDevice
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

				Promise<bool> CreateContext(PlatformWindow::WindowHandle WindowHandle, RenderContext::Handle& Handle);
				Promise<bool> DestroyContext(RenderContext::Handle Handle);
				Promise<bool> SetContext(RenderContext::Handle Handle);
				Promise<bool> SetContextSize(const Vector2I& Size);

				Promise<bool> ResetState(void);
				Promise<IDevice::State> GetState(void);
				Promise<void> SetState(const IDevice::State& State);

				Promise<bool> SetResourceName(NativeType::Handle Handle, IDevice::ResourceTypes Type, cwstr Name);

				Promise<bool> CreateBuffer(GPUBuffer::Handle& Handle);
				Promise<bool> DestroyBuffer(GPUBuffer::Handle Handle);
				Promise<bool> InitializeConstantBuffer(GPUBuffer::Handle Handle, const byte* Data, uint32 Size);
				Promise<bool> CopyFromVertexToBuffer(GPUBuffer::Handle Handle, SubMesh::Handle FromMeshHandle, uint32 Size);
				Promise<bool> CopyFromBufferToVertex(GPUBuffer::Handle Handle, SubMesh::Handle ToMeshHandle, uint32 Size);
				Promise<bool> CopyFromIndexToBuffer(GPUBuffer::Handle Handle, SubMesh::Handle FromMeshHandle, uint32 Size);
				Promise<bool> CopyFromBufferToIndex(GPUBuffer::Handle Handle, SubMesh::Handle ToMeshHandle, uint32 Size);
				Promise<bool> CopyFromTextureToBuffer(GPUBuffer::Handle Handle, Texture::Handle FromTextureHandle, uint32 Size, Texture::Types TextureType, Formats TextureFormat, uint32 Level);
				Promise<bool> CopyFromBufferToTexture(GPUBuffer::Handle Handle, Texture::Handle ToTextureHandle, Texture::Types TextureType, uint32 Width, uint32 Height, Formats TextureFormat);
				Promise<bool> LockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Access Access, byte** Buffer);
				Promise<bool> UnlockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type);

				Promise<bool> CompileProgram(const IDevice::Shaders* Shaders, IDevice::CompiledShaders* CompiledShaders, cstr* ErrorMessage);
				Promise<bool> CreateProgram(const IDevice::CompiledShaders* Shaders, Program::Handle& Handle, cstr* ErrorMessage);
				Promise<bool> DestroyProgram(Program::Handle Handle);

				Promise<bool> CreateTexture(const TextureInfo* Info, Texture::Handle& Handle);
				Promise<bool> DestroyTexture(Texture::Handle Handle);
				Promise<bool> SetTextureVerticalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode);
				Promise<bool> SetTextureHorizontalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode);
				Promise<bool> SetTextureMinifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MinifyFilters Filter);
				Promise<bool> SetTextureMagnifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MagnfyFilters Filter);
				Promise<bool> GenerateTextureMipMap(Texture::Handle Handle, Texture::Types Type);

				Promise<bool> CreateRenderTarget(const RenderTargetInfo* Info, RenderTarget::Handle& Handle, IDevice::TextureList& Textures);
				Promise<bool> DestroyRenderTarget(RenderTarget::Handle Handle);

				Promise<bool> CreateMesh(const SubMeshInfo* Info, SubMesh::Handle& Handle);
				Promise<bool> DestroyMesh(SubMesh::Handle Handle);

				Promise<bool> SetDebugCallback(IDevice::DebugFunction Callback);

				CommandsHolder* GetCommandHolder(void)
				{
					return m_CommandsHolder;
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
				CommandsHolder* m_CommandsHolder;
			};
		}
	}
}

#endif