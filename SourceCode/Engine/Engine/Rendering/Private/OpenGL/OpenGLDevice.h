// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef OPEN_GL_DEVICE_H
#define OPEN_GL_DEVICE_H

#include <Containers\Map.h>
#include <Rendering\IDevice.h>

//https://www.khronos.org/opengl/wiki/Common_Mistakes

namespace Engine
{
	using namespace Platform;

	namespace Rendering
	{
		namespace Private
		{
			namespace OpenGL
			{
				class RENDERING_API OpenGLDevice : public IDevice
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
						BufferInfo VertexBufferObject;
						BufferInfo IndexBufferObject;
						SubMesh::VertexLayouts Layout;
					};

					struct ProgramInfo
					{
					public:
						uint32 Handle;
					};

					struct RenderTargetInfos
					{
					public:
						RenderTarget::Handle Handle;
						TextureList Textures;
					};

					class RenderContextInfo
					{
					public:
						typedef Map<SubMesh::Handle, GPUBuffer::Handle> MeshVertexArrayMap;

					public:
						PlatformWindow::ContextHandle ContextHandle;
						PlatformWindow::WGLContextHandle WGLContextHandle;

						MeshVertexArrayMap VertexArrays;
						GPUBuffer::Handle LastMeshHandle;

						bool IsActive;
					};

					typedef Map<RenderContext::Handle, RenderContextInfo*> RenderContextMap;

				public:
					OpenGLDevice(void);
					~OpenGLDevice(void);

					bool Initialize(void) override;

					cstr GetVersion(void) override;
					cstr GetVendorName(void) override;
					cstr GetRendererName(void) override;
					cstr GetShadingLanguageVersion(void) override;

					bool CreateContext(PlatformWindow::WindowHandle WindowHandle, RenderContext::Handle& Handle) override;
					bool DestroyContext(RenderContext::Handle Handle) override;
					bool SetContext(RenderContext::Handle Handle) override;

					bool SetViewport(const Vector2I& Position, const Vector2I& Size) override;

					bool SetClearColor(const ColorUI8& Color) override;

					bool SetResourceName(NativeType::Handle Handle, ResourceTypes Type, cwstr Name) override;

					bool ResetState(void) override
					{
						SetState({});

						m_LastProgram = 0;
						m_LastFrameBuffer = 0;

						return true;
					}

					const State& GetState(void) const override
					{
						return m_State;
					}
					void SetState(const State& State) override;

					bool CreateBuffer(GPUBuffer::Handle& Handle) override;
					bool DestroyBuffer(GPUBuffer::Handle Handle) override;
					bool InitializeConstantBuffer(GPUBuffer::Handle Handle, const byte* Data, uint32 Size) override;
					bool CopyFromBufferToBuffer(GPUBuffer::Handle Handle, GPUBuffer::Handle FromHandle, uint32 Size) override;
					bool CopyFromVertexToBuffer(GPUBuffer::Handle Handle, SubMesh::Handle FromMeshHandle, uint32 Size) override;
					bool CopyFromBufferToVertex(GPUBuffer::Handle Handle, SubMesh::Handle ToMeshHandle, uint32 Size) override;
					bool CopyFromIndexToBuffer(GPUBuffer::Handle Handle, SubMesh::Handle FromMeshHandle, uint32 Size) override;
					bool CopyFromBufferToIndex(GPUBuffer::Handle Handle, SubMesh::Handle ToMeshHandle, uint32 Size) override;
					bool CopyFromTextureToBuffer(GPUBuffer::Handle Handle, Texture::Handle FromTextureHandle, uint32 Size, Texture::Types TextureType, Formats TextureFormat, uint32 Level) override;
					bool CopyFromBufferToTexture(GPUBuffer::Handle Handle, Texture::Handle ToTextureHandle, Texture::Types TextureType, uint32 Width, uint32 Height, Formats TextureFormat) override;
					bool LockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Access Access, byte** Buffer) override;
					bool UnlockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type) override;

					static bool CompileProgramAPI(const Shaders* Shaders, CompiledShaders* CompiledShaders, cstr* ErrorMessage);

					bool CompileProgram(const Shaders* Shaders, CompiledShaders* CompiledShaders, cstr* ErrorMessage) override;
					bool CreateProgram(const CompiledShaders* Shaders, Program::Handle& Handle, cstr* ErrorMessage) override;
					bool DestroyProgram(Program::Handle Handle) override;
					bool BindProgram(Program::Handle Handle) override;
					bool SetProgramConstantBuffer(Program::ConstantHandle Handle, ConstantBuffer::Handle Value) override;
					bool SetProgramTexture(Program::ConstantHandle Handle, Texture::Types Type, Texture::Handle Value) override;

					bool CreateTexture(const TextureInfo* Info, Texture::Handle& Handle) override;
					bool DestroyTexture(Texture::Handle Handle) override;
					bool SetTextureVerticalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode) override;
					bool SetTextureHorizontalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode) override;
					bool SetTextureMinifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MinifyFilters Filter) override;
					bool SetTextureMagnifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MagnfyFilters Filter) override;
					bool GenerateTextureMipMap(Texture::Handle Handle, Texture::Types Type) override;

					bool CreateRenderTarget(const RenderTargetInfo* Info, RenderTarget::Handle& Handle, TextureList& Textures) override;
					bool DestroyRenderTarget(RenderTarget::Handle Handle) override;
					bool BindRenderTarget(RenderTarget::Handle Handle) override;

					bool CreateMesh(const SubMeshInfo* Info, SubMesh::Handle& Handle) override;
					bool DestroyMesh(SubMesh::Handle Handle) override;
					bool CreateVertexArray(const MeshBufferInfo& Info, NativeType::Handle& Handle);
					bool DestroyVertexArray(NativeType::Handle Handle);
					bool BindMesh(SubMesh::Handle Handle) override;

					bool Clear(ClearFlags Flags) override;

					bool DrawIndexed(SubMesh::PolygonTypes PolygonType, uint32 IndexCount) override;
					bool DrawArray(SubMesh::PolygonTypes PolygonType, uint32 VertexCount)  override;

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
					bool DestroyBuffer(BufferInfo* Info);
					bool LockBuffer(BufferInfo* Info, GPUBuffer::Types Type, GPUBuffer::Access Access, byte** Buffer);
					bool UnlockBuffer(BufferInfo* Info, GPUBuffer::Types Type);

				private:
					bool m_Initialized;

					RenderContextMap m_Contexts;
					RenderContextInfo* m_BaseContext;
					RenderContext::Handle m_CurrentContextHandle;
					RenderContextInfo* m_CurrentContext;

					ColorUI8 m_ClearColor;
					State m_State;

					Program::Handle m_LastProgram;
					RenderTarget::Handle m_LastFrameBuffer;

					DebugFunction m_DebugCallback;
				};
			}
		}
	}
}

#endif