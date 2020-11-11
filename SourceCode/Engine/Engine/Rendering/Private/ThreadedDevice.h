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
		namespace Private
		{
			class RENDERING_API ThreadedDevice
			{
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

				Promise<RenderContext*> CreateContext(PlatformWindow::WindowHandle Handle);
				Promise<bool> DestroyContext(RenderContext* Context);
				Promise<bool> SetContext(RenderContext* Context);
				Promise<RenderContext*> GetContext(void);

				Promise<bool> SetViewport(const Vector2I& Position, const Vector2I& Size);

				Promise<bool> SetClearColor(const ColorUI8& Color);

				Promise<bool> SetFaceOrder(IDevice::FaceOrders Order);

				Promise<bool> SetCullMode(IDevice::CullModes Mode);

				Promise<bool> SetDepthTestFunction(IDevice::TestFunctions Function);

				Promise<bool> SetStencilTestFunction(IDevice::CullModes CullMode, IDevice::TestFunctions Function, int32 Reference, uint32 Mask);

				Promise<bool> SetStencilMask(IDevice::CullModes CullMode, uint32 Mask);

				Promise<bool> SetStencilOperation(IDevice::CullModes CullMode, IDevice::StencilOperations StencilFailed, IDevice::StencilOperations DepthFailed, IDevice::StencilOperations DepthPassed);

				Promise<bool> SetBlendEquation(IDevice::BlendEquations Equation);

				Promise<bool> SetBlendFunction(IDevice::BlendFunctions SourceFactor, IDevice::BlendFunctions DestinationFactor);

				Promise<bool> SetPolygonMode(IDevice::CullModes CullMode, IDevice::PolygonModes PolygonMode);

				Promise<bool> ResetState(void);
				Promise<IDevice::State> GetState(void);
				Promise<void> SetState(const IDevice::State& State);

				Promise<bool> CreateBuffer(GPUBuffer::Handle& Handle);
				Promise<bool> DestroyBuffer(GPUBuffer::Handle Handle);
				Promise<bool> BindBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type);
				Promise<bool> AttachBufferData(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Usages Usage, uint32 Size, const void* Data);
				Promise<bool> AttachBufferData(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Usages Usage, uint32 Size, Texture::Handle TextureHandle, Texture::Types TextureType, Texture::Formats TextureFormat, uint32 Level);
				Promise<bool> ReadBufferData(GPUBuffer::Handle Handle, GPUBuffer::Types Type, Texture::Handle TextureHandle, Texture::Types TextureType, uint32 Width, uint32 Height, Texture::Formats TextureFormat);
				Promise<bool> LockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Access Access, byte** Buffer);
				Promise<bool> UnlockBuffer(GPUBuffer::Types Type);

				Promise<bool> CreateShader(const IDevice::Shaders* Shaders, Shader::Handle& Handle, cstr* ErrorMessage);
				Promise<bool> DestroyShader(Shader::Handle Handle);
				Promise<bool> BindShader(Shader::Handle Handle);
				Promise<bool> QueryShaderActiveConstants(Shader::Handle Handle, Shader::ConstantDataList& Constants);
				Promise<bool> GetShaderConstantHandle(Shader::Handle Handle, const String& Name, Shader::ConstantHandle& ConstantHandle);
				Promise<bool> SetShaderFloat32(Shader::ConstantHandle Handle, float32 Value);
				Promise<bool> SetShaderColor(Shader::ConstantHandle Handle, const ColorUI8& Value);
				Promise<bool> SetShaderVector2(Shader::ConstantHandle Handle, const Vector2F& Value);
				Promise<bool> SetShaderVector3(Shader::ConstantHandle Handle, const Vector3F& Value);
				Promise<bool> SetShaderVector4(Shader::ConstantHandle Handle, const Vector4F& Value);
				Promise<bool> SetShaderMatrix4(Shader::ConstantHandle Handle, const Matrix4F& Value);
				Promise<bool> SetShaderTexture(Shader::ConstantHandle Handle, Texture::Types Type, Texture::Handle Value);

				Promise<bool> CreateTexture(const TextureInfo* Info, Texture::Handle& Handle);
				Promise<bool> DestroyTexture(Texture::Handle Handle);
				Promise<bool> BindTexture(Texture::Handle Handle, Texture::Types Type);
				Promise<bool> SetTextureVerticalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode);
				Promise<bool> SetTextureHorizontalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode);
				Promise<bool> SetTextureMinifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MinifyFilters Filter);
				Promise<bool> SetTextureMagnifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MagnfyFilters Filter);
				Promise<bool> GenerateTextureMipMap(Texture::Handle Handle, Texture::Types Type);

				Promise<bool> CreateRenderTarget(const RenderTargetInfo* Info, RenderTarget::Handle& Handle, IDevice::TextureList& Textures);
				Promise<bool> DestroyRenderTarget(RenderTarget::Handle Handle);
				Promise<bool> BindRenderTarget(RenderTarget::Handle Handle);

				Promise<bool> CreateMesh(const SubMeshInfo* Info, GPUBuffer::Usages Usage, SubMesh::Handle& Handle);
				Promise<bool> GetMeshVertexBuffer(SubMesh::Handle Handle, GPUBuffer::Handle& BufferHandle);
				Promise<bool> GetMeshElementBuffer(SubMesh::Handle Handle, GPUBuffer::Handle& BufferHandle);
				Promise<bool> DestroyMesh(SubMesh::Handle Handle);
				Promise<bool> BindMesh(SubMesh::Handle Handle);

				Promise<bool> Clear(IDevice::ClearFlags Flags);

				Promise<bool> DrawIndexed(SubMesh::PolygonTypes PolygonType, uint32 IndexCount);
				Promise<bool> DrawArray(SubMesh::PolygonTypes PolygonType, uint32 VertexCount);

				Promise<bool> SwapBuffers(void);

				Promise<bool> SetDebugCallback(IDevice::DebugFunction Callback);

				CommandsHolder* GetCommandHolder(void)
				{
					return &m_CommandsHolder;
				}

			private:
				void Worker(void);

			private:
				Thread m_Thread;
				TaskQueue m_Tasks;
				SpinLock m_TasksLock;
				IDevice* m_Device;
				DeviceTypes m_DeviceType;
				CommandsHolder m_CommandsHolder;
			};
		}
	}
}

#endif