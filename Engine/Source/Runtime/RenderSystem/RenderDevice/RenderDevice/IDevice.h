// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef I_DEVICE_H
#define I_DEVICE_H

#include <Containers\Color.h>
#include <RenderCommon\RenderCommon.h>
#include <RenderCommon\RenderState.h>
#include <RenderDevice\TextureInfo.h>
#include <RenderDevice\RenderTargetInfo.h>
#include <RenderDevice\MeshInfo.h>
#include <RenderDevice\IFence.h>
#include <Platform\PlatformWindow.h>
#include <Common\BitwiseUtils.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;
	using namespace Containers;
	using namespace RenderCommon;

	namespace RenderDevice
	{
		class ICommandBuffer;

		class IDevice
		{
		public:
			enum class ResourceTypes
			{
				Buffer = 0,
				Program,
				Mesh,
				Texture,
				RenderTarget
			};

			enum class DebugSources
			{
				API = 0,
				WindowSystem,
				ProgramCompiler,
				ThirdParty,
				Application,
				Other
			};

			enum class DebugTypes
			{
				Error = 0,
				DeprecatedBehavior,
				UndefinedBehavior,
				Portability,
				Performance,
				Marker,
				PushGroup,
				PopGroup,
				Other
			};

			enum class DebugSeverities
			{
				Notification = 0,
				Low,
				Medium,
				High
			};

			typedef std::function<void(int32 ID, DebugSources Source, cstr Message, DebugTypes Type, DebugSeverities Severity)> DebugFunction;

			struct CompiledShaders
			{
			public:
				struct CompiledShader
				{
					byte* Buffer;
					uint16 Size;
				};

			public:
				CompiledShader VertexShader;
				CompiledShader HullShader;
				CompiledShader DomainShader;
				CompiledShader GeometryShader;
				CompiledShader FragmentShader;
				CompiledShader ComputeShader;
			};

			struct BufferFootprintInfo
			{
			public:
				uint32 Size;
				uint32 RowPitch;
				uint8 ElementPadding;
			};

			typedef Vector<ResourceHandle> TextureList;

		public:
			virtual ~IDevice(void)
			{
			}

			virtual bool Initialize(void) = 0;
			virtual bool Uninitialize(void) = 0;

			virtual cstr GetVersion(void) = 0;
			virtual cstr GetVendorName(void) = 0;
			virtual cstr GetRendererName(void) = 0;
			virtual cstr GetShadingLanguageVersion(void) = 0;

			virtual bool CreateContext(PlatformWindow::WindowHandle WindowHandle, ResourceHandle& Handle) = 0;
			virtual bool DestroyContext(ResourceHandle Handle) = 0;
			virtual bool SetContext(ResourceHandle Handle) = 0;
			virtual bool SwapBuffers(void) = 0;

			virtual bool CreateBuffer(GPUBufferTypes Type, uint32 Size, ResourceHandle& Handle) = 0;
			virtual	bool DestroyBuffer(ResourceHandle Handle) = 0;
			virtual bool LockBuffer(ResourceHandle Handle, GPUBufferAccess Access, byte** Buffer) = 0;
			virtual	bool UnlockBuffer(ResourceHandle Handle) = 0;

			virtual bool CreateProgram(const CompiledShaders* Shaders, ResourceHandle& Handle, cstr* ErrorMessage) = 0;
			virtual bool DestroyProgram(ResourceHandle Handle) = 0;

			virtual bool CreateTexture(const TextureInfo* Info, ResourceHandle& Handle) = 0;
			virtual bool DestroyTexture(ResourceHandle Handle) = 0;
			virtual bool GetTextureFootprint(ResourceHandle Handle, BufferFootprintInfo& Footprint) = 0;
			virtual bool SetTextureVerticalWrapping(ResourceHandle Handle, TextureWrapModes Mode) = 0;
			virtual bool SetTextureHorizontalWrapping(ResourceHandle Handle, TextureWrapModes Mode) = 0;
			virtual bool SetTextureMinifyFilter(ResourceHandle Handle, TextureMinifyFilters Filter) = 0;
			virtual bool SetTextureMagnifyFilter(ResourceHandle Handle, TextureMagnfyFilters Filter) = 0;

			virtual bool CreateRenderTarget(const RenderTargetInfo* Info, ResourceHandle& Handle, TextureList& Textures) = 0;
			virtual bool DestroyRenderTarget(ResourceHandle Handle) = 0;

			virtual bool CreateMesh(const SubMeshInfo* Info, ResourceHandle& Handle) = 0;
			virtual bool DestroyMesh(ResourceHandle Handle) = 0;

			virtual bool CreateCommandBuffer(ICommandBuffer*& Buffer) = 0;
			virtual bool DestroyCommandBuffers(ICommandBuffer** Buffers, uint8 Count) = 0;
			virtual bool SubmitCommandBuffers(ICommandBuffer* const* Buffers, uint8 Count) = 0;

			virtual bool CreateFence(IFence*& Fence) = 0;
			virtual bool DestroyFences(IFence** Fence, uint8 Count) = 0;

			virtual bool SetResourceName(ResourceHandle Handle, ResourceTypes Type, cwstr Name) = 0;
			virtual bool SetDebugCallback(DebugFunction Callback) = 0;
		};
	}
}

#endif