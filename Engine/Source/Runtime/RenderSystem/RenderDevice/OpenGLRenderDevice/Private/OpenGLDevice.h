// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef OPEN_GL_DEVICE_H
#define OPEN_GL_DEVICE_H

#include <Containers\Map.h>
#include <RenderDevice\IDevice.h>
#include <RenderDevice\Private\NativeCommandBufferPool.h>
#include <OpenGLRenderDevice\Private\OpenGLCommandBuffer.h>
#include <OpenGLRenderDevice\Private\OpenGLCommon.h>
#include <WindowUtility\Window.h>

namespace Engine
{
	using namespace Platform;
	using namespace RenderDevice;
	using namespace RenderDevice::Private;
	using namespace WindowUtility;

	namespace OpenGLRenderDevice
	{
		namespace Private
		{
			class OpenGLDevice : public IDevice
			{
			private:
				typedef NativeCommandBufferPool<OpenGLCommandBuffer> CommandBufferPool;

			public:
				OpenGLDevice(void);
				~OpenGLDevice(void);

				bool Initialize(void) override;
				bool Uninitialize(void) override;

				cstr GetVersion(void) override;
				cstr GetVendorName(void) override;
				cstr GetRendererName(void) override;
				cstr GetShadingLanguageVersion(void) override;

				bool CreateContext(PlatformWindow::WindowHandle WindowHandle, ResourceHandle& Handle) override;
				bool DestroyContext(ResourceHandle Handle) override;
				bool SetContext(ResourceHandle Handle) override;
				bool SwapBuffers(void) override;

				bool CreateBuffer(GPUBufferTypes Type, uint32 Size, ResourceHandle& Handle) override;
				bool DestroyBuffer(ResourceHandle Handle) override;
				bool LockBuffer(ResourceHandle Handle, GPUBufferAccess Access, byte** Buffer) override;
				bool UnlockBuffer(ResourceHandle Handle) override;

				bool CreateProgram(const CompiledShaders* Shaders, ResourceHandle& Handle, cstr* ErrorMessage) override;
				bool DestroyProgram(ResourceHandle Handle) override;

				bool CreateTexture(const TextureInfo* Info, ResourceHandle& Handle) override;
				bool DestroyTexture(ResourceHandle Handle) override;
				bool SetTextureVerticalWrapping(ResourceHandle Handle, TextureWrapModes Mode) override;
				bool SetTextureHorizontalWrapping(ResourceHandle Handle, TextureWrapModes Mode) override;
				bool SetTextureMinifyFilter(ResourceHandle Handle, TextureMinifyFilters Filter) override;
				bool SetTextureMagnifyFilter(ResourceHandle Handle, TextureMagnfyFilters Filter) override;

				bool CreateRenderTarget(const RenderTargetInfo* Info, ResourceHandle& Handle, TextureList& Textures) override;
				bool DestroyRenderTarget(ResourceHandle Handle) override;

				bool CreateMesh(const SubMeshInfo* Info, ResourceHandle& Handle) override;
				bool DestroyMesh(ResourceHandle Handle) override;

				bool CreateCommandBuffer(ICommandBuffer::Types Type, ICommandBuffer*& Buffer) override;
				bool DestroyCommandBuffer(ICommandBuffer** Buffers, uint16 Count) override;
				bool SubmitCommandBuffer(ICommandBuffer* const* Buffers, uint16 Count) override;
				bool SubmitCommandBufferAsync(ICommandBuffer* const* Buffers, uint16 Count) override
				{
					return SubmitCommandBuffer(Buffers, Count);
				}

				bool SetResourceName(ResourceHandle Handle, ResourceTypes Type, cwstr Name) override;
				bool SetDebugCallback(DebugFunction Callback) override
				{
					m_DebugCallback = Callback;

					return true;
				}
				DebugFunction GetDebugCallback(void) const
				{
					return m_DebugCallback;
				}

				RenderContextInfo* GetCurrentContext(void) const
				{
					return m_CurrentContext;
				}

			private:
				bool InitializeBaseContext(void);

			private:
				bool m_Initialized;

				ResourceHandle m_BaseContextHandle;
				Window* m_BaseContextWindow;

				RenderContextMap m_Contexts;
				RenderContextInfo* m_BaseContext;
				ResourceHandle m_CurrentContextHandle;
				RenderContextInfo* m_CurrentContext;

				CommandBufferPool m_CommandBufferPool;

				DebugFunction m_DebugCallback;
			};
		}
	}
}

#endif