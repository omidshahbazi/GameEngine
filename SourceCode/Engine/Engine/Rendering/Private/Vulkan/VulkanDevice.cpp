// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\Vulkan\VulkanDevice.h>
#include <Rendering\Private\RenderingAllocators.h>
#include <Debugging\Debug.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Utility\Window.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;
	using namespace MemoryManagement::Allocator;

	namespace Rendering
	{
		namespace Private
		{
			namespace Vulkan
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

				bool VulkanDevice::CreateContext(PlatformWindow::WindowHandle WindowHandle, RenderContext::Handle& Handle)
				{
					return false;
				}

				bool VulkanDevice::DestroyContext(RenderContext::Handle Handle)
				{
					return false;
				}

				bool VulkanDevice::SetContext(RenderContext::Handle Handle)
				{
					return false;
				}

				bool VulkanDevice::SetViewport(const Vector2I& Position, const Vector2I& Size)
				{
					return true;
				}
				bool VulkanDevice::SetClearColor(const ColorUI8& Color)
				{
					return true;
				}
				void VulkanDevice::SetState(const State& State)
				{
				}
				bool VulkanDevice::SetResourceName(NativeType::Handle Handle, ResourceTypes Type, cwstr Name)
				{
					return false;
				}
				bool VulkanDevice::CreateBuffer(GPUBuffer::Handle& Handle)
				{
					return true;
				}
				bool VulkanDevice::DestroyBuffer(GPUBuffer::Handle Handle)
				{
					return true;
				}
				bool VulkanDevice::InitializeConstantBuffer(GPUBuffer::Handle Handle, const byte* Data, uint32 Size)
				{
					return true;
				}
				bool VulkanDevice::CopyFromBufferToBuffer(GPUBuffer::Handle Handle, GPUBuffer::Handle FromHandle, uint32 Size)
				{
					return false;
				}
				bool VulkanDevice::CopyFromVertexToBuffer(GPUBuffer::Handle Handle, SubMesh::Handle FromMeshHandle, uint32 Size)
				{
					return true;
				}
				bool VulkanDevice::CopyFromBufferToVertex(GPUBuffer::Handle Handle, SubMesh::Handle ToMeshHandle, uint32 Size)
				{
					return true;
				}
				bool VulkanDevice::CopyFromIndexToBuffer(GPUBuffer::Handle Handle, SubMesh::Handle FromMeshHandle, uint32 Size)
				{
					return true;
				}
				bool VulkanDevice::CopyFromBufferToIndex(GPUBuffer::Handle Handle, SubMesh::Handle ToMeshHandle, uint32 Size)
				{
					return true;
				}
				bool VulkanDevice::CopyFromTextureToBuffer(GPUBuffer::Handle Handle, Texture::Handle FromTextureHandle, uint32 Size, Texture::Types TextureType, Formats TextureFormat, uint32 Level)
				{
					return true;
				}
				bool VulkanDevice::CopyFromBufferToTexture(GPUBuffer::Handle Handle, Texture::Handle ToTextureHandle, Texture::Types TextureType, uint32 Width, uint32 Height, Formats TextureFormat)
				{
					return true;
				}
				bool VulkanDevice::LockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Access Access, byte** Buffer)
				{
					return true;
				}
				bool VulkanDevice::UnlockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type)
				{
					return true;
				}
				bool VulkanDevice::CompileProgramAPI(const Shaders* Shaders, CompiledShaders* CompiledShaders, cstr* ErrorMessage)
				{
					return true;
				}
				bool VulkanDevice::CompileProgram(const Shaders* Shaders, CompiledShaders* CompiledShaders, cstr* ErrorMessage)
				{
					return CompileProgramAPI(Shaders, CompiledShaders, ErrorMessage);
				}
				bool VulkanDevice::CreateProgram(const CompiledShaders* Shaders, Program::Handle& Handle, cstr* ErrorMessage)
				{
					return false;
				}
				bool VulkanDevice::DestroyProgram(Program::Handle Handle)
				{
					return true;
				}
				bool VulkanDevice::BindProgram(Program::Handle Handle)
				{
					return true;
				}
				bool VulkanDevice::SetProgramConstantBuffer(Program::ConstantHandle Handle, ConstantBuffer::Handle Value)
				{
					return true;
				}
				bool VulkanDevice::SetProgramTexture(Program::ConstantHandle Handle, Texture::Types Type, Texture::Handle Value)
				{
					return true;
				}
				bool VulkanDevice::CreateTexture(const TextureInfo* Info, Texture::Handle& Handle)
				{
					return true;
				}
				bool VulkanDevice::DestroyTexture(Texture::Handle Handle)
				{
					return true;
				}
				bool VulkanDevice::BindTexture(Texture::Handle Handle, Texture::Types Type)
				{
					return true;
				}
				bool VulkanDevice::SetTextureVerticalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode)
				{
					return true;
				}
				bool VulkanDevice::SetTextureHorizontalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode)
				{
					return true;
				}
				bool VulkanDevice::SetTextureMinifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MinifyFilters Filter)
				{
					return true;
				}
				bool VulkanDevice::SetTextureMagnifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MagnfyFilters Filter)
				{
					return true;
				}
				bool VulkanDevice::GenerateTextureMipMap(Texture::Handle Handle, Texture::Types Type)
				{
					return true;
				}
				bool VulkanDevice::CreateRenderTarget(const RenderTargetInfo* Info, RenderTarget::Handle& Handle, TextureList& Textures)
				{
					return true;
				}
				bool VulkanDevice::DestroyRenderTarget(RenderTarget::Handle Handle)
				{
					return true;
				}
				bool VulkanDevice::BindRenderTarget(RenderTarget::Handle Handle)
				{
					return true;
				}
				bool VulkanDevice::CreateMesh(const SubMeshInfo* Info, SubMesh::Handle& Handle)
				{
					return true;
				}
				bool VulkanDevice::DestroyMesh(SubMesh::Handle Handle)
				{
					return true;
				}

				bool VulkanDevice::BindMesh(SubMesh::Handle Handle)
				{
					return true;
				}
				bool VulkanDevice::Clear(ClearFlags Flags)
				{
					return true;
				}
				bool VulkanDevice::DrawIndexed(SubMesh::PolygonTypes PolygonType, uint32 IndexCount)
				{
					return true;
				}
				bool VulkanDevice::DrawArray(SubMesh::PolygonTypes PolygonType, uint32 VertexCount)
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
}