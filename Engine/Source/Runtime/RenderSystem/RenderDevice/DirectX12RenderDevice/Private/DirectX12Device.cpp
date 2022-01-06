// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <DirectX12RenderDevice\Private\DirectX12Device.h>
#include <DirectX12RenderDevice\Private\DirectX12DebugInfo.h>
#include <DirectX12RenderDevice\Private\DirectX12CommandBuffer.h>
#include <DirectX12RenderDevice\Private\DirectX12Common.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <RenderCommon\Helper.h>
#include <Containers\StringUtility.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace Platform;
	using namespace RenderCommon::Private;

	namespace DirectX12RenderDevice
	{
		namespace Private
		{
#define INITIALIZE_RESOURCE_INFO(ResourceInfoPtr, CurrentState) \
				(ResourceInfoPtr)->Resource = {}; \
				(ResourceInfoPtr)->View = {}; \
				(ResourceInfoPtr)->State = CurrentState;

#define REALLOCATE_SAMPLER(TextureResourcePtr) \
				if (!CHECK_CALL(m_SamplerViewAllocator.DeallocateView(TextureResourcePtr->SamplerView))) \
					return false; \
				return CHECK_CALL(m_SamplerViewAllocator.AllocateSampler(TextureResourcePtr->SamplerDescription, &TextureResourcePtr->SamplerView));

#define BEGIN_UPLOAD() \
				{ \
					byte* buffer = nullptr; \
					if (!CHECK_CALL(DirectX12Wrapper::Resource::Map(m_UploadBuffer.Resource.Resource, &buffer))) \
						return false; \
					PlatformMemory::Set(buffer, 0, UPLAOD_BUFFER_SIZE);

#define END_UPLOAD(BufferType, MainResourceInfo, DestinationIsABuffer) \
					if (!CHECK_CALL(DirectX12Wrapper::Resource::Unmap(m_UploadBuffer.Resource.Resource))) \
						return false; \
					if (!CopyBuffer(BufferType, &m_UploadBuffer, true, MainResourceInfo, DestinationIsABuffer)) \
						return false; \
				}

#define SKIP_NEXT_FRAMES() m_RenderCommandSet.SkipFrameCount = 1

			const uint8 BACK_BUFFER_COUNT = 2;
			const uint32 UPLAOD_BUFFER_SIZE = 8 * MegaByte;

			D3D12_RESOURCE_DIMENSION GetTextureType(TextureTypes Type)
			{
				switch (Type)
				{
				case TextureTypes::TwoD:
					return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				}

				return D3D12_RESOURCE_DIMENSION_UNKNOWN;
			}

			DXGI_FORMAT GetFormat(Formats Format)
			{
				switch (Format)
				{
				case Formats::R8:
					return DXGI_FORMAT_R8_UNORM;
				case Formats::R16:
					return DXGI_FORMAT_R16_UNORM;
				case Formats::R32:
					return DXGI_FORMAT_R32_UINT;
				case Formats::R16F:
					return DXGI_FORMAT_R16_FLOAT;
				case Formats::R32F:
					return DXGI_FORMAT_R32_FLOAT;
				case Formats::RG8:
					return DXGI_FORMAT_R8G8_UNORM;
				case Formats::RG16:
					return DXGI_FORMAT_R16G16_UNORM;
				case Formats::RG32:
					return DXGI_FORMAT_R32G32_UINT;
				case Formats::RG16F:
					return DXGI_FORMAT_R16G16_FLOAT;
				case Formats::RG32F:
					return DXGI_FORMAT_R32G32_FLOAT;
				case Formats::RGB8:
					return DXGI_FORMAT_R8G8B8A8_UNORM;
				case Formats::RGB16:
					return DXGI_FORMAT_R16G16B16A16_UNORM;
				case Formats::RGB32:
					return DXGI_FORMAT_R32G32B32_UINT;
				case Formats::RGB16F:
					return DXGI_FORMAT_R16G16B16A16_FLOAT;
				case Formats::RGB32F:
					return DXGI_FORMAT_R32G32B32_FLOAT;
				case Formats::RGBA8:
					return DXGI_FORMAT_R8G8B8A8_UNORM;
				case Formats::RGBA16:
					return DXGI_FORMAT_R16G16B16A16_UNORM;
				case Formats::RGBA32:
					return DXGI_FORMAT_R32G32B32A32_UINT;
				case Formats::RGBA16F:
					return DXGI_FORMAT_R16G16B16A16_FLOAT;
				case Formats::RGBA32F:
					return DXGI_FORMAT_R32G32B32A32_FLOAT;
				case Formats::Depth16:
					return DXGI_FORMAT_D16_UNORM;
				case Formats::Depth24:
					return DXGI_FORMAT_D24_UNORM_S8_UINT;
				case Formats::Depth32:
					return DXGI_FORMAT_D32_FLOAT;
				case Formats::Depth32F:
					return DXGI_FORMAT_D32_FLOAT;
				case Formats::DepthStencil24F:
					return DXGI_FORMAT_D24_UNORM_S8_UINT;
				case Formats::DepthStencil32F:
					return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
				}

				return DXGI_FORMAT_UNKNOWN;
			}

			DXGI_FORMAT GetTextureFormat(Formats Format)
			{
				switch (Format)
				{
				case Formats::R8:
					return DXGI_FORMAT_R8_UNORM;
				case Formats::R16:
					return DXGI_FORMAT_R16_UNORM;
				case Formats::R32:
					return DXGI_FORMAT_R32_UINT;
				case Formats::R16F:
					return DXGI_FORMAT_R16_FLOAT;
				case Formats::R32F:
					return DXGI_FORMAT_R32_FLOAT;
				case Formats::RG8:
					return DXGI_FORMAT_R8G8_UNORM;
				case Formats::RG16:
					return DXGI_FORMAT_R16G16_UNORM;
				case Formats::RG32:
					return DXGI_FORMAT_R32G32_UINT;
				case Formats::RG16F:
					return DXGI_FORMAT_R16G16_FLOAT;
				case Formats::RG32F:
					return DXGI_FORMAT_R32G32_FLOAT;
				case Formats::RGB8:
					return DXGI_FORMAT_R8G8B8A8_UNORM;
				case Formats::RGB16:
					return DXGI_FORMAT_R16G16B16A16_UNORM;
				case Formats::RGB32:
					return DXGI_FORMAT_R32G32B32A32_UINT;
				case Formats::RGB16F:
					return DXGI_FORMAT_R16G16B16A16_FLOAT;
				case Formats::RGB32F:
					return DXGI_FORMAT_R32G32B32A32_FLOAT;
				case Formats::RGBA8:
					return DXGI_FORMAT_R8G8B8A8_UNORM;
				case Formats::RGBA16:
					return DXGI_FORMAT_R16G16B16A16_UNORM;
				case Formats::RGBA32:
					return DXGI_FORMAT_R32G32B32A32_UINT;
				case Formats::RGBA16F:
					return DXGI_FORMAT_R16G16B16A16_FLOAT;
				case Formats::RGBA32F:
					return DXGI_FORMAT_R32G32B32A32_FLOAT;
				case Formats::Depth16:
					return DXGI_FORMAT_D16_UNORM;
				case Formats::Depth24:
					return DXGI_FORMAT_D24_UNORM_S8_UINT;
				case Formats::Depth32:
					return DXGI_FORMAT_D32_FLOAT;
				case Formats::Depth32F:
					return DXGI_FORMAT_D32_FLOAT;
				case Formats::DepthStencil24F:
					return DXGI_FORMAT_D24_UNORM_S8_UINT;
				case Formats::DepthStencil32F:
					return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
				}

				return DXGI_FORMAT_UNKNOWN;
			}

			DXGI_FORMAT GetShaderResourceViewFormat(Formats Format)
			{
				switch (Format)
				{
				case Formats::R8:
					return DXGI_FORMAT_R8_UNORM;
				case Formats::R16:
					return DXGI_FORMAT_R16_UNORM;
				case Formats::R32:
					return DXGI_FORMAT_R32_UINT;
				case Formats::R16F:
					return DXGI_FORMAT_R16_FLOAT;
				case Formats::R32F:
					return DXGI_FORMAT_R32_FLOAT;
				case Formats::RG8:
					return DXGI_FORMAT_R8G8_UNORM;
				case Formats::RG16:
					return DXGI_FORMAT_R16G16_UNORM;
				case Formats::RG32:
					return DXGI_FORMAT_R32G32_UINT;
				case Formats::RG16F:
					return DXGI_FORMAT_R16G16_FLOAT;
				case Formats::RG32F:
					return DXGI_FORMAT_R32G32_FLOAT;
				case Formats::RGB8:
					return DXGI_FORMAT_R8G8B8A8_UNORM;
				case Formats::RGB16:
					return DXGI_FORMAT_R16G16B16A16_UNORM;
				case Formats::RGB32:
					return DXGI_FORMAT_R32G32B32A32_UINT;
				case Formats::RGB16F:
					return DXGI_FORMAT_R16G16B16A16_FLOAT;
				case Formats::RGB32F:
					return DXGI_FORMAT_R32G32B32A32_FLOAT;
				case Formats::RGBA8:
					return DXGI_FORMAT_R8G8B8A8_UNORM;
				case Formats::RGBA16:
					return DXGI_FORMAT_R16G16B16A16_UNORM;
				case Formats::RGBA32:
					return DXGI_FORMAT_R32G32B32A32_UINT;
				case Formats::RGBA16F:
					return DXGI_FORMAT_R16G16B16A16_FLOAT;
				case Formats::RGBA32F:
					return DXGI_FORMAT_R32G32B32A32_FLOAT;
				case Formats::Depth16:
					return DXGI_FORMAT_R16_UNORM;
				case Formats::Depth24:
					return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				case Formats::Depth32:
					return DXGI_FORMAT_R32_UINT;
				case Formats::Depth32F:
					return DXGI_FORMAT_R32_FLOAT;
				case Formats::DepthStencil24F:
					return DXGI_FORMAT_D24_UNORM_S8_UINT;
				case Formats::DepthStencil32F:
					return DXGI_FORMAT_R32G32_UINT;
				}

				return DXGI_FORMAT_UNKNOWN;
			}

			uint8 GetTextureFormatPadding(Formats Format)
			{
				switch (Format)
				{
				case Formats::R8:
				case Formats::R16:
				case Formats::R16F:
				case Formats::Depth16:
				case Formats::Depth24:
				case Formats::DepthStencil24F:
				case Formats::R32:
				case Formats::R32F:
				case Formats::Depth32:
				case Formats::Depth32F:
				case Formats::DepthStencil32F:
				case Formats::RG8:
				case Formats::RG16:
				case Formats::RG16F:
				case Formats::RG32:
				case Formats::RG32F:
					return 0;

				case Formats::RGB8:
				case Formats::RGB16:
				case Formats::RGB16F:
				case Formats::RGB32:
				case Formats::RGB32F:
					return Helper::GetTextureChannelSize(Format);
				}

				return 0;
			}

			D3D12_FILTER GetMinifyFilter(TextureMinifyFilters Filter)
			{
				switch (Filter)
				{
				case TextureMinifyFilters::Nearest:
					return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;

				case TextureMinifyFilters::Linear:
					return D3D12_FILTER_MIN_MAG_MIP_LINEAR;

				case TextureMinifyFilters::NearestMipMapNearest:
					return D3D12_FILTER_MIN_MAG_MIP_LINEAR;

				case TextureMinifyFilters::LinearMipMapNearest:
					return D3D12_FILTER_MIN_MAG_MIP_LINEAR;

				case TextureMinifyFilters::NearestMipMapLinear:
					return D3D12_FILTER_MIN_MAG_MIP_LINEAR;

				case TextureMinifyFilters::LinearMipMapLinear:
					return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
				}
			}

			D3D12_FILTER GetMagnifyFilter(TextureMagnfyFilters Filter)
			{
				switch (Filter)
				{
				case TextureMagnfyFilters::Nearest:
					return D3D12_FILTER_MIN_MAG_MIP_LINEAR;

				case TextureMagnfyFilters::Linear:
					return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
				}
			}

			D3D12_TEXTURE_ADDRESS_MODE GetWrapMode(TextureWrapModes Mode)
			{
				switch (Mode)
				{
				case TextureWrapModes::Clamp:
					return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

				case TextureWrapModes::Repeat:
					return D3D12_TEXTURE_ADDRESS_MODE_BORDER;

				case TextureWrapModes::ClampToEdge:
					return D3D12_TEXTURE_ADDRESS_MODE_WRAP;

				case TextureWrapModes::MirroredRepeat:
					return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
				}
			}

			DirectX12Device::DirectX12Device(void) :
				m_Initialized(false),
				m_Factory(nullptr),
				m_Adapter(nullptr),
				m_AdapterDesc({}),
				m_Device(nullptr),
				m_UploadBuffer({}),
				m_CurrentContextHandle(0),
				m_CurrentContext(nullptr),
				m_InputLayout(nullptr),
				m_InputLayoutCount(0),
				m_CommandBufferPool(this),
				m_AsyncCommandBuffers(RenderSystemAllocators::ContainersAllocator)
			{
			}

			DirectX12Device::~DirectX12Device(void)
			{
				if (!m_Initialized)
					return;

				Uninitialize();
			}

			bool DirectX12Device::Initialize(void)
			{
				CoreDebugAssert(Categories::RenderSystem, !m_Initialized, "DirectX12Device already initialized");

#ifdef DEBUG_MODE
				if (!DirectX12Wrapper::Debugging::EnableDebugLayer())
					return false;

				if (!DirectX12Wrapper::Debugging::EnableValidationLayer())
					return false;

				if (!DirectX12Wrapper::Initialization::CreateFactory(true, &m_Factory))
					return false;
#else
				if (!DirectX12Wrapper::Initialization::CreateFactory(false, &m_Factory))
					return false;
#endif

				if (!DirectX12Wrapper::Initialization::FindBestAdapter(m_Factory, &m_Adapter, &m_AdapterDesc))
					return false;

				if (!DirectX12Wrapper::Initialization::CreateDevice(m_Adapter, &m_Device))
					return false;

#ifdef DEBUG_MODE
				if (!DirectX12DebugInfo::Create(RenderSystemAllocators::ContainersAllocator)->Initialize(this))
					return false;
#endif

				if (!CHECK_CALL(m_BufferHeapAllocator.Initialize(m_Device)))
					return false;

				if (!CHECK_CALL(m_TextureHeapAllocator.Initialize(m_Device)))
					return false;

				if (!CHECK_CALL(m_RenderTargetHeapAllocator.Initialize(m_Device)))
					return false;

				if (!CHECK_CALL(m_RenderTargetViewAllocator.Initialize(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE)))
					return false;

				if (!CHECK_CALL(m_DepthStencilViewAllocator.Initialize(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE)))
					return false;

				if (!CHECK_CALL(m_ResourceViewAllocator.Initialize(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)))
					return false;

				if (!CHECK_CALL(m_SamplerViewAllocator.Initialize(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)))
					return false;

				if (!CreateIntermediateBuffer(UPLAOD_BUFFER_SIZE, &m_UploadBuffer))
					return false;

				m_InputLayoutCount = SubMeshInfo::GetLayoutCount();
				m_InputLayout = RenderSystemAllocators::ResourceAllocator_AllocateArray<D3D12_INPUT_ELEMENT_DESC>(m_InputLayoutCount);

				VertexLayouts layout = VertexLayouts::Position;
				for (uint8 i = 0; i < m_InputLayoutCount; ++i)
				{
					D3D12_INPUT_ELEMENT_DESC& desc = m_InputLayout[i];

					desc.SemanticName = SubMeshInfo::GetLayoutName(layout);
					desc.SemanticIndex = 0;
					desc.Format = GetFormat(SubMeshInfo::GetLayoutFormat(layout));
					desc.InputSlot = 0;
					desc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
					desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
					desc.InstanceDataStepRate = 0;

					layout = (VertexLayouts)((int32)layout << 1);
				}

				m_Initialized = true;

				return true;
			}

			bool DirectX12Device::Uninitialize(void)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Initialized, "DirectX12Device already initialized");

				RenderSystemAllocators::ResourceAllocator_Deallocate(m_InputLayout);

				m_BufferHeapAllocator.Deallocate(m_UploadBuffer.Resource);

				m_SamplerViewAllocator.Deinitialize();
				m_ResourceViewAllocator.Deinitialize();
				m_DepthStencilViewAllocator.Deinitialize();
				m_RenderTargetViewAllocator.Deinitialize();

				m_RenderTargetHeapAllocator.Deinitialize();
				m_TextureHeapAllocator.Deinitialize();
				m_BufferHeapAllocator.Deinitialize();

#ifdef DEBUG_MODE
				if (!DirectX12DebugInfo::GetInstance()->Deinitialize())
					return false;

				DirectX12DebugInfo::Destroy();
#endif

				if (!DirectX12Wrapper::DestroyInstance(m_Device))
					return false;

				if (!DirectX12Wrapper::DestroyInstance(m_Factory))
					return false;

				m_Initialized = false;

				return true;
			}

			cstr DirectX12Device::GetVersion(void)
			{
				return "12";
			}

			cstr DirectX12Device::GetVendorName(void)
			{
				static String value = WString(m_AdapterDesc.Description).ChangeType<char8>();

				return value.GetValue();
			}

			cstr DirectX12Device::GetRendererName(void)
			{
				return "DirectX";
			}

			cstr DirectX12Device::GetShadingLanguageVersion(void)
			{
				return "5.1";
			}

			bool DirectX12Device::CreateContext(PlatformWindow::WindowHandle WindowHandle, ResourceHandle& Handle)
			{
				if (WindowHandle == 0)
					return false;

				for (auto& item : m_Contexts)
					if (item.GetFirst() == (ResourceHandle)WindowHandle)
						return false;

				ID3D12CommandQueue* queue = nullptr;
				if (!CHECK_CALL(DirectX12Wrapper::Command::CreateCommandQueue(m_Device, D3D12_COMMAND_LIST_TYPE_DIRECT, &queue)))
					return false;

				IDXGISwapChain4* swapChain = nullptr;
				if (!CHECK_CALL(DirectX12Wrapper::SwapChain::Create(m_Factory, queue, BACK_BUFFER_COUNT, WindowHandle, &swapChain)))
					return false;

				RenderContextInfo* info = RenderSystemAllocators::ResourceAllocator_Allocate<RenderContextInfo>();
				PlatformMemory::Set(info, 0, 1);

				info->Queue = queue;
				info->SwapChain = swapChain;
				info->BackBufferCount = BACK_BUFFER_COUNT;

				Handle = (ResourceHandle)WindowHandle;

				m_Contexts[Handle] = info;

				return true;
			}

			bool DirectX12Device::DestroyContext(ResourceHandle Handle)
			{
				if (Handle == 0)
					return true;

				if (!m_Contexts.Contains(Handle))
					return false;

				RenderContextInfo* info = m_Contexts[Handle];

				if (m_CurrentContextHandle == Handle)
					SetContext(0);

				if (!WaitForGPU(m_RenderCommandSet))
					return false;

				if (!DestroySwapChainBuffers(info))
					return false;

				if (!CHECK_CALL(DirectX12Wrapper::DestroyInstance(info->SwapChain)))
					return false;

				if (!CHECK_CALL(DirectX12Wrapper::DestroyInstance(info->Queue)))
					return false;

				RenderSystemAllocators::ResourceAllocator_Deallocate(info);

				m_Contexts.Remove(Handle);

				return true;
			}

			bool DirectX12Device::SetContext(ResourceHandle Handle)
			{
				if (m_CurrentContextHandle == Handle)
					return true;

				if (Handle == 0)
				{
					m_CurrentContextHandle = 0;
					m_CurrentContext = nullptr;

					//m_CurrentRenderTargetViewCount = 0;
					//m_CurrentDepthStencilView = nullptr;

					return true;
				}

				if (!m_Contexts.Contains(Handle))
					return false;

				RenderContextInfo* info = m_Contexts[Handle];

				m_CurrentContextHandle = Handle;
				m_CurrentContext = info;

				//if (m_CurrentRenderTarget == nullptr)
				//	BindRenderTarget(0);

				//ResetState();

				return true;
			}

			bool DirectX12Device::SetContextSize(const Vector2I& Size)
			{
				if (m_CurrentContext == nullptr)
					return false;

				if (m_CurrentContext->Initialized && m_CurrentContext->Size == Size)
					return true;

				if (!WaitForGPU(m_RenderCommandSet))
					return false;

				if (m_CurrentContext->Initialized && !DestroySwapChainBuffers(m_CurrentContext))
					return false;

				m_CurrentContext->Size = Size;

				if (!CHECK_CALL(DirectX12Wrapper::SwapChain::Resize(m_CurrentContext->SwapChain, m_CurrentContext->BackBufferCount, m_CurrentContext->Size.X, m_CurrentContext->Size.Y)))
					return false;

				ID3D12Resource1* backBuffers[BACK_BUFFER_COUNT];
				if (!CHECK_CALL(DirectX12Wrapper::SwapChain::GetBuffers(m_CurrentContext->SwapChain, m_CurrentContext->BackBufferCount, backBuffers)))
					return false;

				const D3D12_RESOURCE_STATES depthStencilBufferState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
				const DXGI_FORMAT depthStencilFormat = GetTextureFormat(Formats::Depth24);
				const D3D12_RESOURCE_DIMENSION dimension = GetTextureType(TextureTypes::TwoD);

				static int index = 0;

				for (uint8 i = 0; i < m_CurrentContext->BackBufferCount; ++i)
				{
					ViewInfo& renderTargetView = m_CurrentContext->Views[i][RenderContextInfo::RENDER_TARGET_VIEW_INDEX];
					INITIALIZE_RESOURCE_INFO(&renderTargetView, D3D12_RESOURCE_STATE_PRESENT);
					renderTargetView.Resource.Resource = backBuffers[i];

					renderTargetView.Point = AttachmentPoints::Color0;
					renderTargetView.Format = renderTargetView.Resource.Resource->GetDesc().Format;
					if (!CHECK_CALL(m_RenderTargetViewAllocator.AllocateRenderTargetView(renderTargetView.Resource.Resource, &renderTargetView.TargetView)))
						return false;

					ViewInfo& depthStencilView = m_CurrentContext->Views[i][RenderContextInfo::DEPTH_STENCIL_VIEW_INDEX];
					INITIALIZE_RESOURCE_INFO(&depthStencilView, depthStencilBufferState);
					if (!CHECK_CALL(m_RenderTargetHeapAllocator.Allocate(m_CurrentContext->Size.X, m_CurrentContext->Size.Y, depthStencilFormat, false, depthStencilBufferState, false, &depthStencilView.Resource)))
						return false;

					depthStencilView.Point = AttachmentPoints::DepthStencil;
					depthStencilView.Format = depthStencilFormat;
					if (!CHECK_CALL(m_DepthStencilViewAllocator.AllocateDepthStencilView(depthStencilView.Resource.Resource, depthStencilFormat, D3D12_DSV_FLAG_NONE, &depthStencilView.TargetView)))
						return false;
				}

				m_CurrentContext->CurrentBackBufferIndex = 0;
				m_CurrentContext->Initialized = true;

				SKIP_NEXT_FRAMES();

				return true;
			}

			bool DirectX12Device::SetResourceName(ResourceHandle Handle, ResourceTypes Type, cwstr Name)
			{
				if (Type == ResourceTypes::Buffer)
				{
					ResourceInfo* resourceInfo = ReinterpretCast(ResourceInfo*, Handle);

					if (!CHECK_CALL(DirectX12Wrapper::Debugging::SetObjectName(resourceInfo->Resource.Resource, Name)))
						return false;
				}
				else if (Type == ResourceTypes::Program)
				{
					ProgramInfos* programInfos = ReinterpretCast(ProgramInfos*, Handle);

					WString tempName(Name);

					if (!CHECK_CALL(DirectX12Wrapper::Debugging::SetObjectName(programInfos->RootSignature, (tempName + L"_RootSignature").GetValue())))
						return false;

					for (auto& item : programInfos->Pipelines)
						if (!CHECK_CALL(DirectX12Wrapper::Debugging::SetObjectName(item.GetSecond(), (tempName + L"_Pipeline").GetValue())))
							return false;
				}
				else if (Type == ResourceTypes::Mesh)
				{
					MeshBufferInfo* meshBufferInfo = ReinterpretCast(MeshBufferInfo*, Handle);

					WString tempName(Name);

					if (!CHECK_CALL(DirectX12Wrapper::Debugging::SetObjectName(meshBufferInfo->VertexBuffer.Resource.Resource, (tempName + L"_VertexBuffer").GetValue())))
						return false;

					if (meshBufferInfo->IndexBuffer.Resource.Resource != nullptr)
						if (!CHECK_CALL(DirectX12Wrapper::Debugging::SetObjectName(meshBufferInfo->IndexBuffer.Resource.Resource, (tempName + L"_IndexBuffer").GetValue())))
							return false;
				}
				else if (Type == ResourceTypes::Texture)
				{
					TextureResourceInfo* textureResourceInfo = ReinterpretCast(TextureResourceInfo*, Handle);

					if (!CHECK_CALL(DirectX12Wrapper::Debugging::SetObjectName(textureResourceInfo->Resource.Resource, Name)))
						return false;
				}
				else if (Type == ResourceTypes::RenderTarget)
				{
					RenderTargetInfos* renderTargetInfos = ReinterpretCast(RenderTargetInfos*, Handle);

					WString tempName(Name);

					uint8 index = 0;
					for (auto view : renderTargetInfos->Views)
						if (!CHECK_CALL(DirectX12Wrapper::Debugging::SetObjectName(view.Resource.Resource, (tempName + L"_TextureBuffer_" + StringUtility::ToString<char16>(index++)).GetValue())))
							return false;
				}

				return true;
			}

			bool DirectX12Device::CreateBuffer(ResourceHandle& Handle)
			{
				BoundBuffersInfo* info = RenderSystemAllocators::ResourceAllocator_Allocate<BoundBuffersInfo>();
				INITIALIZE_RESOURCE_INFO(&info->Buffer, D3D12_RESOURCE_STATE_COMMON);
				info->Resource = nullptr;
				info->Buffer.Size = 0;
				info->Buffer.Stride = 0;

				Handle = (ResourceHandle)info;

				return true;
			}

			bool DirectX12Device::DestroyBuffer(ResourceHandle Handle)
			{
				if (Handle == 0)
					return false;

				if (!WaitForGPU())
					return false;

				BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

				if (boundBufferInfo->Buffer.Resource.Resource != nullptr)
					if (!CHECK_CALL(m_BufferHeapAllocator.Deallocate(boundBufferInfo->Buffer.Resource)))
						return false;

				RenderSystemAllocators::ResourceAllocator_Deallocate(boundBufferInfo);

				return true;
			}

			bool DirectX12Device::LockBuffer(ResourceHandle Handle, GPUBufferTypes Type, GPUBufferAccess Access, byte** Buffer)
			{
				if (Handle == 0)
					return false;

				BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

				if (Type != GPUBufferTypes::Constant)
				{
					if (boundBufferInfo->Resource == nullptr)
						return false;

					if (!CopyBuffer(Type, boundBufferInfo->Resource, false, &boundBufferInfo->Buffer, true))
						return false;
				}

				return CHECK_CALL(DirectX12Wrapper::Resource::Map(boundBufferInfo->Buffer.Resource.Resource, Buffer));
			}

			bool DirectX12Device::UnlockBuffer(ResourceHandle Handle, GPUBufferTypes Type)
			{
				if (Handle == 0)
					return false;

				BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

				CHECK_CALL(DirectX12Wrapper::Resource::Unmap(boundBufferInfo->Buffer.Resource.Resource));

				if (Type == GPUBufferTypes::Constant)
					return true;

				return CopyBuffer(Type, &boundBufferInfo->Buffer, true, boundBufferInfo->Resource, false);
			}

			bool DirectX12Device::InitializeConstantBuffer(ResourceHandle Handle, const byte* Data, uint32 Size)
			{
				if (Handle == 0)
					return false;

				BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

				if (!CreateIntermediateBuffer(Size, &boundBufferInfo->Buffer))
					return false;

				byte* buffer = nullptr;
				CHECK_CALL(DirectX12Wrapper::Resource::Map(boundBufferInfo->Buffer.Resource.Resource, &buffer));

				PlatformMemory::Copy(Data, buffer, Size);

				return CHECK_CALL(DirectX12Wrapper::Resource::Unmap(boundBufferInfo->Buffer.Resource.Resource));

				return true;
			}

			bool DirectX12Device::CreateProgram(const CompiledShaders* Shaders, ResourceHandle& Handle, cstr* ErrorMessage)
			{
#define IMPLEMENT(StageName) \
					if (Shaders->StageName.Size != 0) \
					{ \
						programInfos->StageName.Buffer = RenderSystemAllocators::ResourceAllocator_AllocateArray<byte>(Shaders->StageName.Size); \
						PlatformMemory::Copy(Shaders->StageName.Buffer, programInfos->StageName.Buffer, Shaders->StageName.Size); \
						programInfos->StageName.Size = Shaders->StageName.Size; \
					}

				ID3DBlob* serializedRootSignature = nullptr;
				if (!CHECK_CALL(DirectX12Wrapper::Shader::GetInlineRootSignature(Shaders->VertexShader.Buffer, Shaders->VertexShader.Size, &serializedRootSignature)))
					return false;

				ID3D12RootSignature* rootSignature = nullptr;
				if (!CHECK_CALL(DirectX12Wrapper::RootSignature::Create(m_Device, serializedRootSignature, &rootSignature)))
					return false;

				ProgramInfos* programInfos = RenderSystemAllocators::ResourceAllocator_Allocate<ProgramInfos>();
				PlatformMemory::Set(programInfos, 0, 1);

				programInfos->RootSignature = rootSignature;

				IMPLEMENT(VertexShader);
				IMPLEMENT(TessellationShader);
				IMPLEMENT(GeometryShader);
				IMPLEMENT(FragmentShader);
				IMPLEMENT(ComputeShader);

				Handle = (ResourceHandle)programInfos;

				return true;
#undef IMPLEMENT
			}

			bool DirectX12Device::DestroyProgram(ResourceHandle Handle)
			{
#define IMPLEMENT(StageName) \
					if (programInfos->StageName.Buffer != nullptr) \
						RenderSystemAllocators::ResourceAllocator_Deallocate(programInfos->StageName.Buffer); \

				if (Handle == 0)
					return false;

				if (!WaitForGPU(m_RenderCommandSet))
					return false;

				ProgramInfos* programInfos = ReinterpretCast(ProgramInfos*, Handle);

				IMPLEMENT(VertexShader);
				IMPLEMENT(TessellationShader);
				IMPLEMENT(GeometryShader);
				IMPLEMENT(FragmentShader);
				IMPLEMENT(ComputeShader);

				for (auto& item : programInfos->Pipelines)
					if (!CHECK_CALL(DirectX12Wrapper::DestroyInstance(item.GetSecond())))
						return false;

				RenderSystemAllocators::ResourceAllocator_Deallocate(programInfos);

				return true;
#undef IMPLEMENT
			}

			bool DirectX12Device::CreateTexture(const TextureInfo* Info, ResourceHandle& Handle)
			{
				D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

				DXGI_FORMAT format = GetTextureFormat(Info->Format);
				D3D12_RESOURCE_DIMENSION dimension = GetTextureType(Info->Type);

				TextureResourceInfo* info = RenderSystemAllocators::ResourceAllocator_Allocate<TextureResourceInfo>();
				INITIALIZE_RESOURCE_INFO(info, state);

				if (Info->Type == TextureTypes::TwoD)
				{
					if (!CHECK_CALL(m_TextureHeapAllocator.Allocate(Info->Dimension.X, Info->Dimension.Y, format, dimension, state, false, &info->Resource)))
						return false;
				}
				else
					return false;


				if (!AllocateSampler(info))
					return false;

				if (Info->Data != nullptr)
				{
					BEGIN_UPLOAD();

					uint32 dataPitch = Helper::GetTextureRowPitch(Info->Format, Info->Dimension.X);
					uint8 pixelSize = Helper::GetTexturePixelSize(Info->Format);

					uint32 resourcePitch = DirectX12Wrapper::Support::GetResourceRowPitch(m_Device, info->Resource.Resource);
					uint8 padding = GetTextureFormatPadding(Info->Format);

					for (int32 y = 0; y < Info->Dimension.Y; ++y)
						for (int32 x = 0; x < Info->Dimension.X; ++x)
							PlatformMemory::Copy(Info->Data + (y * dataPitch) + (x * pixelSize), buffer + (y * resourcePitch) + (x * (pixelSize + padding)), pixelSize);

					END_UPLOAD(GPUBufferTypes::Pixel, info, false);
				}

				if (!CHECK_CALL(m_ResourceViewAllocator.AllocateTextureShaderResourceView(info->Resource.Resource, format, dimension, &info->View)))
					return false;

				Handle = (ResourceHandle)info;

				return true;
			}

			bool DirectX12Device::DestroyTexture(ResourceHandle Handle)
			{
				if (Handle == 0)
					return false;

				if (!WaitForGPU())
					return false;

				TextureResourceInfo* textureResourceInfo = ReinterpretCast(TextureResourceInfo*, Handle);

				if (!CHECK_CALL(m_ResourceViewAllocator.DeallocateView(textureResourceInfo->View)))
					return false;

				if (!CHECK_CALL(m_TextureHeapAllocator.Deallocate(textureResourceInfo->Resource)))
					return false;

				RenderSystemAllocators::ResourceAllocator_Deallocate(textureResourceInfo);

				return true;
			}

			bool DirectX12Device::SetTextureVerticalWrapping(ResourceHandle Handle, TextureTypes Type, TextureWrapModes Mode)
			{
				if (Handle == 0)
					return false;

				TextureResourceInfo* textureResourceInfo = ReinterpretCast(TextureResourceInfo*, Handle);
				textureResourceInfo->SamplerDescription.AddressV = GetWrapMode(Mode);

				REALLOCATE_SAMPLER(textureResourceInfo);
			}

			bool DirectX12Device::SetTextureHorizontalWrapping(ResourceHandle Handle, TextureTypes Type, TextureWrapModes Mode)
			{
				if (Handle == 0)
					return false;

				TextureResourceInfo* textureResourceInfo = ReinterpretCast(TextureResourceInfo*, Handle);
				textureResourceInfo->SamplerDescription.AddressU = GetWrapMode(Mode);

				REALLOCATE_SAMPLER(textureResourceInfo);
			}

			bool DirectX12Device::SetTextureMinifyFilter(ResourceHandle Handle, TextureTypes Type, TextureMinifyFilters Filter)
			{
				if (Handle == 0)
					return false;

				TextureResourceInfo* textureResourceInfo = ReinterpretCast(TextureResourceInfo*, Handle);
				textureResourceInfo->SamplerDescription.Filter = D3D12_ENCODE_BASIC_FILTER(GetMinifyFilter(Filter), D3D12_DECODE_MAG_FILTER(textureResourceInfo->SamplerDescription.Filter), D3D12_FILTER_MIN_MAG_MIP_LINEAR, 0);

				REALLOCATE_SAMPLER(textureResourceInfo);
			}

			bool DirectX12Device::SetTextureMagnifyFilter(ResourceHandle Handle, TextureTypes Type, TextureMagnfyFilters Filter)
			{
				if (Handle == 0)
					return false;

				TextureResourceInfo* textureResourceInfo = ReinterpretCast(TextureResourceInfo*, Handle);
				textureResourceInfo->SamplerDescription.Filter = D3D12_ENCODE_BASIC_FILTER(D3D12_DECODE_MIN_FILTER(textureResourceInfo->SamplerDescription.Filter), GetMagnifyFilter(Filter), D3D12_FILTER_MIN_MAG_MIP_LINEAR, 0);

				REALLOCATE_SAMPLER(textureResourceInfo);
			}

			bool DirectX12Device::GenerateTextureMipMap(ResourceHandle Handle, TextureTypes Type)
			{
				return true;
			}

			bool DirectX12Device::CreateRenderTarget(const RenderTargetInfo* Info, ResourceHandle& Handle, TextureList& Textures)
			{
#define CREATE_VIEW(IsColored, CurrnetState) \
					{ \
						for (const auto & textureInfo : Info->Textures) \
						{ \
							if (!Helper::IsColorPoint(textureInfo.Point) == IsColored) \
								continue; \
							DXGI_FORMAT format = GetTextureFormat(textureInfo.Format); \
							ViewInfo* view = &renderTargetInfos->Views[index++]; \
							INITIALIZE_RESOURCE_INFO(view, CurrnetState); \
							if (!CHECK_CALL(m_RenderTargetHeapAllocator.Allocate(textureInfo.Dimension.X, textureInfo.Dimension.Y, format, IsColored, CurrnetState, false, &view->Resource))) \
								return false; \
							if (!AllocateSampler(view)) \
								return false; \
							view->Point = textureInfo.Point; \
							view->Format = format; \
							if (!CHECK_CALL(m_ResourceViewAllocator.AllocateTextureShaderResourceView(view->Resource.Resource, GetShaderResourceViewFormat(textureInfo.Format), D3D12_RESOURCE_DIMENSION_TEXTURE2D, &view->View))) \
								return false; \
							if (IsColored) \
							{ \
								if (!CHECK_CALL(m_RenderTargetViewAllocator.AllocateRenderTargetView(view->Resource.Resource, &view->TargetView))) \
									return false; \
							} \
							else if (!CHECK_CALL(m_DepthStencilViewAllocator.AllocateDepthStencilView(view->Resource.Resource, format, D3D12_DSV_FLAG_NONE, &view->TargetView))) \
									return false; \
							Textures.Add((ResourceHandle)ReinterpretCast(TextureResourceInfo*, view)); \
						} \
					}

				if (Info->Textures.GetSize() == 0)
					return false;

				RenderTargetInfos* renderTargetInfos = RenderSystemAllocators::ResourceAllocator_Allocate<RenderTargetInfos>();
				PlatformMemory::Set(renderTargetInfos, 0, 1);

				renderTargetInfos->Views.Extend(Info->Textures.GetSize());

				uint8 index = 0;
				CREATE_VIEW(true, D3D12_RESOURCE_STATE_COMMON);
				CREATE_VIEW(false, D3D12_RESOURCE_STATE_DEPTH_WRITE);

				Handle = (ResourceHandle)renderTargetInfos;

				return true;

#undef CREATE_VIEW
			}

			bool DirectX12Device::DestroyRenderTarget(ResourceHandle Handle)
			{
				if (Handle == 0)
					return false;

				if (!WaitForGPU())
					return false;

				RenderTargetInfos* renderTargetInfos = ReinterpretCast(RenderTargetInfos*, Handle);

				for (auto& viewInfo : renderTargetInfos->Views)
				{
					if (Helper::IsColorPoint(viewInfo.Point))
					{
						if (!CHECK_CALL(m_RenderTargetViewAllocator.DeallocateView(viewInfo.TargetView)))
							return false;
					}
					else if (!CHECK_CALL(m_DepthStencilViewAllocator.DeallocateView(viewInfo.TargetView)))
						return false;

					if (!CHECK_CALL(m_ResourceViewAllocator.DeallocateView(viewInfo.View)))
						return false;

					if (!CHECK_CALL(m_SamplerViewAllocator.DeallocateView(viewInfo.SamplerView)))
						return false;

					if (!CHECK_CALL(m_RenderTargetHeapAllocator.Deallocate(viewInfo.Resource)))
						return false;
				}

				RenderSystemAllocators::ResourceAllocator_Deallocate(renderTargetInfos);

				return true;
			}

			bool DirectX12Device::CreateMesh(const SubMeshInfo* Info, ResourceHandle& Handle)
			{
				if (Info->Vertices.GetSize() == 0)
					return false;

				D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

				MeshBufferInfo* info = RenderSystemAllocators::ResourceAllocator_Allocate<MeshBufferInfo>();
				INITIALIZE_RESOURCE_INFO(&info->VertexBuffer, state);

				uint32 bufferSize = Helper::GetMeshVertexBufferSize(Info->Vertices.GetSize());
				if (!CHECK_CALL(m_BufferHeapAllocator.Allocate(bufferSize, state, false, &info->VertexBuffer.Resource)))
					return false;

				info->VertexBuffer.Size = bufferSize;
				info->VertexBuffer.Stride = Helper::GetMeshVertexSize();

				{
					BEGIN_UPLOAD();

					PlatformMemory::Copy(ReinterpretCast(const byte*, Info->Vertices.GetData()), buffer, bufferSize);

					END_UPLOAD(GPUBufferTypes::Vertex, &info->VertexBuffer, true);
				}

				INITIALIZE_RESOURCE_INFO(&info->IndexBuffer, state);

				if (Info->Indices.GetSize() != 0)
				{

					bufferSize = Helper::GetMeshIndexBufferSize(Info->Indices.GetSize());
					if (!CHECK_CALL(m_BufferHeapAllocator.Allocate(bufferSize, state, false, &info->IndexBuffer.Resource)))
						return false;

					info->IndexBuffer.Size = bufferSize;
					info->IndexBuffer.Stride = Helper::GetMeshIndexSize();

					{
						BEGIN_UPLOAD();

						PlatformMemory::Copy(ReinterpretCast(const byte*, Info->Indices.GetData()), buffer, bufferSize);

						END_UPLOAD(GPUBufferTypes::Index, &info->IndexBuffer, true);
					}
				}

				Handle = (ResourceHandle)info;

				return true;
			}

			bool DirectX12Device::DestroyMesh(ResourceHandle Handle)
			{
				if (Handle == 0)
					return false;

				if (!WaitForGPU())
					return false;

				MeshBufferInfo* meshBufferInfo = ReinterpretCast(MeshBufferInfo*, Handle);

				if (!CHECK_CALL(m_BufferHeapAllocator.Deallocate(meshBufferInfo->VertexBuffer.Resource)))
					return false;

				if (meshBufferInfo->IndexBuffer.Resource.Resource != nullptr && !CHECK_CALL(m_BufferHeapAllocator.Deallocate(meshBufferInfo->IndexBuffer.Resource)))
					return false;

				RenderSystemAllocators::ResourceAllocator_Deallocate(meshBufferInfo);

				return true;
			}

			bool DirectX12Device::CreateCommandBuffer(ICommandBuffer::Types Type, ICommandBuffer*& Buffer)
			{
				Buffer = m_CommandBufferPool.Get(Type);

				return true;
			}

			bool DirectX12Device::DestroyCommandBuffer(ICommandBuffer* Buffer)
			{
				m_CommandBufferPool.Back(ReinterpretCast(DirectX12CommandBuffer*, Buffer));

				return true;
			}

			bool DirectX12Device::SubmitCommandBuffer(ICommandBuffer* const* Buffers, uint16 Count)
			{
				ICommandBuffer** buffers = ConstCast(ICommandBuffer**, Buffers);

				for (uint16 i = 0; i < Count; ++i)
				{
					ICommandBuffer* buffer = buffers[i];

					if (!buffer->Execute())
						return false;

					DestroyCommandBuffer(buffer);
				}

				return true;
			}

			bool DirectX12Device::SubmitCommandBufferAsync(ICommandBuffer* const* Buffers, uint16 Count)
			{
				//TODO: must execute inside another thread
				SubmitCommandBuffer(Buffers, Count);

				//m_AsyncCommandBuffers.Add()

				return true;
			}

			bool DirectX12Device::SwapBuffers(void)
			{
				if (m_CurrentContext == nullptr)
					return false;

				ADD_TRANSITION_STATE_FOR_TARGET_BUFFERS(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_DEPTH_WRITE);

				IDXGISwapChain4* swapChain = m_CurrentContext->SwapChain;

				if (!CHECK_CALL(DirectX12Wrapper::SwapChain::Present(swapChain, false)))
					return false;

				m_CurrentContext->CurrentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();

				//if (m_CurrentRenderTarget == nullptr)
				//	FILL_RENDER_VIEWS_USING_CONTEXT();

				return true;
			}

			bool DirectX12Device::SetDebugCallback(DebugFunction Callback)
			{
				m_DebugCallback = Callback;

				return true;
			}

			bool DirectX12Device::DestroySwapChainBuffers(RenderContextInfo* ContextInfo)
			{
				if (!ContextInfo->Initialized)
					return true;

				for (uint8 i = 0; i < ContextInfo->BackBufferCount; ++i)
				{
					ViewInfo& renderTargetView = ContextInfo->Views[i][RenderContextInfo::RENDER_TARGET_VIEW_INDEX];

					if (!DirectX12Wrapper::ReleaseInstance(renderTargetView.Resource.Resource))
						return false;

					if (!CHECK_CALL(m_RenderTargetViewAllocator.DeallocateView(renderTargetView.TargetView)))
						return false;

					ViewInfo& depthStencilView = ContextInfo->Views[i][RenderContextInfo::DEPTH_STENCIL_VIEW_INDEX];
					if (!CHECK_CALL(m_DepthStencilViewAllocator.DeallocateView(depthStencilView.TargetView)))
						return false;

					if (!CHECK_CALL(m_RenderTargetHeapAllocator.Deallocate(depthStencilView.Resource)))
						return false;
				}

				return true;
			}

			bool DirectX12Device::CreateIntermediateBuffer(uint32 Size, BufferInfo* Buffer)
			{
				if (Buffer->Size > Size)
					return true;

				D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

				if (Buffer->Resource.Resource != nullptr)
					if (!CHECK_CALL(m_BufferHeapAllocator.Deallocate(Buffer->Resource)))
						return false;

				if (!CHECK_CALL(m_BufferHeapAllocator.Allocate(Size, state, true, &Buffer->Resource)))
					return false;

				Buffer->State = state;
				Buffer->Size = Size;

				return true;
			}

			bool DirectX12Device::AllocateSampler(TextureResourceInfo* Info)
			{
				D3D12_SAMPLER_DESC& desc = Info->SamplerDescription;

				desc.Filter = D3D12_ENCODE_BASIC_FILTER(GetMinifyFilter(TextureMinifyFilters::Linear), GetMagnifyFilter(TextureMagnfyFilters::Linear), D3D12_FILTER_MIN_MAG_MIP_LINEAR, 0);
				desc.AddressU = desc.AddressV = desc.AddressW = GetWrapMode(TextureWrapModes::Clamp);
				desc.MipLODBias = 0;
				desc.MaxAnisotropy = 0;
				desc.ComparisonFunc = GetComparisonFunction(TestFunctions::Never);
				desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0;
				desc.MinLOD = 0;
				desc.MaxLOD = 0;

				return CHECK_CALL(m_SamplerViewAllocator.AllocateSampler(Info->SamplerDescription, &Info->SamplerView));
			}

#undef CHECK_CALL
#undef INITIALIZE_RESOURCE_INFO
#undef REALLOCATE_SAMPLER
#undef BEGIN_UPLOAD
#undef END_UPLOAD
#undef SKIP_NEXT_FRAMES
		}
	}
}