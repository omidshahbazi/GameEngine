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
			bool VulkanDevice::SwapBuffers(void)
			{
				return true;
			}
			bool VulkanDevice::CreateBuffer(GPUBufferTypes Type, uint32 Size, ResourceHandle& Handle)
			{
				return false;
			}
			bool VulkanDevice::DestroyBuffer(ResourceHandle Handle)
			{
				return false;
			}
			bool VulkanDevice::LockBuffer(ResourceHandle Handle, GPUBufferAccess Access, byte** Buffer)
			{
				return false;
			}
			bool VulkanDevice::UnlockBuffer(ResourceHandle Handle)
			{
				return false;
			}
			bool VulkanDevice::CreateProgram(const CompiledShaders* Shaders, ResourceHandle& Handle, cstr* ErrorMessage)
			{
				return false;
			}
			bool VulkanDevice::DestroyProgram(ResourceHandle Handle)
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
			bool VulkanDevice::SetTextureVerticalWrapping(ResourceHandle Handle, TextureWrapModes Mode)
			{
				return true;
			}
			bool VulkanDevice::SetTextureHorizontalWrapping(ResourceHandle Handle, TextureWrapModes Mode)
			{
				return true;
			}
			bool VulkanDevice::SetTextureMinifyFilter(ResourceHandle Handle, TextureMinifyFilters Filter)
			{
				return true;
			}
			bool VulkanDevice::SetTextureMagnifyFilter(ResourceHandle Handle, TextureMagnfyFilters Filter)
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
			bool VulkanDevice::CreateMesh(const SubMeshInfo* Info, ResourceHandle& Handle)
			{
				return true;
			}
			bool VulkanDevice::DestroyMesh(ResourceHandle Handle)
			{
				return true;
			}
			bool VulkanDevice::CreateCommandBuffer(ICommandBuffer*& Buffer)
			{
				return false;
			}
			bool VulkanDevice::DestroyCommandBuffer(ICommandBuffer** Buffers, uint16 Count)
			{
				return false;
			}
			bool VulkanDevice::SubmitCommandBuffer(ICommandBuffer* const* Buffers, uint16 Count)
			{
				return false;
			}
			bool VulkanDevice::SubmitCommandBufferAsync(ICommandBuffer* const* Buffers, uint16 Count)
			{
				return false;
			}
			bool VulkanDevice::SetResourceName(ResourceHandle Handle, ResourceTypes Type, cwstr Name)
			{
				return false;
			}
			bool VulkanDevice::SetDebugCallback(DebugFunction Callback)
			{
				return true;
			}
		}
	}
}