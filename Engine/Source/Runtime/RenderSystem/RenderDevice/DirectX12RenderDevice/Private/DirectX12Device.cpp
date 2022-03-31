// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <DirectX12RenderDevice\Private\DirectX12Device.h>
#include <DirectX12RenderDevice\Private\DirectX12DebugInfo.h>
#include <DirectX12RenderDevice\Private\DirectX12CommandBuffer.h>
#include <DirectX12RenderDevice\Private\DirectX12Fence.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <RenderCommon\Helper.h>
#include <Containers\StringUtility.h>
#include <Debugging\CoreDebug.h>
#include <WindowUtility\Window.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace Platform;
	using namespace RenderCommon::Private;
	using namespace WindowUtility;

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

#define END_UPLOAD(MainResourceInfo) \
					if (!CHECK_CALL(DirectX12Wrapper::Resource::Unmap(m_UploadBuffer.Resource.Resource))) \
						return false; \
					m_UploadCommandBuffer->Clear(); \
					m_UploadCommandBuffer->BeginEvent(L"UploadCommandBuffer"); \
					m_UploadBuffer.Type = (MainResourceInfo)->Type; \
					m_UploadCommandBuffer->CopyBuffer(ReinterpretCast(ResourceHandle, &m_UploadBuffer), ReinterpretCast(ResourceHandle, MainResourceInfo)); \
					m_UploadCommandBuffer->EndEvent(); \
					if (!m_UploadCommandBuffer->Execute()) \
						return false; \
					IFence* fence = m_FencePool.Get(); \
					m_UploadCommandBuffer->SignalFences(&fence, 1); \
					if (!fence->Wait()) \
						return false; \
					m_FencePool.Back(fence); \
				}

#define CREATE_VIEW(ViewPtr, BufferWidth, BufferHeight, AttachPoint, BufferFormat, IsColored, CurrentState, ShaderVisible) \
				{ \
					DXGI_FORMAT format = GetTextureFormat(BufferFormat); \
					INITIALIZE_RESOURCE_INFO(ViewPtr, CurrentState); \
					ViewPtr->Type = GPUBufferTypes::Pixel; \
					ViewPtr->IsIntermediate = false; \
					if (!CHECK_CALL(m_RenderTargetHeapAllocator.Allocate(BufferWidth, BufferHeight, format, IsColored, CurrentState, false, &ViewPtr->Resource))) \
						return false; \
					if (!AllocateSampler(ViewPtr)) \
						return false; \
					ViewPtr->Point = AttachPoint; \
					ViewPtr->Format = BufferFormat; \
					ViewPtr->Dimension = Vector2I(BufferWidth, BufferHeight); \
					if (ShaderVisible) \
						if (!CHECK_CALL(m_ResourceViewAllocator.AllocateTextureShaderResourceView(ViewPtr->Resource.Resource, format, D3D12_RESOURCE_DIMENSION_TEXTURE2D, &ViewPtr->View))) \
							return false; \
					if (IsColored) \
					{ \
						if (!CHECK_CALL(m_RenderTargetViewAllocator.AllocateRenderTargetView(ViewPtr->Resource.Resource, &ViewPtr->TargetView))) \
							return false; \
					} \
					else if (!CHECK_CALL(m_DepthStencilViewAllocator.AllocateDepthStencilView(ViewPtr->Resource.Resource, format, D3D12_DSV_FLAG_NONE, &ViewPtr->TargetView))) \
						return false; \
				}

#define DESTROY_VIEW(ViewPtr) \
				{ \
					if (Helper::IsColorPoint((ViewPtr)->Point)) \
					{ \
						if (!CHECK_CALL(m_RenderTargetViewAllocator.DeallocateView((ViewPtr)->TargetView))) \
							return false; \
					} \
					else if (!CHECK_CALL(m_DepthStencilViewAllocator.DeallocateView((ViewPtr)->TargetView))) \
						return false; \
					if ((ViewPtr)->View.DescriptorHeap != nullptr && !CHECK_CALL(m_ResourceViewAllocator.DeallocateView((ViewPtr)->View))) \
						return false; \
					if (!CHECK_CALL(m_SamplerViewAllocator.DeallocateView((ViewPtr)->SamplerView))) \
						return false; \
					if (!CHECK_CALL(m_RenderTargetHeapAllocator.Deallocate((ViewPtr)->Resource))) \
						return false; \
				}


			const Formats BACK_BUFFER_FORMAT = Formats::RGB8;
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

			DXGI_FORMAT GetBufferFormat(Formats Format)
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

			uint8 GetTextureFormatPadding(Formats Format)
			{
				switch (Format)
				{
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
				m_UploadCommandBuffer(nullptr),
				m_InputLayout(nullptr),
				m_InputLayoutCount(0),
				m_CurrentContext(nullptr),
				m_CommandBufferPool(this),
				m_FencePool(this)
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

				if (!CreateBufferInternal(GPUBufferTypes::Pixel, UPLAOD_BUFFER_SIZE, true, &m_UploadBuffer))
					return false;

				m_UploadCommandBuffer = m_CommandBufferPool.Get();

				m_InputLayoutCount = SubMeshInfo::GetLayoutCount();
				m_InputLayout = RenderSystemAllocators::ResourceAllocator_AllocateArray<D3D12_INPUT_ELEMENT_DESC>(m_InputLayoutCount);

				VertexLayouts layout = VertexLayouts::Position;
				for (uint8 i = 0; i < m_InputLayoutCount; ++i)
				{
					D3D12_INPUT_ELEMENT_DESC& desc = m_InputLayout[i];

					desc.SemanticName = SubMeshInfo::GetLayoutName(layout);
					desc.SemanticIndex = 0;
					desc.Format = GetBufferFormat(SubMeshInfo::GetLayoutFormat(layout));
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

				m_CommandBufferPool.Back(m_UploadCommandBuffer);

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

				DirectX12CommandBuffer* commandBuffer = m_CommandBufferPool.Get();
				commandBuffer->SetName(L"RenderContextCommandBuffer");

				IDXGISwapChain4* swapChain = nullptr;
				if (!CHECK_CALL(DirectX12Wrapper::SwapChain::Create(m_Factory, commandBuffer->GetQueue(), GetTextureFormat(BACK_BUFFER_FORMAT), BACK_BUFFER_COUNT, WindowHandle, &swapChain)))
					return false;

				RenderContextInfo* info = RenderSystemAllocators::ResourceAllocator_Allocate<RenderContextInfo>();
				PlatformMemory::Set(info, 0, 1);

				info->WindowHandle = WindowHandle;
				info->CommandBuffer = commandBuffer;
				info->SwapChain = swapChain;
				info->ViewCount = BACK_BUFFER_COUNT;

				Handle = ReinterpretCast(ResourceHandle, info);

				return true;
			}

			bool DirectX12Device::DestroyContext(ResourceHandle Handle)
			{
				if (Handle == 0)
					return true;

				RenderContextInfo* info = ReinterpretCast(RenderContextInfo*, Handle);

				if (m_CurrentContext == info)
					SetContext(0);

				if (!DestroySwapChainBuffers(info))
					return false;

				if (!CHECK_CALL(DirectX12Wrapper::DestroyInstance(info->SwapChain)))
					return false;

				m_CommandBufferPool.Back(info->CommandBuffer);

				RenderSystemAllocators::ResourceAllocator_Deallocate(info);

				return true;
			}

			bool DirectX12Device::SetContext(ResourceHandle Handle)
			{
				if (ReinterpretCast(ResourceHandle, m_CurrentContext) == Handle)
					return true;

				if (Handle == 0)
				{
					m_CurrentContext = nullptr;

					return true;
				}

				RenderContextInfo* info = ReinterpretCast(RenderContextInfo*, Handle);

				uint16 width, height;
				PlatformWindow::GetClientSize(info->WindowHandle, width, height);

				if (!info->Initialized || info->IntermediateViews[RenderContextInfo::RENDER_TARGET_VIEW_INDEX].Dimension != Vector2I(width, height))
					if (!UpdateSwapChainBufferSize(info, Vector2I(width, height)))
						return false;

				m_CurrentContext = info;

				return true;
			}

			bool DirectX12Device::SwapBuffers(void)
			{
				if (m_CurrentContext == nullptr)
					return false;

				ViewInfo& rt = m_CurrentContext->IntermediateViews[RenderContextInfo::RENDER_TARGET_VIEW_INDEX];
				ResourceHandle view = ReinterpretCast(ResourceHandle, m_CurrentContext->GetSwapChainView());

				DirectX12CommandBuffer& cb = *m_CurrentContext->CommandBuffer;
				cb.Clear();
				cb.BeginEvent(L"CopyToBackBuffer");
				cb.CopyTexture(ReinterpretCast(ResourceHandle, &rt), Vector2I::Zero, view, Vector2I::Zero, rt.Dimension);
				cb.MoveTextureToPresentState(view);
				cb.EndEvent();
				if (!cb.Execute())
					return false;

				IDXGISwapChain4* swapChain = m_CurrentContext->SwapChain;

				if (!CHECK_CALL(DirectX12Wrapper::SwapChain::Present(swapChain, false)))
					return false;

				IFence* fence = m_FencePool.Get();
				cb.SignalFences(&fence, 1);
				if (!fence->Wait())
					return false;
				m_FencePool.Back(fence);

				m_CurrentContext->CurrentViewIndex = swapChain->GetCurrentBackBufferIndex();

				return true;
			}

			bool DirectX12Device::CreateBuffer(GPUBufferTypes Type, uint32 Size, ResourceHandle& Handle)
			{
				if (Size == 0)
					return false;

				BufferInfo* info = RenderSystemAllocators::ResourceAllocator_Allocate<BufferInfo>();

				if (!CreateBufferInternal(Type, Size, true, info))
					return false;

				Handle = ReinterpretCast(ResourceHandle, info);

				return true;
			}

			bool DirectX12Device::DestroyBuffer(ResourceHandle Handle)
			{
				if (Handle == 0)
					return false;

				BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);

				if (!CHECK_CALL(m_BufferHeapAllocator.Deallocate(info->Resource)))
					return false;

				RenderSystemAllocators::ResourceAllocator_Deallocate(info);

				return true;
			}

			bool DirectX12Device::LockBuffer(ResourceHandle Handle, GPUBufferAccess Access, byte** Buffer)
			{
				if (Handle == 0)
					return false;

				if (Buffer == nullptr)
					return false;

				BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);

				return CHECK_CALL(DirectX12Wrapper::Resource::Map(info->Resource.Resource, Buffer));
			}

			bool DirectX12Device::UnlockBuffer(ResourceHandle Handle)
			{
				if (Handle == 0)
					return false;

				BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);

				return CHECK_CALL(DirectX12Wrapper::Resource::Unmap(info->Resource.Resource));
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

				Handle = ReinterpretCast(ResourceHandle, programInfos);

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

				D3D12_RESOURCE_DIMENSION dimension = GetTextureType(Info->Type);

				TextureResourceInfo* info = RenderSystemAllocators::ResourceAllocator_Allocate<TextureResourceInfo>();
				INITIALIZE_RESOURCE_INFO(info, state);
				info->Type = GPUBufferTypes::Pixel;
				info->IsIntermediate = false;
				info->Format = Info->Format;
				info->Dimension = Info->Dimension;

				DXGI_FORMAT format = GetTextureFormat(Info->Format);

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

					END_UPLOAD(info);
				}

				if (!CHECK_CALL(m_ResourceViewAllocator.AllocateTextureShaderResourceView(info->Resource.Resource, format, dimension, &info->View)))
					return false;

				Handle = ReinterpretCast(ResourceHandle, info);

				return true;
			}

			bool DirectX12Device::DestroyTexture(ResourceHandle Handle)
			{
				if (Handle == 0)
					return false;

				TextureResourceInfo* textureResourceInfo = ReinterpretCast(TextureResourceInfo*, Handle);

				if (!CHECK_CALL(m_ResourceViewAllocator.DeallocateView(textureResourceInfo->View)))
					return false;

				if (!CHECK_CALL(m_TextureHeapAllocator.Deallocate(textureResourceInfo->Resource)))
					return false;

				RenderSystemAllocators::ResourceAllocator_Deallocate(textureResourceInfo);

				return true;
			}

			bool DirectX12Device::GetTextureFootprint(ResourceHandle Handle, BufferFootprintInfo& Footprint)
			{
				if (Handle == 0)
					return false;

				TextureResourceInfo* textureResourceInfo = ReinterpretCast(TextureResourceInfo*, Handle);

				Footprint.Size = DirectX12Wrapper::Support::GetRequiredBufferSize(m_Device, D3D12_RESOURCE_DIMENSION_TEXTURE2D, textureResourceInfo->Dimension.X, textureResourceInfo->Dimension.Y, GetTextureFormat(textureResourceInfo->Format), D3D12_TEXTURE_LAYOUT_UNKNOWN);
				Footprint.RowPitch = DirectX12Wrapper::Support::GetResourceRowPitch(m_Device, textureResourceInfo->Resource.Resource);
				Footprint.ElementPadding = GetTextureFormatPadding(textureResourceInfo->Format);

				return true;
			}

			bool DirectX12Device::SetTextureVerticalWrapping(ResourceHandle Handle, TextureWrapModes Mode)
			{
				if (Handle == 0)
					return false;

				TextureResourceInfo* textureResourceInfo = ReinterpretCast(TextureResourceInfo*, Handle);
				textureResourceInfo->SamplerDescription.AddressV = GetWrapMode(Mode);

				REALLOCATE_SAMPLER(textureResourceInfo);
			}

			bool DirectX12Device::SetTextureHorizontalWrapping(ResourceHandle Handle, TextureWrapModes Mode)
			{
				if (Handle == 0)
					return false;

				TextureResourceInfo* textureResourceInfo = ReinterpretCast(TextureResourceInfo*, Handle);
				textureResourceInfo->SamplerDescription.AddressU = GetWrapMode(Mode);

				REALLOCATE_SAMPLER(textureResourceInfo);
			}

			bool DirectX12Device::SetTextureMinifyFilter(ResourceHandle Handle, TextureMinifyFilters Filter)
			{
				if (Handle == 0)
					return false;

				TextureResourceInfo* textureResourceInfo = ReinterpretCast(TextureResourceInfo*, Handle);
				textureResourceInfo->SamplerDescription.Filter = D3D12_ENCODE_BASIC_FILTER(GetMinifyFilter(Filter), D3D12_DECODE_MAG_FILTER(textureResourceInfo->SamplerDescription.Filter), D3D12_FILTER_MIN_MAG_MIP_LINEAR, 0);

				REALLOCATE_SAMPLER(textureResourceInfo);
			}

			bool DirectX12Device::SetTextureMagnifyFilter(ResourceHandle Handle, TextureMagnfyFilters Filter)
			{
				if (Handle == 0)
					return false;

				TextureResourceInfo* textureResourceInfo = ReinterpretCast(TextureResourceInfo*, Handle);
				textureResourceInfo->SamplerDescription.Filter = D3D12_ENCODE_BASIC_FILTER(D3D12_DECODE_MIN_FILTER(textureResourceInfo->SamplerDescription.Filter), GetMagnifyFilter(Filter), D3D12_FILTER_MIN_MAG_MIP_LINEAR, 0);

				REALLOCATE_SAMPLER(textureResourceInfo);
			}

			bool DirectX12Device::CreateRenderTarget(const RenderTargetInfo* Info, ResourceHandle& Handle, TextureList& Textures)
			{
#define CREATE_VIEWS(IsColored, CurrentState) \
					{ \
						for (const auto & textureInfo : Info->Textures) \
						{ \
							if (!Helper::IsColorPoint(textureInfo.Point) == IsColored) \
								continue; \
							ViewInfo* view = &renderTargetInfos->Views[index++]; \
							CREATE_VIEW(view, textureInfo.Dimension.X, textureInfo.Dimension.Y, textureInfo.Point, textureInfo.Format, IsColored, CurrentState, true); \
							Textures.Add(ReinterpretCast(ResourceHandle, ReinterpretCast(TextureResourceInfo*, view))); \
						} \
					}

				if (Info->Textures.GetSize() == 0)
					return false;

				RenderTargetInfos* renderTargetInfos = RenderSystemAllocators::ResourceAllocator_Allocate<RenderTargetInfos>();
				PlatformMemory::Set(renderTargetInfos, 0, 1);

				renderTargetInfos->Views.Extend(Info->Textures.GetSize());

				uint8 index = 0;
				CREATE_VIEWS(true, D3D12_RESOURCE_STATE_COMMON);
				CREATE_VIEWS(false, D3D12_RESOURCE_STATE_DEPTH_WRITE);

				Handle = ReinterpretCast(ResourceHandle, renderTargetInfos);

				return true;

#undef CREATE_VIEWS
			}

			bool DirectX12Device::DestroyRenderTarget(ResourceHandle Handle)
			{
				if (Handle == 0)
					return false;

				RenderTargetInfos* renderTargetInfos = ReinterpretCast(RenderTargetInfos*, Handle);

				for (auto& viewInfo : renderTargetInfos->Views)
					DESTROY_VIEW(&viewInfo);

				RenderSystemAllocators::ResourceAllocator_Deallocate(renderTargetInfos);

				return true;
			}

			bool DirectX12Device::CreateMesh(const SubMeshInfo* Info, ResourceHandle& Handle)
			{
				if (Info->Vertices.GetSize() == 0)
					return false;

				D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

				MeshBufferInfo* info = RenderSystemAllocators::ResourceAllocator_Allocate<MeshBufferInfo>();
				INITIALIZE_RESOURCE_INFO(info, state);

				uint32 bufferSize = Helper::GetMeshVertexBufferSize(Info->Vertices.GetSize());
				if (!CHECK_CALL(m_BufferHeapAllocator.Allocate(bufferSize, state, false, &info->Resource)))
					return false;

				info->Size = bufferSize;
				info->Stride = Helper::GetMeshVertexSize();
				info->Type = GPUBufferTypes::Vertex;
				info->IsIntermediate = false;

				{
					BEGIN_UPLOAD();

					PlatformMemory::Copy(ReinterpretCast(const byte*, Info->Vertices.GetData()), buffer, bufferSize);

					END_UPLOAD(info);
				}

				INITIALIZE_RESOURCE_INFO(&info->IndexBuffer, state);

				if (Info->Indices.GetSize() != 0)
				{
					bufferSize = Helper::GetMeshIndexBufferSize(Info->Indices.GetSize());
					if (!CHECK_CALL(m_BufferHeapAllocator.Allocate(bufferSize, state, false, &info->IndexBuffer.Resource)))
						return false;

					info->IndexBuffer.Size = bufferSize;
					info->IndexBuffer.Stride = Helper::GetMeshIndexSize();
					info->Type = GPUBufferTypes::Index;
					info->IsIntermediate = false;

					{
						BEGIN_UPLOAD();

						PlatformMemory::Copy(ReinterpretCast(const byte*, Info->Indices.GetData()), buffer, bufferSize);

						END_UPLOAD(&info->IndexBuffer);
					}
				}

				Handle = ReinterpretCast(ResourceHandle, info);

				return true;
			}

			bool DirectX12Device::DestroyMesh(ResourceHandle Handle)
			{
				if (Handle == 0)
					return false;

				MeshBufferInfo* meshBufferInfo = ReinterpretCast(MeshBufferInfo*, Handle);

				if (!CHECK_CALL(m_BufferHeapAllocator.Deallocate(meshBufferInfo->Resource)))
					return false;

				if (meshBufferInfo->IndexBuffer.Resource.Resource != nullptr && !CHECK_CALL(m_BufferHeapAllocator.Deallocate(meshBufferInfo->IndexBuffer.Resource)))
					return false;

				RenderSystemAllocators::ResourceAllocator_Deallocate(meshBufferInfo);

				return true;
			}

			bool DirectX12Device::CreateCommandBuffer(ICommandBuffer*& Buffer)
			{
				Buffer = m_CommandBufferPool.Get();

				return true;
			}

			bool DirectX12Device::DestroyCommandBuffers(ICommandBuffer** Buffers, uint8 Count)
			{
				for (uint8 i = 0; i < Count; ++i)
					m_CommandBufferPool.Back(Buffers[i]);

				return true;
			}

			bool DirectX12Device::SubmitCommandBuffers(ICommandBuffer* const* Buffers, uint8 Count)
			{
				DirectX12CommandBuffer** buffers = ReinterpretCast(DirectX12CommandBuffer**, ConstCast(ICommandBuffer**, Buffers));

				for (uint8 i = 0; i < Count; ++i)
					if (!buffers[i]->Execute())
						return false;

				return true;
			}

			bool DirectX12Device::CreateFence(IFence*& Fence)
			{
				Fence = m_FencePool.Get();

				return true;
			}

			bool DirectX12Device::DestroyFences(IFence** Fence, uint8 Count)
			{
				for (uint16 i = 0; i < Count; ++i)
					m_FencePool.Back(Fence[i]);

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

					programInfos->PipelineName = tempName;
				}
				else if (Type == ResourceTypes::Mesh)
				{
					MeshBufferInfo* meshBufferInfo = ReinterpretCast(MeshBufferInfo*, Handle);

					WString tempName(Name);

					if (!CHECK_CALL(DirectX12Wrapper::Debugging::SetObjectName(meshBufferInfo->Resource.Resource, (tempName + L"_Vertex").GetValue())))
						return false;

					if (meshBufferInfo->IndexBuffer.Resource.Resource != nullptr)
						if (!CHECK_CALL(DirectX12Wrapper::Debugging::SetObjectName(meshBufferInfo->IndexBuffer.Resource.Resource, (tempName + L"_Index").GetValue())))
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
						if (!CHECK_CALL(DirectX12Wrapper::Debugging::SetObjectName(view.Resource.Resource, (tempName + L"_Texture_" + StringUtility::ToString<char16>(index++)).GetValue())))
							return false;
				}

				return true;
			}

			bool DirectX12Device::CreateBufferInternal(GPUBufferTypes Type, uint32 Size, bool IsIntermediate, BufferInfo* Buffer)
			{
				D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

				if (Buffer->Resource.Resource != nullptr)
					if (!CHECK_CALL(m_BufferHeapAllocator.Deallocate(Buffer->Resource)))
						return false;

				if (!CHECK_CALL(m_BufferHeapAllocator.Allocate(Size, state, true, &Buffer->Resource)))
					return false;

				Buffer->State = state;
				Buffer->Size = Size;
				Buffer->Stride = 0;
				Buffer->Type = Type;
				Buffer->IsIntermediate = IsIntermediate;

				return true;
			}

			bool DirectX12Device::UpdateSwapChainBufferSize(RenderContextInfo* Info, const Vector2I& Size)
			{
				if (Info->Initialized)
				{
					if (!DestroySwapChainBuffers(Info))
						return false;

					for (uint8 i = 0; i < Info->ViewCount; ++i)
						DirectX12Wrapper::ReleaseInstance(Info->Views[i].Resource.Resource);
				}

				if (!CHECK_CALL(DirectX12Wrapper::SwapChain::Resize(Info->SwapChain, Info->ViewCount, Size.X, Size.Y)))
					return false;

				ID3D12Resource1* backBuffers[BACK_BUFFER_COUNT];
				if (!CHECK_CALL(DirectX12Wrapper::SwapChain::GetBuffers(Info->SwapChain, Info->ViewCount, backBuffers)))
					return false;

				for (uint8 i = 0; i < Info->ViewCount; ++i)
				{
					ViewInfo& renderTargetView = Info->Views[i];
					INITIALIZE_RESOURCE_INFO(&renderTargetView, D3D12_RESOURCE_STATE_PRESENT);
					renderTargetView.Resource.Resource = backBuffers[i];
					renderTargetView.Type = GPUBufferTypes::Pixel;
					renderTargetView.IsIntermediate = false;
					renderTargetView.Point = AttachmentPoints::Color0;
					renderTargetView.Format = BACK_BUFFER_FORMAT;
					renderTargetView.Dimension = Size;

					if (!CHECK_CALL(m_RenderTargetViewAllocator.AllocateRenderTargetView(renderTargetView.Resource.Resource, &renderTargetView.TargetView)))
						return false;
				}

				ViewInfo* view = &Info->IntermediateViews[RenderContextInfo::RENDER_TARGET_VIEW_INDEX];
				CREATE_VIEW(view, Size.X, Size.Y, AttachmentPoints::Color0, Info->Views[0].Format, true, D3D12_RESOURCE_STATE_COMMON, false);
				DirectX12Wrapper::Debugging::SetObjectName(view->Resource.Resource, L"Intermediate Render Target");

				view = &Info->IntermediateViews[RenderContextInfo::DEPTH_STENCIL_VIEW_INDEX];
				CREATE_VIEW(view, Size.X, Size.Y, AttachmentPoints::DepthStencil, Formats::DepthStencil24F, false, D3D12_RESOURCE_STATE_DEPTH_WRITE, false);
				DirectX12Wrapper::Debugging::SetObjectName(view->Resource.Resource, L"Intermediate Depth/Stencil");

				Info->CurrentViewIndex = 0;
				Info->Initialized = true;

				return true;
			}

			bool DirectX12Device::DestroySwapChainBuffers(RenderContextInfo* ContextInfo)
			{
				if (!ContextInfo->Initialized)
					return true;

				DESTROY_VIEW(&ContextInfo->IntermediateViews[RenderContextInfo::RENDER_TARGET_VIEW_INDEX]);
				DESTROY_VIEW(&ContextInfo->IntermediateViews[RenderContextInfo::DEPTH_STENCIL_VIEW_INDEX]);

				for (uint8 i = 0; i < ContextInfo->ViewCount; ++i)
					if (!CHECK_CALL(m_RenderTargetViewAllocator.DeallocateView(ContextInfo->Views[i].TargetView)))
						return false;

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
#undef CREATE_VIEW
#undef DESTROY_VIEW
		}
	}
}