// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef OPEN_GL_DEVICE_H
#define OPEN_GL_DEVICE_H

#include <Containers\Map.h>
#include <RenderDevice\IDevice.h>
#include <Platform\PlatformGL.h>

namespace Engine
{
	using namespace Platform;
	using namespace RenderDevice;

	namespace OpenGLRenderDevice
	{
		namespace Private
		{
			class OpenGLDevice : public IDevice
			{
			private:
				struct BufferInfo
				{
				public:
					uint32 Handle;
				};

				struct MeshBufferInfo
				{
				public:
					BufferInfo* VertexBufferObject;
					BufferInfo* IndexBufferObject;
					VertexLayouts Layout;
				};

				struct ProgramInfo
				{
				public:
					uint32 Handle;
				};

				struct RenderTargetInfos
				{
				public:
					ResourceHandle Handle;
					TextureList Textures;
				};

				class RenderContextInfo
				{
				public:
					typedef Map<ResourceHandle, ResourceHandle> MeshVertexArrayMap;

				public:
					PlatformGL::ContextHandle ContextHandle;
					PlatformGL::WGLContextHandle WGLContextHandle;

					MeshVertexArrayMap VertexArrays;
					ResourceHandle LastMeshHandle;

					bool IsActive;
				};

				typedef Map<ResourceHandle, RenderContextInfo*> RenderContextMap;

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

				bool SetContextSize(const Vector2I& Size) override;
				bool SetViewport(const Vector2I& Position, const Vector2I& Size) override;

				bool SetResourceName(ResourceHandle Handle, ResourceTypes Type, cwstr Name) override;

				bool ResetState(void) override
				{
					SetState({});

					m_LastProgram = 0;
					m_LastFrameBuffer = 0;

					return true;
				}

				const RenderState& GetState(void) const override
				{
					return m_State;
				}
				void SetState(const RenderState& State) override;

				bool CreateBuffer(ResourceHandle& Handle) override;
				bool DestroyBuffer(ResourceHandle Handle) override;
				bool LockBuffer(ResourceHandle Handle, GPUBufferTypes Type, GPUBufferAccess Access, byte** Buffer) override;
				bool UnlockBuffer(ResourceHandle Handle, GPUBufferTypes Type) override;
				bool InitializeConstantBuffer(ResourceHandle Handle, const byte* Data, uint32 Size) override;
				bool CopyFromVertexToBuffer(ResourceHandle Handle, ResourceHandle FromMeshHandle, uint32 Size) override;
				bool CopyFromBufferToVertex(ResourceHandle Handle, ResourceHandle ToMeshHandle, uint32 Size) override;
				bool CopyFromIndexToBuffer(ResourceHandle Handle, ResourceHandle FromMeshHandle, uint32 Size) override;
				bool CopyFromBufferToIndex(ResourceHandle Handle, ResourceHandle ToMeshHandle, uint32 Size) override;
				bool CopyFromTextureToBuffer(ResourceHandle Handle, ResourceHandle FromTextureHandle, uint32 Size, TextureTypes TextureType, Formats TextureFormat, uint32 Level) override;
				bool CopyFromBufferToTexture(ResourceHandle Handle, ResourceHandle ToTextureHandle, TextureTypes TextureType, uint32 Width, uint32 Height, Formats TextureFormat) override;

				bool CreateProgram(const CompiledShaders* Shaders, ResourceHandle& Handle, cstr* ErrorMessage) override;
				bool DestroyProgram(ResourceHandle Handle) override;
				bool BindProgram(ResourceHandle Handle) override;
				bool SetProgramConstantBuffer(ProgramConstantHandle Handle, ResourceHandle Value) override;
				bool SetProgramTexture(ProgramConstantHandle Handle, TextureTypes Type, ResourceHandle Value) override;

				bool CreateTexture(const TextureInfo* Info, ResourceHandle& Handle) override;
				bool DestroyTexture(ResourceHandle Handle) override;
				bool SetTextureVerticalWrapping(ResourceHandle Handle, TextureTypes Type, TextureWrapModes Mode) override;
				bool SetTextureHorizontalWrapping(ResourceHandle Handle, TextureTypes Type, TextureWrapModes Mode) override;
				bool SetTextureMinifyFilter(ResourceHandle Handle, TextureTypes Type, TextureMinifyFilters Filter) override;
				bool SetTextureMagnifyFilter(ResourceHandle Handle, TextureTypes Type, TextureMagnfyFilters Filter) override;
				bool GenerateTextureMipMap(ResourceHandle Handle, TextureTypes Type) override;

				bool CreateRenderTarget(const RenderTargetInfo* Info, ResourceHandle& Handle, TextureList& Textures) override;
				bool DestroyRenderTarget(ResourceHandle Handle) override;
				bool BindRenderTarget(ResourceHandle Handle) override;

				bool CreateMesh(const SubMeshInfo* Info, ResourceHandle& Handle) override;
				bool DestroyMesh(ResourceHandle Handle) override;
				bool BindMesh(ResourceHandle Handle) override;

				bool Clear(ClearFlags Flags, const ColorUI8& Color) override;

				bool DrawIndexed(PolygonTypes PolygonType, uint32 IndexCount) override;
				bool DrawArray(PolygonTypes PolygonType, uint32 VertexCount)  override;

				bool BeginExecute(void) override
				{
					return true;
				}
				bool EndExecute(void) override
				{
					return true;
				}

				bool SwapBuffers(void) override;

				bool BeginEvent(cwstr Label) override;

				bool EndEvent(void) override
				{
					return true;
				}

				bool SetMarker(cwstr Label) override;

				bool SetDebugCallback(DebugFunction Callback) override;

				DebugFunction GetDebugCallback(void) const
				{
					return m_DebugCallback;
				}

			private:
				bool InitializeBaseContext(void);

				bool CreateVertexArray(const MeshBufferInfo& Info, ResourceHandle& Handle);
				bool DestroyVertexArray(ResourceHandle Handle);

				bool LockBuffer(BufferInfo* Info, GPUBufferTypes Type, GPUBufferAccess Access, byte** Buffer);
				bool UnlockBuffer(BufferInfo* Info, GPUBufferTypes Type);

			private:
				bool m_Initialized;

				ResourceHandle m_BaseContextHandle;
				byte* m_BaseContextWindow;

				RenderContextMap m_Contexts;
				RenderContextInfo* m_BaseContext;
				ResourceHandle m_CurrentContextHandle;
				RenderContextInfo* m_CurrentContext;

				ColorUI8 m_ClearColor;
				RenderState m_State;

				ResourceHandle m_LastProgram;
				ResourceHandle m_LastFrameBuffer;

				DebugFunction m_DebugCallback;
			};
		}
	}
}

#endif