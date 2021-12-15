// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <VulkanRenderDevice\Private\VulkanDevice.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace VulkanRenderDevice
	{
		namespace Private
		{
			VulkanDevice::VulkanDevice(void)
			{
			}
			VulkanDevice::~VulkanDevice(void)
			{
			}
			bool VulkanDevice::Initialize(void)
			{
				return true;
			}
			bool VulkanDevice::Uninitialize(void)
			{
				return false;
			}
			cstr VulkanDevice::GetVersion(void)
			{
				return cstr();
			}
			cstr VulkanDevice::GetVendorName(void)
			{
				return cstr();
			}
			cstr VulkanDevice::GetRendererName(void)
			{
				return cstr();
			}
			cstr VulkanDevice::GetShadingLanguageVersion(void)
			{
				return cstr();
			}

			bool VulkanDevice::CreateContext(PlatformWindow::WindowHandle WindowHandle, ResourceHandle& Handle)
			{
				return false;
			}

			bool VulkanDevice::DestroyContext(ResourceHandle Handle)
			{
				return false;
			}

			bool VulkanDevice::SetContext(ResourceHandle Handle)
			{
				return false;
			}

			bool VulkanDevice::SetContextSize(const Vector2I& Size)
			{
				return false;
			}

			bool VulkanDevice::SetViewport(const Vector2I& Position, const Vector2I& Size)
			{
				return false;
			}
			void VulkanDevice::SetState(const RenderState& State)
			{
			}
			bool VulkanDevice::SetResourceName(ResourceHandle Handle, ResourceTypes Type, cwstr Name)
			{
				return false;
			}
			bool VulkanDevice::CreateBuffer(ResourceHandle& Handle)
			{
				return true;
			}
			bool VulkanDevice::DestroyBuffer(ResourceHandle Handle)
			{
				return true;
			}
			bool VulkanDevice::InitializeConstantBuffer(ResourceHandle Handle, const byte* Data, uint32 Size)
			{
				return true;
			}
			bool VulkanDevice::CopyFromVertexToBuffer(ResourceHandle Handle, ResourceHandle FromMeshHandle, uint32 Size)
			{
				return true;
			}
			bool VulkanDevice::CopyFromBufferToVertex(ResourceHandle Handle, ResourceHandle ToMeshHandle, uint32 Size)
			{
				return true;
			}
			bool VulkanDevice::CopyFromIndexToBuffer(ResourceHandle Handle, ResourceHandle FromMeshHandle, uint32 Size)
			{
				return true;
			}
			bool VulkanDevice::CopyFromBufferToIndex(ResourceHandle Handle, ResourceHandle ToMeshHandle, uint32 Size)
			{
				return true;
			}
			bool VulkanDevice::CopyFromTextureToBuffer(ResourceHandle Handle, ResourceHandle FromTextureHandle, uint32 Size, TextureTypes TextureType, Formats TextureFormat, uint32 Level)
			{
				return true;
			}
			bool VulkanDevice::CopyFromBufferToTexture(ResourceHandle Handle, ResourceHandle ToTextureHandle, TextureTypes TextureType, uint32 Width, uint32 Height, Formats TextureFormat)
			{
				return true;
			}
			bool VulkanDevice::LockBuffer(ResourceHandle Handle, GPUBufferTypes Type, GPUBufferAccess Access, byte** Buffer)
			{
				return true;
			}
			bool VulkanDevice::UnlockBuffer(ResourceHandle Handle, GPUBufferTypes Type)
			{
				return true;
			}
			bool VulkanDevice::CreateProgram(const CompiledShaders* Shaders, ResourceHandle& Handle, cstr* ErrorMessage)
			{
				return false;
			}
			bool VulkanDevice::DestroyProgram(ResourceHandle Handle)
			{
				return true;
			}
			bool VulkanDevice::BindProgram(ResourceHandle Handle)
			{
				return true;
			}
			bool VulkanDevice::SetProgramConstantBuffer(ProgramConstantHandle Handle, ResourceHandle Value)
			{
				return true;
			}
			bool VulkanDevice::SetProgramTexture(ProgramConstantHandle Handle, TextureTypes Type, ResourceHandle Value)
			{
				return true;
			}
			bool VulkanDevice::CreateTexture(const TextureInfo* Info, ResourceHandle& Handle)
			{
				return true;
			}
			bool VulkanDevice::DestroyTexture(ResourceHandle Handle)
			{
				return true;
			}
			bool VulkanDevice::SetTextureVerticalWrapping(ResourceHandle Handle, TextureTypes Type, TextureWrapModes Mode)
			{
				return true;
			}
			bool VulkanDevice::SetTextureHorizontalWrapping(ResourceHandle Handle, TextureTypes Type, TextureWrapModes Mode)
			{
				return true;
			}
			bool VulkanDevice::SetTextureMinifyFilter(ResourceHandle Handle, TextureTypes Type, TextureMinifyFilters Filter)
			{
				return true;
			}
			bool VulkanDevice::SetTextureMagnifyFilter(ResourceHandle Handle, TextureTypes Type, TextureMagnfyFilters Filter)
			{
				return true;
			}
			bool VulkanDevice::GenerateTextureMipMap(ResourceHandle Handle, TextureTypes Type)
			{
				return true;
			}
			bool VulkanDevice::CreateRenderTarget(const RenderTargetInfo* Info, ResourceHandle& Handle, TextureList& Textures)
			{
				return true;
			}
			bool VulkanDevice::DestroyRenderTarget(ResourceHandle Handle)
			{
				return true;
			}
			bool VulkanDevice::BindRenderTarget(ResourceHandle Handle)
			{
				return true;
			}
			bool VulkanDevice::CreateMesh(const SubMeshInfo* Info, ResourceHandle& Handle)
			{
				return true;
			}
			bool VulkanDevice::DestroyMesh(ResourceHandle Handle)
			{
				return true;
			}

			bool VulkanDevice::BindMesh(ResourceHandle Handle)
			{
				return true;
			}
			bool VulkanDevice::Clear(ClearFlags Flags, const ColorUI8& Color)
			{
				return true;
			}
			bool VulkanDevice::DrawIndexed(PolygonTypes PolygonType, uint32 IndexCount)
			{
				return true;
			}
			bool VulkanDevice::DrawArray(PolygonTypes PolygonType, uint32 VertexCount)
			{
				return true;
			}
			bool VulkanDevice::BeginExecute(void)
			{
				return true;
			}
			bool VulkanDevice::EndExecute(void)
			{
				return true;
			}
			bool VulkanDevice::SwapBuffers(void)
			{
				return true;
			}
			bool VulkanDevice::BeginEvent(cwstr Label)
			{
				return true;
			}

			bool VulkanDevice::EndEvent(void)
			{
				return true;
			}

			bool VulkanDevice::SetMarker(cwstr Label)
			{
				return true;
			}
			bool VulkanDevice::SetDebugCallback(DebugFunction Callback)
			{
				return true;
			}
		}
	}
}