// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\DirectX12\DirectX12Device.h>
#include <Rendering\DeviceInterface.h>
#include <Rendering\Private\Helper.h>
#include <Containers\StringUtility.h>
#include <Debugging\Debug.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Utility\Hash.h>
#include <pix.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace Platform;
	using namespace MemoryManagement::Allocator;
	using namespace Utility;

	namespace Rendering
	{
		namespace Private
		{
			namespace DirectX12
			{
#define CHECK_CALL(Expr) (!(!(Expr) && RaiseDebugMessages(m_InfoQueue, this)))

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

#define FILL_RENDER_VIEWS_USING_CONTEXT() \
				if (m_CurrentContext != nullptr && m_CurrentContext->Initialized) \
				{ \
					m_CurrentRenderTargetViews[0] = m_CurrentContext->GetRenderTargetViews(); \
					m_CurrentRenderTargetViewCount = 1; \
					m_CurrentDepthStencilView = m_CurrentContext->GetDepthStencilViews(); \
				}

#define ADD_TRANSITION_STATE_FOR_TARGET_BUFFERS(RenderTargetState, DepthStencilState) \
				{ \
					for (uint8 i = 0; i < m_CurrentRenderTargetViewCount; ++i) \
					{ \
						if (!AddTransitionResourceBarrier(m_RenderCommandSet, m_CurrentRenderTargetViews[i], RenderTargetState)) \
							return false; \
					} \
					if (m_CurrentDepthStencilView != nullptr) \
						if (!AddTransitionResourceBarrier(m_RenderCommandSet, m_CurrentDepthStencilView, DepthStencilState)) \
							return false; \
				}

#define SKIP_NEXT_FRAMES() m_RenderCommandSet.SkipFrameCount = 1

				const uint8 BACK_BUFFER_COUNT = 2;
				const uint32 UPLAOD_BUFFER_SIZE = 8 * MegaByte;

				D3D12_RESOURCE_DIMENSION GetTextureType(Texture::Types Type)
				{
					switch (Type)
					{
					case Texture::Types::TwoD:
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
						return Texture::GetChannelSize(Format);
					}

					return 0;
				}

				D3D12_FILTER GetMinifyFilter(Texture::MinifyFilters Filter)
				{
					switch (Filter)
					{
					case Texture::MinifyFilters::Nearest:
						return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;

					case Texture::MinifyFilters::Linear:
						return D3D12_FILTER_MIN_MAG_MIP_LINEAR;

					case Texture::MinifyFilters::NearestMipMapNearest:
						return D3D12_FILTER_MIN_MAG_MIP_LINEAR;

					case Texture::MinifyFilters::LinearMipMapNearest:
						return D3D12_FILTER_MIN_MAG_MIP_LINEAR;

					case Texture::MinifyFilters::NearestMipMapLinear:
						return D3D12_FILTER_MIN_MAG_MIP_LINEAR;

					case Texture::MinifyFilters::LinearMipMapLinear:
						return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
					}
				}

				D3D12_FILTER GetMagnifyFilter(Texture::MagnfyFilters Filter)
				{
					switch (Filter)
					{
					case Texture::MagnfyFilters::Nearest:
						return D3D12_FILTER_MIN_MAG_MIP_LINEAR;

					case Texture::MagnfyFilters::Linear:
						return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
					}
				}

				D3D12_TEXTURE_ADDRESS_MODE GetWrapMode(Texture::WrapModes Mode)
				{
					switch (Mode)
					{
					case Texture::WrapModes::Clamp:
						return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

					case Texture::WrapModes::Repeat:
						return D3D12_TEXTURE_ADDRESS_MODE_BORDER;

					case Texture::WrapModes::ClampToEdge:
						return D3D12_TEXTURE_ADDRESS_MODE_WRAP;

					case Texture::WrapModes::MirroredRepeat:
						return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
					}
				}

				D3D12_CULL_MODE GetCullMode(IDevice::CullModes CullMode)
				{
					switch (CullMode)
					{
					case IDevice::CullModes::None:
					case IDevice::CullModes::Both:
						return D3D12_CULL_MODE_NONE;

					case IDevice::CullModes::Front:
						return D3D12_CULL_MODE_FRONT;

					case IDevice::CullModes::Back:
						return D3D12_CULL_MODE_BACK;
					}
				}

				D3D12_COMPARISON_FUNC GetComparisonFunction(IDevice::TestFunctions TestFunction)
				{
					switch (TestFunction)
					{
					case IDevice::TestFunctions::Never:
						return D3D12_COMPARISON_FUNC_NEVER;

					case IDevice::TestFunctions::Less:
						return D3D12_COMPARISON_FUNC_LESS;

					case IDevice::TestFunctions::LessEqual:
						return D3D12_COMPARISON_FUNC_LESS_EQUAL;

					case IDevice::TestFunctions::Equal:
						return D3D12_COMPARISON_FUNC_EQUAL;

					case IDevice::TestFunctions::NotEqual:
						return D3D12_COMPARISON_FUNC_NOT_EQUAL;

					case IDevice::TestFunctions::GreaterEqual:
						return D3D12_COMPARISON_FUNC_GREATER_EQUAL;

					case IDevice::TestFunctions::Greater:
						return D3D12_COMPARISON_FUNC_GREATER;

					case IDevice::TestFunctions::Always:
						return D3D12_COMPARISON_FUNC_ALWAYS;
					}
				}

				D3D12_BLEND_OP GetBlendEquation(IDevice::BlendEquations BlendEquation)
				{
					switch (BlendEquation)
					{
					case IDevice::BlendEquations::Add:
						return D3D12_BLEND_OP_ADD;

					case IDevice::BlendEquations::Subtract:
						return D3D12_BLEND_OP_SUBTRACT;

					case IDevice::BlendEquations::ReverseSubtract:
						return D3D12_BLEND_OP_REV_SUBTRACT;

					case IDevice::BlendEquations::Min:
						return D3D12_BLEND_OP_MIN;

					case IDevice::BlendEquations::Max:
						return D3D12_BLEND_OP_MAX;
					}
				}

				D3D12_BLEND GetBlendFunction(IDevice::BlendFunctions BlendFunction)
				{
					switch (BlendFunction)
					{
					case IDevice::BlendFunctions::Zero:
						return D3D12_BLEND_ZERO;

					case IDevice::BlendFunctions::One:
						return D3D12_BLEND_ONE;

					case IDevice::BlendFunctions::SourceColor:
						return D3D12_BLEND_SRC_COLOR;

					case IDevice::BlendFunctions::OneMinusSourceColor:
						return D3D12_BLEND_INV_SRC_COLOR;

					case IDevice::BlendFunctions::DestinationColor:
						return D3D12_BLEND_DEST_COLOR;

					case IDevice::BlendFunctions::OneMinusDestinationColor:
						return D3D12_BLEND_INV_DEST_COLOR;

					case IDevice::BlendFunctions::SourceAlpha:
						return D3D12_BLEND_SRC_ALPHA;

					case IDevice::BlendFunctions::OneMinusSourceAlpha:
						return D3D12_BLEND_INV_SRC_ALPHA;

					case IDevice::BlendFunctions::DestinationAlpha:
						return D3D12_BLEND_DEST_ALPHA;

					case IDevice::BlendFunctions::OneMinusDestinationAlpha:
						return D3D12_BLEND_INV_DEST_ALPHA;

					case IDevice::BlendFunctions::ConstantColor:
						return D3D12_BLEND_SRC1_COLOR;

					case IDevice::BlendFunctions::OneMinusConstantColor:
						return D3D12_BLEND_INV_SRC1_COLOR;

					case IDevice::BlendFunctions::ConstantAlpha:
						return D3D12_BLEND_SRC1_ALPHA;

					case IDevice::BlendFunctions::OneMinusConstantAlpha:
						return D3D12_BLEND_INV_SRC1_ALPHA;
					}
				}

				D3D12_PRIMITIVE_TOPOLOGY GetPolygonTopology(SubMesh::PolygonTypes PolygonType)
				{
					switch (PolygonType)
					{
					case SubMesh::PolygonTypes::Lines:
						return D3D_PRIMITIVE_TOPOLOGY_LINELIST;

					case SubMesh::PolygonTypes::LineLoop:
						return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;

					case SubMesh::PolygonTypes::LineStrip:
						return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;

					case SubMesh::PolygonTypes::Triangles:
						return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

					case SubMesh::PolygonTypes::TriangleStrip:
						return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

					case SubMesh::PolygonTypes::TriangleFan:
						return D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
					}

					return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
				}

				D3D12_FILL_MODE GetFillMode(IDevice::PolygonModes PolygonMode)
				{
					switch (PolygonMode)
					{
					case IDevice::PolygonModes::Point:
						return D3D12_FILL_MODE_WIREFRAME;

					case IDevice::PolygonModes::Line:
						return D3D12_FILL_MODE_WIREFRAME;

					case IDevice::PolygonModes::Fill:
						return D3D12_FILL_MODE_SOLID;
					}
				}

				D3D12_STENCIL_OP GetStencilOperation(IDevice::StencilOperations StencilOperation)
				{
					switch (StencilOperation)
					{
					case IDevice::StencilOperations::Keep:
						return D3D12_STENCIL_OP_KEEP;

					case IDevice::StencilOperations::Zero:
						return D3D12_STENCIL_OP_ZERO;

					case IDevice::StencilOperations::Replace:
						return D3D12_STENCIL_OP_REPLACE;

					case IDevice::StencilOperations::Increament:
						return D3D12_STENCIL_OP_INCR;

					case IDevice::StencilOperations::IncreamentWrap:
						return D3D12_STENCIL_OP_INCR_SAT;

					case IDevice::StencilOperations::Decreament:
						return D3D12_STENCIL_OP_DECR;

					case IDevice::StencilOperations::DecreamentWrap:
						return D3D12_STENCIL_OP_DECR_SAT;

					case IDevice::StencilOperations::Invert:
						return D3D12_STENCIL_OP_INVERT;
					}
				}

				bool RaiseDebugMessages(ID3D12InfoQueue* InfoQueue, DirectX12Device* Device)
				{
					if (InfoQueue == nullptr)
						return true;

					IDevice::DebugFunction procedure = Device->GetDebugCallback();
#ifndef DEBUG_MODE
					if (procedure == nullptr)
						return true;
#endif

					DirectX12Wrapper::Debugging::IterateOverDebugMessages(InfoQueue,
						[procedure](D3D12_MESSAGE_ID ID, D3D12_MESSAGE_CATEGORY Category, cstr Message, D3D12_MESSAGE_SEVERITY Severity)
						{
							IDevice::DebugSources source;
							switch (Category)
							{
							case D3D12_MESSAGE_CATEGORY_APPLICATION_DEFINED:	source = IDevice::DebugSources::Application; break;
							case D3D12_MESSAGE_CATEGORY_MISCELLANEOUS:			source = IDevice::DebugSources::API; break;
							case D3D12_MESSAGE_CATEGORY_INITIALIZATION:			source = IDevice::DebugSources::API; break;
							case D3D12_MESSAGE_CATEGORY_CLEANUP:				source = IDevice::DebugSources::API; break;
							case D3D12_MESSAGE_CATEGORY_COMPILATION:			source = IDevice::DebugSources::API; break;
							case D3D12_MESSAGE_CATEGORY_STATE_CREATION:			source = IDevice::DebugSources::API; break;
							case D3D12_MESSAGE_CATEGORY_STATE_SETTING:			source = IDevice::DebugSources::API; break;
							case D3D12_MESSAGE_CATEGORY_STATE_GETTING:			source = IDevice::DebugSources::API; break;
							case D3D12_MESSAGE_CATEGORY_RESOURCE_MANIPULATION:	source = IDevice::DebugSources::API; break;
							case D3D12_MESSAGE_CATEGORY_EXECUTION:				source = IDevice::DebugSources::API; break;
							case D3D12_MESSAGE_CATEGORY_SHADER:					source = IDevice::DebugSources::ProgramCompiler; break;
							}

							IDevice::DebugSeverities severity;
							switch (Severity)
							{
							case D3D12_MESSAGE_SEVERITY_CORRUPTION:	severity = IDevice::DebugSeverities::High; break;
							case D3D12_MESSAGE_SEVERITY_ERROR:		severity = IDevice::DebugSeverities::High; break;
							case D3D12_MESSAGE_SEVERITY_WARNING:	severity = IDevice::DebugSeverities::Medium; break;
							case D3D12_MESSAGE_SEVERITY_INFO:		severity = IDevice::DebugSeverities::Low; break;
							case D3D12_MESSAGE_SEVERITY_MESSAGE:	severity = IDevice::DebugSeverities::Notification; break;
							}

#ifdef DEBUG_MODE
							if (severity == IDevice::DebugSeverities::High)
								Debug::LogError(Message);
							else if (severity == IDevice::DebugSeverities::Medium)
								Debug::LogWarning(Message);
							else
								Debug::LogInfo(Message);

							if (procedure != nullptr)
#endif
								procedure(ID, source, Message, IDevice::DebugTypes::All, severity);
						});

					return true;
				}

				DirectX12Device::DirectX12Device(void) :
					m_Initialized(false),
					m_Factory(nullptr),
					m_Adapter(nullptr),
					m_AdapterDesc({}),
					m_Device(nullptr),
					m_InfoQueue(nullptr),
					m_CopyCommandSet({}),
					m_RenderCommandSet({}),
					m_UploadBuffer({}),
					m_CurrentContextHandle(0),
					m_CurrentContext(nullptr),
					m_CurrentRenderTarget(nullptr),
					m_CurrentRenderTargetViewCount(0),
					m_CurrentDepthStencilView(nullptr),
					m_Viewport({}),
					m_InputLayout(nullptr),
					m_InputLayoutCount(0),
					m_CurrentDescriptorHeapCount(0),
					m_AnyProgramBound(false)
				{
				}

				DirectX12Device::~DirectX12Device(void)
				{
					if (!m_Initialized)
						return;

					RenderingAllocators::ResourceAllocator_Deallocate(m_InputLayout);

					m_BufferHeapAllocator.Deallocate(m_UploadBuffer.Resource);

					m_SamplerViewAllocator.Deinitialize();
					m_ResourceViewAllocator.Deinitialize();
					m_DepthStencilViewAllocator.Deinitialize();
					m_RenderTargetViewAllocator.Deinitialize();

					m_RenderTargetHeapAllocator.Deinitialize();
					m_TextureHeapAllocator.Deinitialize();
					m_BufferHeapAllocator.Deinitialize();

					DestroyCommandSet(m_RenderCommandSet);
					DestroyCommandSet(m_CopyCommandSet);
#if DEBUG_MODE
					if (DirectX12Wrapper::DestroyInstance(m_InfoQueue))
						return;
#endif

					if (DirectX12Wrapper::DestroyInstance(m_Device))
						return;

					if (DirectX12Wrapper::DestroyInstance(m_Factory))
						return;
				}

				bool DirectX12Device::Initialize(void)
				{
#if DEBUG_MODE
					if (!DirectX12Wrapper::Debugging::EnableDebugLayer())
						return false;

					if (!DirectX12Wrapper::Debugging::EnableValidationLayer()) // TODO: DX12 validation layer has too much performance hits with large number draw calls
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

#if DEBUG_MODE
					if (!DirectX12Wrapper::Debugging::GetInfoQueue(m_Device, &m_InfoQueue))
						return false;
#endif

					if (!CreateCommandSet(m_CopyCommandSet, D3D12_COMMAND_LIST_TYPE_COPY))
						return false;

					if (!CreateCommandSet(m_RenderCommandSet, D3D12_COMMAND_LIST_TYPE_DIRECT))
						return false;

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

					ResetState();

					m_InputLayoutCount = SubMeshInfo::GetLayoutCount();
					m_InputLayout = RenderingAllocators::ResourceAllocator_AllocateArray<D3D12_INPUT_ELEMENT_DESC>(m_InputLayoutCount);

					SubMesh::VertexLayouts layout = SubMesh::VertexLayouts::Position;
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

						layout = (SubMesh::VertexLayouts)((int32)layout << 1);
					}

					m_Initialized = true;

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

				bool DirectX12Device::CreateContext(PlatformWindow::WindowHandle WindowHandle, RenderContext::Handle& Handle)
				{
					if (WindowHandle == 0)
						return false;

					for (auto& item : m_Contexts)
						if (item.GetFirst() == (RenderContext::Handle)WindowHandle)
							return false;

					IDXGISwapChain4* swapChain = nullptr;
					if (!CHECK_CALL(DirectX12Wrapper::SwapChain::Create(m_Factory, m_RenderCommandSet.Queue, BACK_BUFFER_COUNT, WindowHandle, &swapChain)))
						return false;

					RenderContextInfo* info = RenderingAllocators::ResourceAllocator_Allocate<RenderContextInfo>();
					PlatformMemory::Set(info, 0, 1);

					info->SwapChain = swapChain;
					info->BackBufferCount = BACK_BUFFER_COUNT;

					Handle = (RenderContext::Handle)WindowHandle;

					m_Contexts[Handle] = info;

					return true;
				}

				bool DirectX12Device::DestroyContext(RenderContext::Handle Handle)
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

					RenderingAllocators::ResourceAllocator_Deallocate(info);

					m_Contexts.Remove(Handle);

					return true;
				}

				bool DirectX12Device::SetContext(RenderContext::Handle Handle)
				{
					if (m_CurrentContextHandle == Handle)
						return true;

					if (Handle == 0)
					{
						m_CurrentContextHandle = 0;
						m_CurrentContext = nullptr;

						m_CurrentRenderTargetViewCount = 0;
						m_CurrentDepthStencilView = nullptr;

						return true;
					}

					if (!m_Contexts.Contains(Handle))
						return false;

					RenderContextInfo* info = m_Contexts[Handle];

					m_CurrentContextHandle = Handle;
					m_CurrentContext = info;

					if (m_CurrentRenderTarget == nullptr)
						BindRenderTarget(0);

					ResetState();

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
					const D3D12_RESOURCE_DIMENSION dimension = GetTextureType(Texture::Types::TwoD);

					static int index = 0;

					for (uint8 i = 0; i < m_CurrentContext->BackBufferCount; ++i)
					{
						ViewInfo& renderTargetView = m_CurrentContext->Views[i][RenderContextInfo::RENDER_TARGET_VIEW_INDEX];
						INITIALIZE_RESOURCE_INFO(&renderTargetView, D3D12_RESOURCE_STATE_PRESENT);
						renderTargetView.Resource.Resource = backBuffers[i];

						renderTargetView.Point = RenderTarget::AttachmentPoints::Color0;
						renderTargetView.Format = renderTargetView.Resource.Resource->GetDesc().Format;
						if (!CHECK_CALL(m_RenderTargetViewAllocator.AllocateRenderTargetView(renderTargetView.Resource.Resource, &renderTargetView.TargetView)))
							return false;

						ViewInfo& depthStencilView = m_CurrentContext->Views[i][RenderContextInfo::DEPTH_STENCIL_VIEW_INDEX];
						INITIALIZE_RESOURCE_INFO(&depthStencilView, depthStencilBufferState);
						if (!CHECK_CALL(m_RenderTargetHeapAllocator.Allocate(m_CurrentContext->Size.X, m_CurrentContext->Size.Y, depthStencilFormat, false, depthStencilBufferState, false, &depthStencilView.Resource)))
							return false;

						depthStencilView.Point = RenderTarget::AttachmentPoints::DepthStencil;
						depthStencilView.Format = depthStencilFormat;
						if (!CHECK_CALL(m_DepthStencilViewAllocator.AllocateDepthStencilView(depthStencilView.Resource.Resource, depthStencilFormat, D3D12_DSV_FLAG_NONE, &depthStencilView.TargetView)))
							return false;
					}

					m_CurrentContext->CurrentBackBufferIndex = 0;
					m_CurrentContext->Initialized = true;

					SKIP_NEXT_FRAMES();

					return true;
				}

				bool DirectX12Device::SetViewport(const Vector2I& Position, const Vector2I& Size)
				{
					m_Viewport.TopLeftX = Position.X;
					m_Viewport.TopLeftY = Position.Y;
					m_Viewport.Width = Size.X;
					m_Viewport.Height = Size.Y;
					m_Viewport.MinDepth = 0;
					m_Viewport.MaxDepth = 1;

					return true;
				}

				bool DirectX12Device::SetResourceName(NativeType::Handle Handle, ResourceTypes Type, cwstr Name)
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

				void DirectX12Device::SetState(const State& State)
				{
					PlatformMemory::Copy(&State, &m_State, 1);
				}

				bool DirectX12Device::CreateBuffer(GPUBuffer::Handle& Handle)
				{
					BoundBuffersInfo* info = RenderingAllocators::ResourceAllocator_Allocate<BoundBuffersInfo>();
					INITIALIZE_RESOURCE_INFO(&info->Buffer, D3D12_RESOURCE_STATE_COMMON);
					info->Resource = nullptr;
					info->Buffer.Size = 0;
					info->Buffer.Stride = 0;

					Handle = (GPUBuffer::Handle)info;

					return true;
				}

				bool DirectX12Device::DestroyBuffer(GPUBuffer::Handle Handle)
				{
					if (Handle == 0)
						return false;

					if (!WaitForGPU())
						return false;

					BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

					if (boundBufferInfo->Buffer.Resource.Resource != nullptr)
						if (!CHECK_CALL(m_BufferHeapAllocator.Deallocate(boundBufferInfo->Buffer.Resource)))
							return false;

					RenderingAllocators::ResourceAllocator_Deallocate(boundBufferInfo);

					return true;
				}

				bool DirectX12Device::InitializeConstantBuffer(GPUBuffer::Handle Handle, const byte* Data, uint32 Size)
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

				bool DirectX12Device::CopyFromVertexToBuffer(GPUBuffer::Handle Handle, SubMesh::Handle FromMeshHandle, uint32 Size)
				{
					if (Handle == 0)
						return false;

					if (FromMeshHandle == 0)
						return false;

					BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

					if (!CreateIntermediateBuffer(Size, &boundBufferInfo->Buffer))
						return false;

					boundBufferInfo->Resource = ReinterpretCast(TextureResourceInfo*, FromMeshHandle);

					return true;
				}

				bool DirectX12Device::CopyFromBufferToVertex(GPUBuffer::Handle Handle, SubMesh::Handle ToMeshHandle, uint32 Size)
				{
					if (Handle == 0)
						return false;

					BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

					return CopyBuffer(GPUBuffer::Types::Pixel, &boundBufferInfo->Buffer, true, boundBufferInfo->Resource, false);
				}

				bool DirectX12Device::CopyFromIndexToBuffer(GPUBuffer::Handle Handle, SubMesh::Handle FromMeshHandle, uint32 Size)
				{
					if (Handle == 0)
						return false;

					if (FromMeshHandle == 0)
						return false;

					BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

					if (!CreateIntermediateBuffer(Size, &boundBufferInfo->Buffer))
						return false;

					boundBufferInfo->Resource = ReinterpretCast(TextureResourceInfo*, FromMeshHandle);

					return true;
				}

				bool DirectX12Device::CopyFromBufferToIndex(GPUBuffer::Handle Handle, SubMesh::Handle ToMeshHandle, uint32 Size)
				{
					if (Handle == 0)
						return false;

					BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

					return CopyBuffer(GPUBuffer::Types::Pixel, &boundBufferInfo->Buffer, true, boundBufferInfo->Resource, false);
				}

				bool DirectX12Device::CopyFromTextureToBuffer(GPUBuffer::Handle Handle, Texture::Handle FromTextureHandle, uint32 Size, Texture::Types TextureType, Formats TextureFormat, uint32 Level)
				{
					if (Handle == 0)
						return false;

					if (FromTextureHandle == 0)
						return false;

					BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

					if (!CreateIntermediateBuffer(Size, &boundBufferInfo->Buffer))
						return false;

					boundBufferInfo->Resource = ReinterpretCast(TextureResourceInfo*, FromTextureHandle);

					return true;
				}

				bool DirectX12Device::CopyFromBufferToTexture(GPUBuffer::Handle Handle, Texture::Handle ToTextureHandle, Texture::Types TextureType, uint32 Width, uint32 Height, Formats TextureFormat)
				{
					if (Handle == 0)
						return false;

					BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

					return CopyBuffer(GPUBuffer::Types::Pixel, &boundBufferInfo->Buffer, true, boundBufferInfo->Resource, false);
				}

				bool DirectX12Device::LockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Access Access, byte** Buffer)
				{
					if (Handle == 0)
						return false;

					BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

					if (Type != GPUBuffer::Types::Constant)
					{
						if (boundBufferInfo->Resource == nullptr)
							return false;

						if (!CopyBuffer(Type, boundBufferInfo->Resource, false, &boundBufferInfo->Buffer, true))
							return false;
					}

					return CHECK_CALL(DirectX12Wrapper::Resource::Map(boundBufferInfo->Buffer.Resource.Resource, Buffer));
				}

				bool DirectX12Device::UnlockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type)
				{
					if (Handle == 0)
						return false;

					BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

					CHECK_CALL(DirectX12Wrapper::Resource::Unmap(boundBufferInfo->Buffer.Resource.Resource));

					if (Type == GPUBuffer::Types::Constant)
						return true;

					return CopyBuffer(Type, &boundBufferInfo->Buffer, true, boundBufferInfo->Resource, false);
				}

				bool DirectX12Device::CompileProgramAPI(const Shaders* Shaders, CompiledShaders* CompiledShaders, cstr* ErrorMessage)
				{
#define IMPLEMENT_COMPILE(StageType, StageName) \
					if (Shaders->StageName == nullptr) \
					{ \
						CompiledShaders->StageName.Buffer = nullptr; \
						CompiledShaders->StageName.Size = 0; \
					} \
					else \
					{ \
						ID3DBlob* data = nullptr; \
						if (DirectX12Wrapper::Shader::Compile(Shaders->StageName, StageType, true, &data, ErrorMessage)) \
						{ \
							uint16 dataSize = data->GetBufferSize(); \
							if (dataSize > CompiledShaders->StageName.Size) \
							{ \
								if (ErrorMessage != nullptr) \
									*ErrorMessage = "Not enough buffer size for shader machine code"; \
								return false; \
							} \
							PlatformMemory::Copy(ReinterpretCast(const byte*, data->GetBufferPointer()), CompiledShaders->StageName.Buffer, dataSize); \
							CompiledShaders->StageName.Size = dataSize; \
						} \
						else \
						{ \
							CompiledShaders->StageName.Buffer = nullptr; \
							CompiledShaders->StageName.Size = 0; \
							return false; \
						} \
					}

					IMPLEMENT_COMPILE("vs_5_1", VertexShader);
					IMPLEMENT_COMPILE("ds_5_1", TessellationShader);
					IMPLEMENT_COMPILE("gs_5_1", GeometryShader);
					IMPLEMENT_COMPILE("ps_5_1", FragmentShader);
					IMPLEMENT_COMPILE("cs_5_1", ComputeShader);

					return true;

#undef IMPLEMENT_COMPILE
				}

				bool DirectX12Device::CompileProgram(const Shaders* Shaders, CompiledShaders* CompiledShaders, cstr* ErrorMessage)
				{
					return CompileProgramAPI(Shaders, CompiledShaders, ErrorMessage);
				}

				bool DirectX12Device::CreateProgram(const CompiledShaders* Shaders, Program::Handle& Handle, cstr* ErrorMessage)
				{
#define IMPLEMENT(StageName) \
					if (Shaders->StageName.Size != 0) \
					{ \
						programInfos->StageName.Buffer = RenderingAllocators::ResourceAllocator_AllocateArray<byte>(Shaders->StageName.Size); \
						PlatformMemory::Copy(Shaders->StageName.Buffer, programInfos->StageName.Buffer, Shaders->StageName.Size); \
						programInfos->StageName.Size = Shaders->StageName.Size; \
					}

					ID3DBlob* serializedRootSignature = nullptr;
					if (!CHECK_CALL(DirectX12Wrapper::Shader::GetInlineRootSignature(Shaders->VertexShader.Buffer, Shaders->VertexShader.Size, &serializedRootSignature)))
						return false;

					ID3D12RootSignature* rootSignature = nullptr;
					if (!CHECK_CALL(DirectX12Wrapper::RootSignature::Create(m_Device, serializedRootSignature, &rootSignature)))
						return false;

					ProgramInfos* programInfos = RenderingAllocators::ResourceAllocator_Allocate<ProgramInfos>();
					PlatformMemory::Set(programInfos, 0, 1);

					programInfos->RootSignature = rootSignature;

					IMPLEMENT(VertexShader);
					IMPLEMENT(TessellationShader);
					IMPLEMENT(GeometryShader);
					IMPLEMENT(FragmentShader);
					IMPLEMENT(ComputeShader);

					Handle = (Program::Handle)programInfos;

					return true;
#undef IMPLEMENT
				}

				bool DirectX12Device::DestroyProgram(Program::Handle Handle)
				{
#define IMPLEMENT(StageName) \
					if (programInfos->StageName.Buffer != nullptr) \
						RenderingAllocators::ResourceAllocator_Deallocate(programInfos->StageName.Buffer); \

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

					RenderingAllocators::ResourceAllocator_Deallocate(programInfos);

					return true;
#undef IMPLEMENT
				}

				bool DirectX12Device::BindProgram(Program::Handle Handle)
				{
#define IMPLEMENT_SET_SHADER_DATA(StageName) desc.StageName = { programInfos->StageName.Buffer, programInfos->StageName.Size }

					m_CurrentDescriptorHeapCount = 0;

					m_AnyProgramBound = false;

					if (Handle == 0)
						return true;

					ProgramInfos* programInfos = ReinterpretCast(ProgramInfos*, Handle);

					uint32 stateHash = GetStateHash();

					ID3D12PipelineState* pipelineState = nullptr;

					if (programInfos->Pipelines.Contains(stateHash))
						pipelineState = programInfos->Pipelines[stateHash];
					else
					{
						DirectX12Wrapper::PipelineStateObject::GraphicsPipelineStateDesc desc = {};
						IMPLEMENT_SET_SHADER_DATA(VertexShader);
						IMPLEMENT_SET_SHADER_DATA(TessellationShader);
						IMPLEMENT_SET_SHADER_DATA(GeometryShader);
						IMPLEMENT_SET_SHADER_DATA(FragmentShader);
						//IMPLEMENT_SET_SHADER_DATA(ComputeShader);

						FillGraphicsPipelineState(m_State, desc);

						CHECK_CALL(DirectX12Wrapper::PipelineStateObject::Create(m_Device, &desc, &pipelineState));

						programInfos->Pipelines[stateHash] = pipelineState;
					}

					if (pipelineState == nullptr)
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::Command::AddSetGraphicsRootSignature(m_RenderCommandSet.List, programInfos->RootSignature)))
						return false;

					m_AnyProgramBound = CHECK_CALL(DirectX12Wrapper::Command::AddSetPipelineState(m_RenderCommandSet.List, pipelineState));

					return m_AnyProgramBound;

#undef IMPLEMENT_SET_SHADER_DATA
				}

				bool DirectX12Device::SetProgramConstantBuffer(Program::ConstantHandle Handle, ConstantBuffer::Handle Value)
				{
					if (!m_AnyProgramBound)
						return false;

					if (Value == 0)
						return false;

					BoundBuffersInfo* bufferInfo = ReinterpretCast(BoundBuffersInfo*, Value);

					return CHECK_CALL(DirectX12Wrapper::Command::AddSetGraphicsConstantBuffer(m_RenderCommandSet.List, Handle, bufferInfo->Buffer.Resource.Resource->GetGPUVirtualAddress()));
				}

				bool DirectX12Device::SetProgramTexture(Program::ConstantHandle Handle, Texture::Types Type, Texture::Handle Value)
				{
					if (!m_AnyProgramBound)
						return false;

					if (Value == 0)
						return false;

					TextureResourceInfo* resourceInfo = ReinterpretCast(TextureResourceInfo*, Value);

					if (!AddTransitionResourceBarrier(m_RenderCommandSet, resourceInfo, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE))
						return false;

					bool found = false;
					for (uint8 i = 0; i < m_CurrentDescriptorHeapCount; ++i)
					{
						if (m_CurrentDescriptorHeaps[i] != resourceInfo->View.DescriptorHeap)
							continue;

						found = true;
						break;
					}

					if (!found)
					{
						if (m_CurrentDescriptorHeapCount >= MAX_DESCRIPTOR_HEAP_COUNT)
							return false;

						m_CurrentDescriptorHeaps[m_CurrentDescriptorHeapCount++] = resourceInfo->View.DescriptorHeap;
						m_CurrentDescriptorHeaps[m_CurrentDescriptorHeapCount++] = resourceInfo->SamplerView.DescriptorHeap;

						if (!CHECK_CALL(DirectX12Wrapper::Command::AddSetDescriptorHeap(m_RenderCommandSet.List, m_CurrentDescriptorHeaps, m_CurrentDescriptorHeapCount)))
							return false;
					}

					if (!DirectX12Wrapper::Command::AddSetGraphicsRootDescriptorTable(m_RenderCommandSet.List, Handle + 1, resourceInfo->SamplerView.GPUHandle))
						return false;

					return CHECK_CALL(DirectX12Wrapper::Command::AddSetGraphicsRootDescriptorTable(m_RenderCommandSet.List, Handle, resourceInfo->View.GPUHandle));
				}

				bool DirectX12Device::CreateTexture(const TextureInfo* Info, Texture::Handle& Handle)
				{
					D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

					DXGI_FORMAT format = GetTextureFormat(Info->Format);
					D3D12_RESOURCE_DIMENSION dimension = GetTextureType(Info->Type);

					TextureResourceInfo* info = RenderingAllocators::ResourceAllocator_Allocate<TextureResourceInfo>();
					INITIALIZE_RESOURCE_INFO(info, state);

					if (Info->Type == Texture::Types::TwoD)
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

						uint32 dataPitch = Texture::GetRowPitch(Info->Format, Info->Dimension.X);
						uint8 pixelSize = Texture::GetPixelSize(Info->Format);

						uint32 resourcePitch = DirectX12Wrapper::Support::GetResourceRowPitch(m_Device, info->Resource.Resource);
						uint8 padding = GetTextureFormatPadding(Info->Format);

						for (int32 y = 0; y < Info->Dimension.Y; ++y)
							for (int32 x = 0; x < Info->Dimension.X; ++x)
								PlatformMemory::Copy(Info->Data + (y * dataPitch) + (x * pixelSize), buffer + (y * resourcePitch) + (x * (pixelSize + padding)), pixelSize);

						END_UPLOAD(GPUBuffer::Types::Pixel, info, false);
					}

					if (!CHECK_CALL(m_ResourceViewAllocator.AllocateTextureShaderResourceView(info->Resource.Resource, format, dimension, &info->View)))
						return false;

					Handle = (Texture::Handle)info;

					return true;
				}

				bool DirectX12Device::DestroyTexture(Texture::Handle Handle)
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

					RenderingAllocators::ResourceAllocator_Deallocate(textureResourceInfo);

					return true;
				}

				bool DirectX12Device::SetTextureVerticalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode)
				{
					if (Handle == 0)
						return false;

					TextureResourceInfo* textureResourceInfo = ReinterpretCast(TextureResourceInfo*, Handle);
					textureResourceInfo->SamplerDescription.AddressV = GetWrapMode(Mode);

					REALLOCATE_SAMPLER(textureResourceInfo);
				}

				bool DirectX12Device::SetTextureHorizontalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode)
				{
					if (Handle == 0)
						return false;

					TextureResourceInfo* textureResourceInfo = ReinterpretCast(TextureResourceInfo*, Handle);
					textureResourceInfo->SamplerDescription.AddressU = GetWrapMode(Mode);

					REALLOCATE_SAMPLER(textureResourceInfo);
				}

				bool DirectX12Device::SetTextureMinifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MinifyFilters Filter)
				{
					if (Handle == 0)
						return false;

					TextureResourceInfo* textureResourceInfo = ReinterpretCast(TextureResourceInfo*, Handle);
					textureResourceInfo->SamplerDescription.Filter = D3D12_ENCODE_BASIC_FILTER(GetMinifyFilter(Filter), D3D12_DECODE_MAG_FILTER(textureResourceInfo->SamplerDescription.Filter), D3D12_FILTER_MIN_MAG_MIP_LINEAR, 0);

					REALLOCATE_SAMPLER(textureResourceInfo);
				}

				bool DirectX12Device::SetTextureMagnifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MagnfyFilters Filter)
				{
					if (Handle == 0)
						return false;

					TextureResourceInfo* textureResourceInfo = ReinterpretCast(TextureResourceInfo*, Handle);
					textureResourceInfo->SamplerDescription.Filter = D3D12_ENCODE_BASIC_FILTER(D3D12_DECODE_MIN_FILTER(textureResourceInfo->SamplerDescription.Filter), GetMagnifyFilter(Filter), D3D12_FILTER_MIN_MAG_MIP_LINEAR, 0);

					REALLOCATE_SAMPLER(textureResourceInfo);
				}

				bool DirectX12Device::GenerateTextureMipMap(Texture::Handle Handle, Texture::Types Type)
				{
					return true;
				}

				bool DirectX12Device::CreateRenderTarget(const RenderTargetInfo* Info, RenderTarget::Handle& Handle, TextureList& Textures)
				{
#define CREATE_VIEW(IsColored, CurrnetState) \
					{ \
						for (const auto & textureInfo : Info->Textures) \
						{ \
							if (!RenderTarget::IsColorPoint(textureInfo.Point) == IsColored) \
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
							Textures.Add((Texture::Handle)ReinterpretCast(TextureResourceInfo*, view)); \
						} \
					}

					if (Info->Textures.GetSize() == 0)
						return false;

					RenderTargetInfos* renderTargetInfos = RenderingAllocators::ResourceAllocator_Allocate<RenderTargetInfos>();
					PlatformMemory::Set(renderTargetInfos, 0, 1);

					renderTargetInfos->Views.Extend(Info->Textures.GetSize());

					uint8 index = 0;
					CREATE_VIEW(true, D3D12_RESOURCE_STATE_COMMON);
					CREATE_VIEW(false, D3D12_RESOURCE_STATE_DEPTH_WRITE);

					Handle = (RenderTarget::Handle)renderTargetInfos;

					return true;

#undef CREATE_VIEW
				}

				bool DirectX12Device::DestroyRenderTarget(RenderTarget::Handle Handle)
				{
					if (Handle == 0)
						return false;

					if (!WaitForGPU())
						return false;

					RenderTargetInfos* renderTargetInfos = ReinterpretCast(RenderTargetInfos*, Handle);

					for (auto& viewInfo : renderTargetInfos->Views)
					{
						if (RenderTarget::IsColorPoint(viewInfo.Point))
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

					RenderingAllocators::ResourceAllocator_Deallocate(renderTargetInfos);

					return true;
				}

				bool DirectX12Device::BindRenderTarget(RenderTarget::Handle Handle)
				{
					m_CurrentRenderTargetViewCount = 0;
					m_CurrentDepthStencilView = nullptr;

					if (Handle == 0)
					{
						FILL_RENDER_VIEWS_USING_CONTEXT();
					}
					else
					{
						m_CurrentRenderTarget = ReinterpretCast(RenderTargetInfos*, Handle);

						for (auto& viewInfo : m_CurrentRenderTarget->Views)
						{
							uint8 colorPointIndex = (uint8)viewInfo.Point - (uint8)RenderTarget::AttachmentPoints::Color0;

							if (RenderTarget::IsColorPoint(viewInfo.Point))
							{
								m_CurrentRenderTargetViews[colorPointIndex] = &viewInfo;
								++m_CurrentRenderTargetViewCount;
							}
							else
								m_CurrentDepthStencilView = &viewInfo;
						}
					}

					ADD_TRANSITION_STATE_FOR_TARGET_BUFFERS(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_DEPTH_WRITE);

					D3D12_CPU_DESCRIPTOR_HANDLE renderTargetHandles[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT];
					for (uint8 i = 0; i < m_CurrentRenderTargetViewCount; ++i)
						renderTargetHandles[i] = m_CurrentRenderTargetViews[i]->TargetView.CPUHandle;

					if (m_CurrentRenderTargetViewCount != 0 || m_CurrentDepthStencilView != nullptr)
						if (!CHECK_CALL(DirectX12Wrapper::Command::AddSetTargets(m_RenderCommandSet.List, renderTargetHandles, m_CurrentRenderTargetViewCount, (m_CurrentDepthStencilView == nullptr ? nullptr : &m_CurrentDepthStencilView->TargetView.CPUHandle))))
							return false;

					return true;
				}

				bool DirectX12Device::CreateMesh(const SubMeshInfo* Info, SubMesh::Handle& Handle)
				{
					if (Info->Vertices.GetSize() == 0)
						return false;

					D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

					MeshBufferInfo* info = RenderingAllocators::ResourceAllocator_Allocate<MeshBufferInfo>();
					INITIALIZE_RESOURCE_INFO(&info->VertexBuffer, state);

					uint32 bufferSize = SubMesh::GetVertexBufferSize(Info->Vertices.GetSize());
					if (!CHECK_CALL(m_BufferHeapAllocator.Allocate(bufferSize, state, false, &info->VertexBuffer.Resource)))
						return false;

					info->VertexBuffer.Size = bufferSize;
					info->VertexBuffer.Stride = SubMesh::GetVertexSize();

					{
						BEGIN_UPLOAD();

						PlatformMemory::Copy(ReinterpretCast(const byte*, Info->Vertices.GetData()), buffer, bufferSize);

						END_UPLOAD(GPUBuffer::Types::Vertex, &info->VertexBuffer, true);
					}

					INITIALIZE_RESOURCE_INFO(&info->IndexBuffer, state);

					if (Info->Indices.GetSize() != 0)
					{

						bufferSize = SubMesh::GetIndexBufferSize(Info->Indices.GetSize());
						if (!CHECK_CALL(m_BufferHeapAllocator.Allocate(bufferSize, state, false, &info->IndexBuffer.Resource)))
							return false;

						info->IndexBuffer.Size = bufferSize;
						info->IndexBuffer.Stride = SubMesh::GetIndexSize();

						{
							BEGIN_UPLOAD();

							PlatformMemory::Copy(ReinterpretCast(const byte*, Info->Indices.GetData()), buffer, bufferSize);

							END_UPLOAD(GPUBuffer::Types::Index, &info->IndexBuffer, true);
						}
					}

					Handle = (SubMesh::Handle)info;

					return true;
				}

				bool DirectX12Device::DestroyMesh(SubMesh::Handle Handle)
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

					RenderingAllocators::ResourceAllocator_Deallocate(meshBufferInfo);

					return true;
				}

				bool DirectX12Device::BindMesh(SubMesh::Handle Handle)
				{
					if (Handle == 0)
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::Command::AddSetPrimitiveTopologyCommand(m_RenderCommandSet.List, GetPolygonTopology(SubMesh::PolygonTypes::Triangles))))
						return false;

					MeshBufferInfo* meshBufferInfo = ReinterpretCast(MeshBufferInfo*, Handle);

					BufferInfo& vertextBufferInfo = meshBufferInfo->VertexBuffer;
					if (!CHECK_CALL(DirectX12Wrapper::Command::AddSetVertexBufferCommand(m_RenderCommandSet.List, vertextBufferInfo.Resource.Resource, vertextBufferInfo.Size, vertextBufferInfo.Stride)))
						return false;

					BufferInfo& indextBufferInfo = meshBufferInfo->IndexBuffer;
					if (indextBufferInfo.Resource.Resource != nullptr)
						if (!CHECK_CALL(DirectX12Wrapper::Command::AddSetIndexBufferCommand(m_RenderCommandSet.List, indextBufferInfo.Resource.Resource, indextBufferInfo.Size)))
							return false;

					return true;
				}

				bool DirectX12Device::Clear(ClearFlags Flags, const ColorUI8& Color)
				{
					m_ClearColor = Color;

					if (BitwiseUtils::IsEnabled(Flags, ClearFlags::ColorBuffer))
					{
						Vector4F color;
						Helper::GetNormalizedColor(m_ClearColor, color);

						for (uint8 i = 0; i < m_CurrentRenderTargetViewCount; ++i)
						{
							if (!CHECK_CALL(DirectX12Wrapper::Command::AddClearRenderTargetCommand(m_RenderCommandSet.List, m_CurrentRenderTargetViews[i]->TargetView.CPUHandle, &color.X)))
								return false;
						}
					}

					bool shouldClearDepth = BitwiseUtils::IsEnabled(Flags, ClearFlags::DepthBuffer);
					bool shouldClearStencil = BitwiseUtils::IsEnabled(Flags, ClearFlags::StencilBuffer);
					if (m_CurrentDepthStencilView != nullptr && (shouldClearDepth || shouldClearStencil))
					{
						D3D12_CLEAR_FLAGS flags = (D3D12_CLEAR_FLAGS)0;
						if (shouldClearDepth) flags |= D3D12_CLEAR_FLAG_DEPTH;
						if (shouldClearStencil) flags |= D3D12_CLEAR_FLAG_STENCIL;

						if (!CHECK_CALL(DirectX12Wrapper::Command::AddClearDepthStencilCommand(m_RenderCommandSet.List, m_CurrentDepthStencilView->TargetView.CPUHandle, flags, 1, 1)))
							return false;
					}

					return true;
				}

				bool DirectX12Device::DrawIndexed(SubMesh::PolygonTypes PolygonType, uint32 IndexCount)
				{
					if (!CHECK_CALL(DirectX12Wrapper::Command::AddSetPrimitiveTopologyCommand(m_RenderCommandSet.List, GetPolygonTopology(PolygonType))))
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::Command::AddDrawIndexedCommand(m_RenderCommandSet.List, IndexCount)))
						return false;

					return true;
				}

				bool DirectX12Device::DrawArray(SubMesh::PolygonTypes PolygonType, uint32 VertexCount)
				{
					if (!CHECK_CALL(DirectX12Wrapper::Command::AddSetPrimitiveTopologyCommand(m_RenderCommandSet.List, GetPolygonTopology(PolygonType))))
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::Command::AddDrawCommand(m_RenderCommandSet.List, VertexCount)))
						return false;

					return true;
				}

				bool DirectX12Device::BeginExecute(void)
				{
					if (!CHECK_CALL(DirectX12Wrapper::Command::AddSetViewportCommand(m_RenderCommandSet.List, &m_Viewport)))
						return false;

					return true;
				}

				bool DirectX12Device::EndExecute(void)
				{
					ADD_TRANSITION_STATE_FOR_TARGET_BUFFERS(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_DEPTH_WRITE);

					return ExecuteCommands(m_RenderCommandSet);
				}

				bool DirectX12Device::SwapBuffers(void)
				{
					if (m_CurrentContext == nullptr)
						return false;

					IDXGISwapChain4* swapChain = m_CurrentContext->SwapChain;

					if (!CHECK_CALL(DirectX12Wrapper::SwapChain::Present(swapChain, false)))
						return false;

					m_CurrentContext->CurrentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();

					if (m_CurrentRenderTarget == nullptr)
						FILL_RENDER_VIEWS_USING_CONTEXT();

					return true;
				}

				bool DirectX12Device::BeginEvent(cwstr Label)
				{
					PIXBeginEvent(m_RenderCommandSet.List, 0, Label);

					return false;
				}

				bool DirectX12Device::EndEvent(void)
				{
					PIXEndEvent(m_RenderCommandSet.List);

					return true;
				}

				bool DirectX12Device::SetMarker(cwstr Label)
				{
					PIXSetMarker(m_RenderCommandSet.List, 0, Label);

					return true;
				}

				bool DirectX12Device::SetDebugCallback(DebugFunction Callback)
				{
					m_DebugCallback = Callback;

					return true;
				}

				bool DirectX12Device::AddTransitionResourceBarrier(CommandSet& Set, ResourceInfo* Info, D3D12_RESOURCE_STATES AfterState)
				{
					if (Info->State == AfterState)
					{
#if DEBUG_MODE
						if (!CHECK_CALL(DirectX12Wrapper::Command::AddResourceStateAssertion(Set.Debug, Info->Resource.Resource, AfterState)))
							return false;
#endif

						return true;
					}

					if (!CHECK_CALL(DirectX12Wrapper::Command::AddTransitionResourceBarrier(Set.List, Info->Resource.Resource, Info->State, AfterState)))
						return false;

					Info->State = AfterState;

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

				bool DirectX12Device::CreateCommandSet(CommandSet& Set, D3D12_COMMAND_LIST_TYPE Type)
				{
					if (!CHECK_CALL(DirectX12Wrapper::Command::CreateCommandQueue(m_Device, Type, &Set.Queue)))
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::Command::CreateCommandAllocator(m_Device, Type, &Set.Allocator)))
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::Command::CreateCommandList(m_Device, Set.Allocator, Type, &Set.List)))
						return false;
#if DEBUG_MODE
					if (!CHECK_CALL(DirectX12Wrapper::Debugging::GetDebugCommandList(Set.List, &Set.Debug)))
						return false;
#endif

					if (!CHECK_CALL(DirectX12Wrapper::Fence::Create(m_Device, &Set.Fence)))
						return false;

					Set.FenceValue = 1;
					Set.FenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

					return true;
				}

				bool DirectX12Device::DestroyCommandSet(CommandSet& Set)
				{
					CloseHandle(Set.FenceEvent);

					if (!CHECK_CALL(DirectX12Wrapper::DestroyInstance(Set.Fence)))
						return false;

#if DEBUG_MODE
					if (!CHECK_CALL(DirectX12Wrapper::DestroyInstance(Set.Debug)))
						return false;
#endif

					if (!CHECK_CALL(DirectX12Wrapper::DestroyInstance(Set.List)))
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::DestroyInstance(Set.Allocator)))
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::DestroyInstance(Set.Queue)))
						return false;

					return true;
				}

				bool DirectX12Device::ExecuteCommands(CommandSet& Set)
				{
					if (!CHECK_CALL(DirectX12Wrapper::Command::CloseCommandList(Set.List)))
						return false;

					if (Set.SkipFrameCount != 0)
						--Set.SkipFrameCount;
					else
					{
						if (!CHECK_CALL(DirectX12Wrapper::Command::ExecuteCommandList(Set.Queue, Set.List)))
							return false;
					}

					if (!WaitForGPU(Set))
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::Command::ResetCommandAllocator(Set.Allocator)))
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::Command::ResetCommandList(Set.List, Set.Allocator)))
						return false;

					return true;
				}

				bool DirectX12Device::WaitForGPU(void)
				{
					if (!WaitForGPU(m_CopyCommandSet))
						return false;

					if (!WaitForGPU(m_RenderCommandSet))
						return false;

					return true;
				}

				bool DirectX12Device::WaitForGPU(CommandSet& Set)
				{
					return CHECK_CALL(DirectX12Wrapper::Fence::SignalAndWait(Set.Queue, Set.Fence, Set.FenceEvent, Set.FenceValue));
				}

				bool DirectX12Device::AllocateSampler(TextureResourceInfo* Info)
				{
					D3D12_SAMPLER_DESC& desc = Info->SamplerDescription;

					desc.Filter = D3D12_ENCODE_BASIC_FILTER(GetMinifyFilter(Texture::MinifyFilters::Linear), GetMagnifyFilter(Texture::MagnfyFilters::Linear), D3D12_FILTER_MIN_MAG_MIP_LINEAR, 0);
					desc.AddressU = desc.AddressV = desc.AddressW = GetWrapMode(Texture::WrapModes::Clamp);
					desc.MipLODBias = 0;
					desc.MaxAnisotropy = 0;
					desc.ComparisonFunc = GetComparisonFunction(TestFunctions::Never);
					desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0;
					desc.MinLOD = 0;
					desc.MaxLOD = 0;

					return CHECK_CALL(m_SamplerViewAllocator.AllocateSampler(Info->SamplerDescription, &Info->SamplerView));
				}

				bool DirectX12Device::CopyBuffer(GPUBuffer::Types Type, ResourceInfo* Source, bool SourceIsABuffer, ResourceInfo* Destination, bool DestinationIsABuffer)
				{
					D3D12_RESOURCE_STATES sourceState = Source->State;
					D3D12_RESOURCE_STATES destinationState = Source->State;

					if (!AddTransitionResourceBarrier(m_CopyCommandSet, Source, D3D12_RESOURCE_STATE_COPY_SOURCE))
						return false;

					if (!AddTransitionResourceBarrier(m_CopyCommandSet, Destination, D3D12_RESOURCE_STATE_COPY_DEST))
						return false;

					if (Type == GPUBuffer::Types::Constant || Type == GPUBuffer::Types::Vertex || Type == GPUBuffer::Types::Index)
					{
						BufferInfo* bufferInfo = nullptr;
						if (DestinationIsABuffer)
							bufferInfo = ReinterpretCast(BufferInfo*, Destination);
						else if (SourceIsABuffer)
							bufferInfo = ReinterpretCast(BufferInfo*, Source);
						else
							return false;

						if (!CHECK_CALL(DirectX12Wrapper::Command::AddCopyBufferCommand(m_CopyCommandSet.List, Source->Resource.Resource, Destination->Resource.Resource, bufferInfo->Size)))
							return false;
					}
					else if (Type == GPUBuffer::Types::Pixel)
					{
						if (SourceIsABuffer && !DestinationIsABuffer)
						{
							if (!CHECK_CALL(DirectX12Wrapper::Command::AddCopyBufferToTextureCommand(m_CopyCommandSet.List, Source->Resource.Resource, Destination->Resource.Resource)))
								return false;
						}
						else if (!SourceIsABuffer && DestinationIsABuffer)
						{
							if (!CHECK_CALL(DirectX12Wrapper::Command::AddCopyTextureToBufferCommand(m_CopyCommandSet.List, Source->Resource.Resource, Destination->Resource.Resource)))
								return false;
						}
					}

					if (!AddTransitionResourceBarrier(m_CopyCommandSet, Source, sourceState))
						return false;

					if (!AddTransitionResourceBarrier(m_CopyCommandSet, Destination, destinationState))
						return false;

					return ExecuteCommands(m_CopyCommandSet);
				}

				void DirectX12Device::FillGraphicsPipelineState(const IDevice::State& State, DirectX12Wrapper::PipelineStateObject::GraphicsPipelineStateDesc& Desc)
				{
					auto FillDepthStencilOperation = [](const IDevice::State::FaceState& State, D3D12_DEPTH_STENCILOP_DESC& Desc)
					{
						Desc.StencilFailOp = GetStencilOperation(State.StencilOperationStencilFailed);
						Desc.StencilDepthFailOp = GetStencilOperation(State.StencilOperationDepthFailed);
						Desc.StencilPassOp = GetStencilOperation(State.StencilOperationDepthPassed);
						Desc.StencilFunc = GetComparisonFunction(State.StencilTestFunction);
					};

					D3D12_RASTERIZER_DESC rasterizerDesc = {};
					{
						rasterizerDesc.FrontCounterClockwise = (State.FaceOrder == IDevice::FaceOrders::Clockwise ? false : true);
						rasterizerDesc.CullMode = GetCullMode(State.CullMode);
						rasterizerDesc.FillMode = GetFillMode(State.GetFaceState(State.CullMode).PolygonMode);
						rasterizerDesc.DepthClipEnable = (State.DepthTestFunction != IDevice::TestFunctions::Never);

						Desc.RasterizerState = rasterizerDesc;
					}

					D3D12_DEPTH_STENCIL_DESC1 depthStencilDesc = {};
					{
						depthStencilDesc.DepthEnable = (State.DepthTestFunction != IDevice::TestFunctions::Never);
						depthStencilDesc.DepthWriteMask = (State.DepthTestFunction == IDevice::TestFunctions::Never ? D3D12_DEPTH_WRITE_MASK_ZERO : D3D12_DEPTH_WRITE_MASK_ALL);
						depthStencilDesc.DepthFunc = GetComparisonFunction(State.DepthTestFunction);

						FillDepthStencilOperation(State.FrontFaceState, depthStencilDesc.FrontFace);
						FillDepthStencilOperation(State.BackFaceState, depthStencilDesc.BackFace);

						depthStencilDesc.StencilWriteMask = State.FrontFaceState.StencilTestFunctionMask;

						depthStencilDesc.StencilReadMask = State.GetFaceState(State.CullMode).StencilMask;

						Desc.DepthStencil = depthStencilDesc;
					}

					D3D12_BLEND_DESC blendDesc = {};
					{
						for (uint8 i = 0; i < m_CurrentRenderTargetViewCount; ++i)
						{
							D3D12_RENDER_TARGET_BLEND_DESC& desc = blendDesc.RenderTarget[i];

							desc.BlendEnable = !(State.BlendFunctionSourceFactor == BlendFunctions::One && State.BlendFunctionDestinationFactor == BlendFunctions::Zero);
							desc.BlendOp = desc.BlendOpAlpha = GetBlendEquation(State.BlendEquation);
							desc.SrcBlend = desc.SrcBlendAlpha = GetBlendFunction(State.BlendFunctionSourceFactor);
							desc.DestBlend = desc.DestBlendAlpha = GetBlendFunction(State.BlendFunctionDestinationFactor);
							desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
						}

						Desc.BlendState = blendDesc;
					}

					D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
					{
						inputLayoutDesc.NumElements = m_InputLayoutCount;
						inputLayoutDesc.pInputElementDescs = m_InputLayout;

						Desc.InputLayout = inputLayoutDesc;
					}

					Desc.PrimitiveToplogy = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

					if (m_CurrentDepthStencilView != nullptr)
						Desc.DepthStencilFormat = m_CurrentDepthStencilView->Format;

					D3D12_RT_FORMAT_ARRAY rtvFormats = {};
					{
						rtvFormats.NumRenderTargets = m_CurrentRenderTargetViewCount;

						for (uint8 i = 0; i < m_CurrentRenderTargetViewCount; ++i)
							rtvFormats.RTFormats[i] = m_CurrentRenderTargetViews[i]->Format;

						Desc.RenderTargetFormats = rtvFormats;
					}
				}

				uint32 DirectX12Device::GetStateHash(void)
				{
					uint32 hash = 0;

					Hash::CRC32(&m_State, 1, hash);

					hash += (m_CurrentDepthStencilView == nullptr ? 0 : m_CurrentDepthStencilView->Format);

					for (uint8 i = 0; i < m_CurrentRenderTargetViewCount; ++i)
						hash += m_CurrentRenderTargetViews[i]->Format;

					return hash;
				}
			}

#undef CHECK_CALL
#undef INITIALIZE_RESOURCE_INFO
#undef REALLOCATE_SAMPLER
#undef BEGIN_UPLOAD
#undef END_UPLOAD
#undef FILL_RENDER_VIEWS_USING_CONTEXT()
#undef ADD_TRANSITION_STATE_FOR_TARGET_BUFFERS
#undef SKIP_NEXT_FRAMES
		}
	}
}