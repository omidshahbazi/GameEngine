// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\DirectX12\DirectX12Device.h>
#include <Rendering\Private\Helper.h>
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
			namespace DirectX12
			{
#define CHECK_CALL(Expr) (!(!(Expr) && RaiseDebugMessages(m_InfoQueue, this)))

#define INITIALIZE_RESOURCE_INFO(ResourceInfoPtr, ResourcePtr, State) \
				(ResourceInfoPtr)->Resource = ResourcePtr; \
				(ResourceInfoPtr)->View = {}; \
				(ResourceInfoPtr)->PrevState = State;

#define BEGIN_UPLOAD() \
				byte* buffer = nullptr; \
				if (!CHECK_CALL(DirectX12Wrapper::MapResource(m_UploadBuffer.Resource, &buffer))) \
					return false; \
				PlatformMemory::Set(buffer, 0, UPLAOD_BUFFER_SIZE);

#define END_UPLOAD(BufferType, MainResourceInfo, DestinationIsABuffer) \
				if (!CHECK_CALL(DirectX12Wrapper::UnmapResource(m_UploadBuffer.Resource))) \
					return false; \
				if (!CopyBuffer(BufferType, &m_UploadBuffer, true, MainResourceInfo, DestinationIsABuffer)) \
					return false;

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

				DXGI_FORMAT GetTextureFormat(Texture::Formats Format)
				{
					switch (Format)
					{
					case Texture::Formats::R8:
						return DXGI_FORMAT_R8_UINT;
					case Texture::Formats::R16:
						return DXGI_FORMAT_R16_UINT;
					case Texture::Formats::R32:
						return DXGI_FORMAT_R32_UINT;
					case Texture::Formats::R16F:
						return DXGI_FORMAT_R16_FLOAT;
					case Texture::Formats::R32F:
						return DXGI_FORMAT_R32_FLOAT;
					case Texture::Formats::RG8:
						return DXGI_FORMAT_R8G8_UINT;
					case Texture::Formats::RG16:
						return DXGI_FORMAT_R16G16_UINT;
					case Texture::Formats::RG32:
						return DXGI_FORMAT_R32G32_UINT;
					case Texture::Formats::RG16F:
						return DXGI_FORMAT_R16G16_FLOAT;
					case Texture::Formats::RG32F:
						return DXGI_FORMAT_R32G32_FLOAT;
					case Texture::Formats::RGB8:
						return DXGI_FORMAT_R8G8B8A8_UINT;
					case Texture::Formats::RGB16:
						return DXGI_FORMAT_R16G16B16A16_UINT;
					case Texture::Formats::RGB32:
						return DXGI_FORMAT_R32G32B32A32_UINT;
					case Texture::Formats::RGB16F:
						return DXGI_FORMAT_R16G16B16A16_FLOAT;
					case Texture::Formats::RGB32F:
						return DXGI_FORMAT_R32G32B32A32_FLOAT;
					case Texture::Formats::RGBA8:
						return DXGI_FORMAT_R8G8B8A8_UINT;
					case Texture::Formats::RGBA16:
						return DXGI_FORMAT_R16G16B16A16_UINT;
					case Texture::Formats::RGBA32:
						return DXGI_FORMAT_R32G32B32A32_UINT;
					case Texture::Formats::RGBA16F:
						return DXGI_FORMAT_R16G16B16A16_FLOAT;
					case Texture::Formats::RGBA32F:
						return DXGI_FORMAT_R32G32B32A32_FLOAT;
					case Texture::Formats::Depth16:
						return DXGI_FORMAT_D16_UNORM;
					case Texture::Formats::Depth24:
						return DXGI_FORMAT_D24_UNORM_S8_UINT;
					case Texture::Formats::Depth32:
						return DXGI_FORMAT_D24_UNORM_S8_UINT;
					case Texture::Formats::Depth32F:
						return DXGI_FORMAT_D32_FLOAT;
					case Texture::Formats::Stencil24F:
						return DXGI_FORMAT_D24_UNORM_S8_UINT;
					case Texture::Formats::Stencil32F:
						return DXGI_FORMAT_D32_FLOAT;
					}

					return DXGI_FORMAT_UNKNOWN;
				}

				uint8 GetTextureFormatPadding(Texture::Formats Format)
				{
					switch (Format)
					{
					case Texture::Formats::R8:
					case Texture::Formats::R16:
					case Texture::Formats::R16F:
					case Texture::Formats::Depth16:
					case Texture::Formats::Depth24:
					case Texture::Formats::Stencil24F:
					case Texture::Formats::R32:
					case Texture::Formats::R32F:
					case Texture::Formats::Depth32:
					case Texture::Formats::Depth32F:
					case Texture::Formats::Stencil32F:
					case Texture::Formats::RG8:
					case Texture::Formats::RG16:
					case Texture::Formats::RG16F:
					case Texture::Formats::RG32:
					case Texture::Formats::RG32F:
						return 0;

					case Texture::Formats::RGB8:
					case Texture::Formats::RGB16:
					case Texture::Formats::RGB16F:
					case Texture::Formats::RGB32:
					case Texture::Formats::RGB32F:
						return Texture::GetChannelSize(Format);
					}

					return 0;
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

				bool RaiseDebugMessages(ID3D12InfoQueue* InfoQueue, DirectX12Device* Device)
				{
					if (InfoQueue == nullptr)
						return true;

					IDevice::DebugFunction procedure = Device->GetDebugCallback();
#ifndef DEBUG_MODE
					if (procedure == nullptr)
						return true;
#endif

					DirectX12Wrapper::IterateOverDebugMessages(InfoQueue,
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
							case D3D12_MESSAGE_CATEGORY_SHADER:					source = IDevice::DebugSources::ShaderCompiler; break;
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
					m_Device(nullptr),
					m_InfoQueue(nullptr),
					m_RootSignature(nullptr),
					m_CurrentContextHandle(0),
					m_CurrentContext(nullptr),
					m_CurrentViewCount(0),
					m_CurrentRenderTarget(nullptr)
				{
					PlatformMemory::Set(&m_CopyCommandSet, 0, 1);
					PlatformMemory::Set(&m_RenderCommandSet, 0, 1);
					PlatformMemory::Set(&m_UploadBuffer, 0, 1);
					PlatformMemory::Set(&m_Viewport, 0, 1);
				}

				DirectX12Device::~DirectX12Device(void)
				{
					if (!m_Initialized)
						return;

					m_MemoryManager.DeallocateBuffer(m_UploadBuffer.Resource);

					DestroyCommandSet(m_RenderCommandSet);
					DestroyCommandSet(m_CopyCommandSet);
#if DEBUG_MODE
					if (DirectX12Wrapper::ReleaseInstance(m_InfoQueue))
						return;
#endif

					if (DirectX12Wrapper::ReleaseInstance(m_Device))
						return;

					if (DirectX12Wrapper::ReleaseInstance(m_Factory))
						return;
				}

				bool DirectX12Device::Initialize(void)
				{
#if DEBUG_MODE
					if (!DirectX12Wrapper::EnableDebugLayer())
						return false;

					if (!DirectX12Wrapper::CreateFactory(true, &m_Factory))
						return false;
#else
					if (!DirectX12Wrapper::CreateFactory(false, &m_Factory))
						return false;
#endif

					if (!DirectX12Wrapper::FindBestAdapter(m_Factory, &m_Adapter, &m_AdapterDesc))
						return false;

					if (!DirectX12Wrapper::CreateDevice(m_Adapter, &m_Device))
						return false;

#if DEBUG_MODE
					if (!DirectX12Wrapper::GetInfoQueue(m_Device, &m_InfoQueue))
						return false;
#endif

					if (!CreateCommandSet(m_CopyCommandSet, D3D12_COMMAND_LIST_TYPE_COPY))
						return false;

					if (!CreateCommandSet(m_RenderCommandSet, D3D12_COMMAND_LIST_TYPE_DIRECT))
						return false;

					DirectX12Wrapper::RootSignatureDesc desc = {};

					desc.ParameterCount = 2;

					DirectX12Wrapper::RootSignatureDesc::ParameterDesc& matrixParameter = desc.Parameters[0];
					matrixParameter.ParameterType = DirectX12Wrapper::RootSignatureDesc::ParameterTypes::Constants;
					matrixParameter.ShaderVisibility = DirectX12Wrapper::RootSignatureDesc::ShaderVisibilities::All;
					matrixParameter.Constants.ShaderRegister = 0;
					matrixParameter.Constants.ValueCount = sizeof(Matrix4F) / sizeof(float32);

					DirectX12Wrapper::RootSignatureDesc::ParameterDesc& samplerParameter = desc.Parameters[1];
					samplerParameter.ParameterType = DirectX12Wrapper::RootSignatureDesc::ParameterTypes::DescriptorTable;
					samplerParameter.ShaderVisibility = DirectX12Wrapper::RootSignatureDesc::ShaderVisibilities::Pixel;
					samplerParameter.DescriptorTable.DescriptorRangeCount = 1;
					samplerParameter.DescriptorTable.DescriptorRanges[0].BaseShaderRegister = 0;
					samplerParameter.DescriptorTable.DescriptorRanges[0].DescriptorCount = 2;
					samplerParameter.DescriptorTable.DescriptorRanges[0].Type = DirectX12Wrapper::RootSignatureDesc::DescriptorRangeTypes::ShaderResourceView;




					//rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
					//rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
					//rootParameters[1].Constants.ShaderRegister = 0;
					//rootParameters[1].Constants.RegisterSpace = 0;
					//rootParameters[1].Constants.Num32BitValues = sizeof(Matrix4F) / sizeof(float32);

					//rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
					//rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
					//rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
					////rootParameters[2].DescriptorTable.pDescriptorRanges->s


					cstr errorMessage = nullptr;
					if (!CHECK_CALL(DirectX12Wrapper::CreateRootSignature(m_Device, &desc, &m_RootSignature, &errorMessage)))
						return false;

					if (!CHECK_CALL(m_MemoryManager.Initialize(m_Device)))
						return false;

					if (!CHECK_CALL(m_RenderTargetViewAllocator.Initialize(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV)))
						return false;

					if (!CHECK_CALL(m_DepthStencilViewAllocator.Initialize(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV)))
						return false;

					if (!CHECK_CALL(m_SamplerAllocator.Initialize(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)))
						return false;

					if (!CreateIntermediateBuffer(UPLAOD_BUFFER_SIZE, &m_UploadBuffer))
						return false;

					ResetState();

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
					if (!CHECK_CALL(DirectX12Wrapper::CreateSwapChain(m_Factory, m_RenderCommandSet.Queue, WindowHandle, BACK_BUFFER_COUNT, &swapChain)))
						return false;

					ID3D12Resource1* backBuffers[BACK_BUFFER_COUNT];
					if (!CHECK_CALL(DirectX12Wrapper::GetSwapChainBackBuffers(swapChain, BACK_BUFFER_COUNT, backBuffers)))
						return false;

					DescriptorViewAllocator::ViewHandle handles[BACK_BUFFER_COUNT];
					for (uint8 i = 0; i < BACK_BUFFER_COUNT; ++i)
						if (!CHECK_CALL(m_RenderTargetViewAllocator.AllocateRenderTargetView(backBuffers[i], &handles[i])))
							return false;

					//D3D12_RESOURCE_DESC bufferDesc = backBuffers[0]->GetDesc();

					//ID3D12Resource* depthStencilBuffer = nullptr;
					//if (!CHECK_CALL(DirectX12Wrapper::CreateTexture(m_Device, GetTextureType(Texture::Types::TwoD), bufferDesc.Width, bufferDesc.Height, GetTextureFormat(Texture::Formats::Depth24), D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, true, &depthStencilBuffer)))
					//	return false;

					//if (!CHECK_CALL(DirectX12Wrapper::CreateDepthStencilView(m_Device, depthStencilBuffer, depthStencilDescriptorHeap, 0, m_DepthStencilViewDescriptorSize)))
					//	return false;

					Handle = (RenderContext::Handle)WindowHandle;

					RenderContextInfo* info = RenderingAllocators::RenderingSystemAllocator_Allocate<RenderContextInfo>();
					PlatformMemory::Set(info, 0, 1);

					info->SwapChain = swapChain;

					for (uint8 i = 0; i < BACK_BUFFER_COUNT; ++i)
					{
						ViewInfo& view = info->Views[i];

						INITIALIZE_RESOURCE_INFO(&view, backBuffers[i], D3D12_RESOURCE_STATE_COMMON);

						view.Point = (RenderTarget::AttachmentPoints)((uint8)RenderTarget::AttachmentPoints::Color0 + i);
						view.View = handles[i];
					}

					info->BackBufferCount = BACK_BUFFER_COUNT;
					info->CurrentBackBufferIndex = 0;

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

					if (!CHECK_CALL(DirectX12Wrapper::ReleaseInstance(info->SwapChain)))
						return false;

					RenderingAllocators::RenderingSystemAllocator_Deallocate(info);

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

						m_CurrentViewCount = 0;

						return true;
					}

					if (!m_Contexts.Contains(Handle))
						return false;

					RenderContextInfo* info = m_Contexts[Handle];

					m_CurrentContextHandle = Handle;
					m_CurrentContext = info;

					if (m_CurrentRenderTarget != nullptr)
					{
						m_CurrentViews[0] = m_CurrentContext->GetCurrentView();
						m_CurrentViewCount = 1;
					}

					ResetState();

					return true;
				}

				bool DirectX12Device::SetViewport(const Vector2I& Position, const Vector2I& Size)
				{
					m_Viewport.TopLeftX = Position.X;
					m_Viewport.TopLeftY = Position.Y;
					m_Viewport.Width = Size.X;
					m_Viewport.Height = Size.Y;

					return true;
				}

				bool DirectX12Device::SetClearColor(const ColorUI8& Color)
				{
					m_ClearColor = Color;

					return true;
				}

				bool DirectX12Device::SetFaceOrder(FaceOrders Order)
				{
					return true;
				}

				bool DirectX12Device::SetCullMode(CullModes Mode)
				{
					return true;
				}

				bool DirectX12Device::SetDepthTestFunction(TestFunctions Function)
				{
					return true;
				}

				bool DirectX12Device::SetStencilTestFunction(CullModes CullMode, TestFunctions Function, int32 Reference, uint32 Mask)
				{
					return true;
				}

				bool DirectX12Device::SetStencilMask(CullModes CullMode, uint32 Mask)
				{
					return true;
				}

				bool DirectX12Device::SetStencilOperation(CullModes CullMode, StencilOperations StencilFailed, StencilOperations DepthFailed, StencilOperations DepthPassed)
				{
					return true;
				}

				bool DirectX12Device::SetBlendEquation(BlendEquations Equation)
				{
					return true;
				}

				bool DirectX12Device::SetBlendFunction(BlendFunctions SourceFactor, BlendFunctions DestinationFactor)
				{
					return true;
				}

				bool DirectX12Device::SetPolygonMode(CullModes CullMode, PolygonModes PolygonMode)
				{
					return true;
				}

				bool DirectX12Device::SetResourceName(NativeType::Handle Handle, ResourceTypes Type, cwstr Name) //HITODO: should cast based on Type
				{
					if (Type == ResourceTypes::Mesh)
					{
						MeshBufferInfo* meshBufferInfo = ReinterpretCast(MeshBufferInfo*, Handle);

						WString tempName(Name);

						if (!CHECK_CALL(DirectX12Wrapper::SetObjectName(meshBufferInfo->VertexBuffer.Resource, (tempName + L"_VertexBuffer").GetValue())))
							return false;

						if (!CHECK_CALL(DirectX12Wrapper::SetObjectName(meshBufferInfo->IndexBuffer.Resource, (tempName + L"_IndexBuffer").GetValue())))
							return false;
					}
					else if (Type == ResourceTypes::RenderTarget)
					{
						RenderTargetInfos* renderTargetInfos = ReinterpretCast(RenderTargetInfos*, Handle);

						WString tempName(Name);

						uint8 index = 0;
						for (auto view : renderTargetInfos->Views)
						{
							if (!CHECK_CALL(DirectX12Wrapper::SetObjectName(view.Resource, (tempName + L"_Texture").GetValue())))
								return false;
						}
					}
					else if (Type == ResourceTypes::Shader)
					{
						ShaderInfos* shaderInfos = ReinterpretCast(ShaderInfos*, Handle);

						WString tempName(Name);

						if (!CHECK_CALL(DirectX12Wrapper::SetObjectName(shaderInfos->Pipeline, (tempName + L"_Pipeline").GetValue())))
							return false;
					}
					else
					{
						ResourceInfo* resourceInfo = ReinterpretCast(ResourceInfo*, Handle);

						if (!CHECK_CALL(DirectX12Wrapper::SetObjectName(resourceInfo->Resource, Name)))
							return false;
					}

					return true;
				}

				bool DirectX12Device::CreateBuffer(GPUBuffer::Handle& Handle)
				{
					BoundBuffersInfo* info = RenderingAllocators::RenderingSystemAllocator_Allocate<BoundBuffersInfo>();
					INITIALIZE_RESOURCE_INFO(&info->Buffer, nullptr, D3D12_RESOURCE_STATE_COMMON);
					info->Buffer.Size = 0;
					info->Buffer.Stride = 0;
					info->Resource = nullptr;

					Handle = (GPUBuffer::Handle)info;

					return true;
				}

				bool DirectX12Device::DestroyBuffer(GPUBuffer::Handle Handle)
				{
					if (Handle == 0)
						return false;

					BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

					if (boundBufferInfo->Buffer.Resource != nullptr)
						if (!CHECK_CALL(m_MemoryManager.DeallocateBuffer(boundBufferInfo->Buffer.Resource)))
							return false;

					RenderingAllocators::RenderingSystemAllocator_Deallocate(boundBufferInfo);

					return true;
				}

				bool DirectX12Device::BindBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type)
				{
					return true;
				}

				bool DirectX12Device::CopyFromVertexToBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Usages Usage, SubMesh::Handle FromMeshHandle, uint32 Size)
				{
					return true;
				}

				bool DirectX12Device::CopyFromIndexoBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Usages Usage, SubMesh::Handle FromMeshHandle, uint32 Size)
				{
					return true;
				}

				bool DirectX12Device::CopyFromTextureToBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Usages Usage, Texture::Handle FromTextureHandle, uint32 Size, Texture::Types TextureType, Texture::Formats TextureFormat, uint32 Level)
				{
					if (Handle == 0)
						return false;

					if (FromTextureHandle == 0)
						return false;

					BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

					if (!CreateIntermediateBuffer(Size, &boundBufferInfo->Buffer))
						return false;

					boundBufferInfo->Resource = ReinterpretCast(ResourceInfo*, FromTextureHandle);

					return true;
				}

				bool DirectX12Device::CopyFromBufferToVertex(GPUBuffer::Handle Handle, GPUBuffer::Types Type, Texture::Handle ToMeshHandle, uint32 Size)
				{
					return false;
				}

				bool DirectX12Device::CopyFromBufferToIndex(GPUBuffer::Handle Handle, GPUBuffer::Types Type, Texture::Handle ToMeshHandle, uint32 Size)
				{
					return false;
				}

				bool DirectX12Device::CopyFromBufferToTexture(GPUBuffer::Handle Handle, GPUBuffer::Types Type, Texture::Handle ToTextureHandle, Texture::Types TextureType, uint32 Width, uint32 Height, Texture::Formats TextureFormat)
				{
					if (Handle == 0)
						return false;

					BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

					return CopyBuffer(Type, &boundBufferInfo->Buffer, true, boundBufferInfo->Resource, false);
				}

				//HITODO: think about RGB format layout in Buffer
				bool DirectX12Device::LockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Access Access, byte** Buffer)
				{
					if (Handle == 0)
						return false;

					BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

					if (boundBufferInfo->Resource == nullptr)
						return false;

					if (!CopyBuffer(Type, boundBufferInfo->Resource, false, &boundBufferInfo->Buffer, true))
						return false;

					return CHECK_CALL(DirectX12Wrapper::MapResource(boundBufferInfo->Buffer.Resource, Buffer));
				}

				bool DirectX12Device::UnlockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type)
				{
					if (Handle == 0)
						return false;

					BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

					return CHECK_CALL(DirectX12Wrapper::UnmapResource(boundBufferInfo->Buffer.Resource));
				}

				bool DirectX12Device::CreateShader(const Shaders* Shaders, Shader::Handle& Handle, cstr* ErrorMessage)
				{
					ShaderInfos* shaderInfo = RenderingAllocators::RenderingSystemAllocator_Allocate<ShaderInfos>();
					PlatformMemory::Set(shaderInfo, 0, 1);

					if (!CHECK_CALL(DirectX12Wrapper::CompileShader(Shaders->VertexShader, "vs_5_0", &shaderInfo->VertexShader, ErrorMessage)))
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::CompileShader(Shaders->FragmentShader, "ps_5_0", &shaderInfo->FragmentShader, ErrorMessage)))
						return false;

					D3D12_INPUT_ELEMENT_DESC inputLayout[] =
					{
						{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
						{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
					};

					DirectX12Wrapper::GraphicsPipelineStateDesc desc = {};
					desc.RootSignature = m_RootSignature;
					desc.InputLayout.NumElements = 2;
					desc.InputLayout.pInputElementDescs = inputLayout;
					desc.PrimitiveToplogyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
					desc.VertexShader = shaderInfo->VertexShader;
					desc.PixelShader = shaderInfo->FragmentShader;
					desc.DepthStencilFormat = DXGI_FORMAT_D32_FLOAT;

					if (!CHECK_CALL(DirectX12Wrapper::CreateGraphicsPipelineState(m_Device, &desc, sizeof(DirectX12Wrapper::GraphicsPipelineStateDesc), &shaderInfo->Pipeline)))
						return false;

					Handle = (Shader::Handle)shaderInfo;

					return true;
				}

				bool DirectX12Device::DestroyShader(Shader::Handle Handle)
				{
					return true;
				}

				bool DirectX12Device::BindShader(Shader::Handle Handle)
				{
					return true;
				}

				bool DirectX12Device::QueryShaderActiveConstants(Shader::Handle Handle, Shader::ConstantDataList& Constants)
				{
#define IMPLEMENT(ByteCode) \
					count = 0; \
					if (!CHECK_CALL(DirectX12Wrapper::ReflectShaderConstants(&ByteCode, variableDescs, VARIABLES_COUNT, &count))) \
						return false; \
					Constants.Extend(count); \
					for (uint8 i = 0; i < count; ++i) \
					{ \
						D3D12_SHADER_VARIABLE_DESC& desc = variableDescs[i]; \
						Shader::ConstantHandle handle = desc.StartOffset; \
						ShaderDataType::Types dataType = ShaderDataType::Types::Unknown; \
						AnyDataType value; \
						switch (desc.Size) \
						{ \
						case 4: \
						{ \
							dataType = ShaderDataType::Types::Float; \
							value = 0.0F; \
						} \
						break; \
						case 8: \
						{ \
							dataType = ShaderDataType::Types::Float2; \
							value = Vector2F(); \
						} \
						break; \
						case 12: \
						{ \
							dataType = ShaderDataType::Types::Float3; \
							value = Vector3F(); \
						} \
						break; \
						case 16: \
						{ \
							dataType = ShaderDataType::Types::Float4; \
							value = Vector4F(); \
						} \
						break; \
						case 64: \
						{ \
							dataType = ShaderDataType::Types::Matrix4; \
							value = Matrix4F::Identity; \
						} \
						break; \
						case 0: \
						{ \
							dataType = ShaderDataType::Types::Texture2D; \
							value = nullptr; \
						} \
						break; \
						} \
						Constants[i] = Shader::ConstantData(handle, desc.Name, dataType, value); \
					}

					if (Handle == 0)
						return false;

					ShaderInfos* shaderInfos = ReinterpretCast(ShaderInfos*, Handle);

					const uint8 VARIABLES_COUNT = 128;
					D3D12_SHADER_VARIABLE_DESC variableDescs[VARIABLES_COUNT];

					uint8 count = 0;

					IMPLEMENT(shaderInfos->VertexShader);
					IMPLEMENT(shaderInfos->FragmentShader);

					return true;

#undef IMPLEMENT
				}

				bool DirectX12Device::SetShaderFloat32(Shader::ConstantHandle Handle, float32 Value)
				{
					return true;
				}

				bool DirectX12Device::SetShaderColor(Shader::ConstantHandle Handle, const ColorUI8& Value)
				{
					return true;
				}

				bool DirectX12Device::SetShaderVector2(Shader::ConstantHandle Handle, const Vector2F& Value)
				{
					return true;
				}

				bool DirectX12Device::SetShaderVector3(Shader::ConstantHandle Handle, const Vector3F& Value)
				{
					return true;
				}

				bool DirectX12Device::SetShaderVector4(Shader::ConstantHandle Handle, const Vector4F& Value)
				{
					return true;
				}

				bool DirectX12Device::SetShaderMatrix4(Shader::ConstantHandle Handle, const Matrix4F& Value)
				{
					return true;
				}

				bool DirectX12Device::SetShaderTexture(Shader::ConstantHandle Handle, Texture::Types Type, Texture::Handle Value)
				{
					return true;
				}

				bool DirectX12Device::CreateTexture(const TextureInfo* Info, Texture::Handle& Handle)
				{
					D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

					ID3D12Resource1* resource = nullptr;

					if (Info->Type == Texture::Types::TwoD)
					{
						if (!CHECK_CALL(m_MemoryManager.AllocateTexture2D(Info->Dimension.X, Info->Dimension.Y, GetTextureFormat(Info->Format), state, false, &resource)))
							return false;
					}
					else
						return false;

					ResourceInfo* info = RenderingAllocators::RenderingSystemAllocator_Allocate<ResourceInfo>();
					INITIALIZE_RESOURCE_INFO(info, resource, state);

					if (Info->Data != nullptr)
					{
						BEGIN_UPLOAD();

						uint32 dataPitch = Texture::GetRowPitch(Info->Format, Info->Dimension.X);
						uint8 pixelSize = Texture::GetPixelSize(Info->Format);

						uint32 resourcePitch = DirectX12Wrapper::GetResourceRowPitch(m_Device, resource);
						uint8 padding = GetTextureFormatPadding(Info->Format);

						for (int32 y = 0; y < Info->Dimension.Y; ++y)
							for (int32 x = 0; x < Info->Dimension.X; ++x)
								PlatformMemory::Copy(Info->Data + (y * dataPitch) + (x * pixelSize), buffer + (y * resourcePitch) + (x * (pixelSize + padding)), pixelSize);

						END_UPLOAD(GPUBuffer::Types::PixelUnpack, info, false);
					}

					if (!m_SamplerAllocator.AllocateUnorderedAccessView(resource, &info->View))
						return false;

					Handle = (Texture::Handle)info;

					return true;
				}

				bool DirectX12Device::DestroyTexture(Texture::Handle Handle)
				{
					if (Handle == 0)
						return false;

					ResourceInfo* resourceInfo = ReinterpretCast(ResourceInfo*, Handle);

					//LOTODO: Reimpl. to handle 3D textures
					if (!CHECK_CALL(m_MemoryManager.DeallocateTexture2D(resourceInfo->Resource)))
						return false;

					RenderingAllocators::RenderingSystemAllocator_Deallocate(resourceInfo);

					return true;
				}

				bool DirectX12Device::BindTexture(Texture::Handle Handle, Texture::Types Type)
				{
					return true;
				}

				bool DirectX12Device::SetTextureVerticalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode)
				{
					return true;
				}

				bool DirectX12Device::SetTextureHorizontalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode)
				{
					return true;
				}

				bool DirectX12Device::SetTextureMinifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MinifyFilters Filter)
				{
					return true;
				}

				bool DirectX12Device::SetTextureMagnifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MagnfyFilters Filter)
				{
					return true;
				}

				bool DirectX12Device::GenerateTextureMipMap(Texture::Handle Handle, Texture::Types Type)
				{
					return true;
				}

				bool DirectX12Device::CreateRenderTarget(const RenderTargetInfo* Info, RenderTarget::Handle& Handle, TextureList& Textures)
				{
					//HITODO: channge Textures.Add((Texture::Handle)resource); to add resourceInfo*
#define CREATE_VIEW(IsColored, CurrnetState) \
					{ \
						RenderTargetInfos::ViewList viewList; \
						for (const auto & textureInfo : Info->Textures) \
						{ \
							if (!RenderTarget::IsColorPoint(textureInfo.Point) == IsColored) \
								continue; \
							ID3D12Resource1* resource = nullptr; \
							if (!CHECK_CALL(m_MemoryManager.AllocateRenderTarget(textureInfo.Dimension.X, textureInfo.Dimension.Y, GetTextureFormat(textureInfo.Format), IsColored, CurrnetState, false, &resource))) \
								return false; \
							ViewInfo view = {}; \
							INITIALIZE_RESOURCE_INFO(&view, resource, CurrnetState); \
							view.Point = textureInfo.Point; \
							viewList.Add(view); \
							Textures.Add((Texture::Handle)resource); \
							if (IsColored) \
							{ \
								if (!CHECK_CALL(m_RenderTargetViewAllocator.AllocateRenderTargetView(view.Resource, &view.View))) \
									return false; \
							} \
							else \
								if (!CHECK_CALL(m_DepthStencilViewAllocator.AllocateDepthStencilView(view.Resource, &view.View))) \
									return false; \
						} \
					}

					if (Info->Textures.GetSize() == 0)
						return false;

					RenderTargetInfos* renderTargetInfos = RenderingAllocators::RenderingSystemAllocator_Allocate<RenderTargetInfos>();
					PlatformMemory::Set(renderTargetInfos, 0, 1);

					CREATE_VIEW(true, D3D12_RESOURCE_STATE_COMMON);
					CREATE_VIEW(false, D3D12_RESOURCE_STATE_COMMON);

					Handle = (RenderTarget::Handle)renderTargetInfos;

					return true;

#undef CREATE_VIEW
				}

				bool DirectX12Device::DestroyRenderTarget(RenderTarget::Handle Handle)
				{
					if (Handle == 0)
						return false;

					RenderTargetInfos* renderTargetInfos = ReinterpretCast(RenderTargetInfos*, Handle);

					return true;
				}

				bool DirectX12Device::BindRenderTarget(RenderTarget::Handle Handle)
				{
					if (Handle == 0)
					{
						m_CurrentRenderTarget = nullptr;
						m_CurrentViewCount = 0;

						if (m_CurrentContext != nullptr)
						{
							m_CurrentViews[0] = m_CurrentContext->GetCurrentView();
							m_CurrentViewCount = 1;
						}

						return true;
					}

					m_CurrentRenderTarget = ReinterpretCast(RenderTargetInfos*, Handle);;

					m_CurrentViewCount = m_CurrentRenderTarget->Views.GetSize();
					for (uint8 i = 0; i < m_CurrentViewCount; ++i)
						m_CurrentViews[i] = &m_CurrentRenderTarget->Views[i];

					//m_CommandList->OMSetRenderTargets()

					return true;
				}

				bool DirectX12Device::CreateMesh(const SubMeshInfo* Info, GPUBuffer::Usages Usage, SubMesh::Handle& Handle)
				{
					if (Info->Vertices.GetSize() == 0)
						return false;

					D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;

					MeshBufferInfo* info = RenderingAllocators::RenderingSystemAllocator_Allocate<MeshBufferInfo>();
					INITIALIZE_RESOURCE_INFO(&info->VertexBuffer, nullptr, state);
					INITIALIZE_RESOURCE_INFO(&info->IndexBuffer, nullptr, state);

					uint32 bufferSize = SubMesh::GetVertexBufferSize(Info->Vertices.GetSize());

					ID3D12Resource1* vertexResource = nullptr;
					if (!CHECK_CALL(m_MemoryManager.AllocateBuffer(bufferSize, state, false, &vertexResource)))
						return true;

					INITIALIZE_RESOURCE_INFO(&info->VertexBuffer, vertexResource, state);
					info->VertexBuffer.Size = bufferSize;
					info->VertexBuffer.Stride = sizeof(Vertex);

					{
						BEGIN_UPLOAD();

						PlatformMemory::Copy(ReinterpretCast(const byte*, Info->Vertices.GetData()), buffer, bufferSize);

						END_UPLOAD(GPUBuffer::Types::Array, &info->VertexBuffer, true);
					}

					bufferSize = SubMesh::GetVertexBufferSize(Info->Indices.GetSize());

					if (Info->Indices.GetSize() != 0)
					{
						ID3D12Resource1* indexResource = nullptr;
						if (!CHECK_CALL(m_MemoryManager.AllocateBuffer(bufferSize, state, false, &indexResource)))
							return true;

						INITIALIZE_RESOURCE_INFO(&info->IndexBuffer, indexResource, state);
						info->IndexBuffer.Size = bufferSize;
						info->IndexBuffer.Stride = sizeof(uint32);

						{
							BEGIN_UPLOAD();

							PlatformMemory::Copy(ReinterpretCast(const byte*, Info->Indices.GetData()), buffer, bufferSize);

							END_UPLOAD(GPUBuffer::Types::ElementArray, &info->IndexBuffer, true);
						}
					}

					Handle = (SubMesh::Handle)info;

					return true;
				}

				bool DirectX12Device::DestroyMesh(SubMesh::Handle Handle)
				{
					return true;
				}

				bool DirectX12Device::BindMesh(SubMesh::Handle Handle)
				{
					if (Handle == 0)
						return false;

					MeshBufferInfo* meshBufferInfo = ReinterpretCast(MeshBufferInfo*, Handle);

					BufferInfo& bufferInfo = meshBufferInfo->VertexBuffer;

					if (!CHECK_CALL(DirectX12Wrapper::AddSetVertexBufferCommand(m_RenderCommandSet.List, bufferInfo.Resource, bufferInfo.Size, bufferInfo.Stride)))
						return false;

					bufferInfo = meshBufferInfo->IndexBuffer;

					if (bufferInfo.Resource != nullptr)
						if (!CHECK_CALL(DirectX12Wrapper::AddSetIndexBufferCommand(m_RenderCommandSet.List, bufferInfo.Resource, bufferInfo.Size)))
							return false;

					return true;
				}

				bool DirectX12Device::Clear(ClearFlags Flags)
				{
					if (m_CurrentViewCount == 0)
						return false;

					bool shouldClearColor = BitwiseUtils::IsEnabled(Flags, ClearFlags::ColorBuffer);
					bool shouldClearDepth = BitwiseUtils::IsEnabled(Flags, ClearFlags::DepthBuffer);
					bool shouldClearStencil = BitwiseUtils::IsEnabled(Flags, ClearFlags::StencilBuffer);

					Vector4F color;
					Helper::GetNormalizedColor(m_ClearColor, color);

					for (uint8 i = 0; i < m_CurrentViewCount; ++i)
					{
						ViewInfo* view = m_CurrentViews[i];

						bool isColorPoint = RenderTarget::IsColorPoint(view->Point);

						bool shouldProceed = (shouldClearColor && isColorPoint) || (shouldClearDepth && view->Point == RenderTarget::AttachmentPoints::Depth) || (shouldClearStencil && view->Point == RenderTarget::AttachmentPoints::Stencil);

						if (!shouldProceed)
							continue;

						if (isColorPoint)
						{
							if (!AddTransitionResourceBarrier(m_RenderCommandSet, view, D3D12_RESOURCE_STATE_RENDER_TARGET))
								return false;

							if (!CHECK_CALL(DirectX12Wrapper::AddClearRenderTargetCommand(m_RenderCommandSet.List, view->View.CPUHandle, &color.X)))
								return false;

							continue;
						}

						if (!shouldClearDepth && !shouldClearStencil)
							continue;

						if (!AddTransitionResourceBarrier(m_RenderCommandSet, view, D3D12_RESOURCE_STATE_DEPTH_WRITE))
							return false;

						D3D12_CLEAR_FLAGS flags = (D3D12_CLEAR_FLAGS)0;
						if (shouldClearDepth) flags |= D3D12_CLEAR_FLAG_DEPTH;
						if (shouldClearStencil) flags |= D3D12_CLEAR_FLAG_STENCIL;

						if (!CHECK_CALL(DirectX12Wrapper::AddClearDepthStencilCommand(m_RenderCommandSet.List, view->View.CPUHandle, flags, 1, 1)))
							return false;
					}

					return true;
				}

				bool DirectX12Device::DrawIndexed(SubMesh::PolygonTypes PolygonType, uint32 IndexCount)
				{
					if (!CHECK_CALL(DirectX12Wrapper::AddSetPrimitiveTopologyCommand(m_RenderCommandSet.List, GetPolygonTopology(PolygonType))))
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::AddDrawIndexedCommand(m_RenderCommandSet.List, IndexCount)))
						return false;

					return true;
				}

				bool DirectX12Device::DrawArray(SubMesh::PolygonTypes PolygonType, uint32 VertexCount)
				{
					if (!CHECK_CALL(DirectX12Wrapper::AddSetPrimitiveTopologyCommand(m_RenderCommandSet.List, GetPolygonTopology(PolygonType))))
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::AddDrawCommand(m_RenderCommandSet.List, VertexCount)))
						return false;

					return true;
				}

				bool DirectX12Device::BeginExecute(void)
				{
					if (!CHECK_CALL(DirectX12Wrapper::AddSetViewportCommand(m_RenderCommandSet.List, &m_Viewport)))
						return false;

					return true;
				}

				bool DirectX12Device::EndExecute(void)
				{
					if (m_CurrentViewCount == 0)
						return false;

					for (uint8 i = 0; i < m_CurrentViewCount; ++i)
					{
						ViewInfo* view = m_CurrentViews[i];

						if (!AddTransitionResourceBarrier(m_RenderCommandSet, view, D3D12_RESOURCE_STATE_PRESENT))
							return false;
					}

					return ExecuteCommands(m_RenderCommandSet);
				}

				bool DirectX12Device::SwapBuffers(void)
				{
					if (m_CurrentContext == nullptr)
						return false;

					IDXGISwapChain4* swapChain = m_CurrentContext->SwapChain;

					if (!CHECK_CALL(DirectX12Wrapper::Present(swapChain)))
						return false;

					m_CurrentContext->CurrentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();

					if (m_CurrentRenderTarget == nullptr)
					{
						m_CurrentViews[0] = m_CurrentContext->GetCurrentView();
						m_CurrentViewCount = 1;
					}

					return true;
				}

				bool DirectX12Device::SetDebugCallback(DebugFunction Callback)
				{
					m_DebugCallback = Callback;

					return true;
				}

				bool DirectX12Device::SetFaceOrderInternal(FaceOrders Order)
				{
					return true;
				}

				bool DirectX12Device::SetCullModeInternal(CullModes Mode)
				{
					return true;
				}

				bool DirectX12Device::SetDepthTestFunctionInternal(TestFunctions Function)
				{
					return true;
				}

				bool DirectX12Device::SetStencilTestFunctionInternal(CullModes CullMode, TestFunctions Function, int32 Reference, uint32 Mask)
				{
					return true;
				}

				bool DirectX12Device::SetStencilMaskInternal(CullModes CullMode, uint32 Mask)
				{
					return true;
				}

				bool DirectX12Device::SetStencilOperationInternal(CullModes CullMode, StencilOperations StencilFailed, StencilOperations DepthFailed, StencilOperations DepthPassed)
				{
					return true;
				}

				bool DirectX12Device::SetBlendEquationInternal(BlendEquations Equation)
				{
					return true;
				}

				bool DirectX12Device::SetBlendFunctionInternal(BlendFunctions SourceFactor, BlendFunctions DestinationFactor)
				{
					return true;
				}

				bool DirectX12Device::SetPolygonModeInternal(CullModes CullMode, PolygonModes PolygonMode)
				{
					return true;
				}

				bool DirectX12Device::AddTransitionResourceBarrier(CommandSet& Set, ResourceInfo* Info, D3D12_RESOURCE_STATES AfterState)
				{
					if (Info->PrevState == AfterState)
						return true;

					if (!CHECK_CALL(DirectX12Wrapper::AddTransitionResourceBarrier(Set.List, Info->Resource, Info->PrevState, AfterState)))
						return false;

					Info->PrevState = AfterState;

					return true;
				}

				bool DirectX12Device::CreateIntermediateBuffer(uint32 Size, BufferInfo* Buffer)
				{
					if (Buffer->Size > Size)
						return true;

					D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COPY_DEST;

					if (Buffer->Resource != nullptr)
						if (!CHECK_CALL(m_MemoryManager.DeallocateBuffer(Buffer->Resource)))
							return false;

					if (!CHECK_CALL(m_MemoryManager.AllocateBuffer(Size, state, true, &Buffer->Resource)))
						return false;

					Buffer->PrevState = state;
					Buffer->Size = Size;

					return true;
				}

				bool DirectX12Device::CreateCommandSet(CommandSet& Set, D3D12_COMMAND_LIST_TYPE Type)
				{
					if (!CHECK_CALL(DirectX12Wrapper::CreateCommandQueue(m_Device, Type, &Set.Queue)))
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::CreateCommandAllocator(m_Device, Type, &Set.Allocator)))
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::CreateCommandList(m_Device, Set.Allocator, Type, &Set.List)))
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::CreateFence(m_Device, &Set.Fence)))
						return false;

					Set.FenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

					return true;
				}

				bool DirectX12Device::DestroyCommandSet(CommandSet& Set)
				{
					CloseHandle(Set.FenceEvent);

					if (!CHECK_CALL(DirectX12Wrapper::ReleaseInstance(Set.Fence)))
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::ReleaseInstance(Set.List)))
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::ReleaseInstance(Set.Allocator)))
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::ReleaseInstance(Set.Queue)))
						return false;

					return true;
				}

				bool DirectX12Device::ExecuteCommands(CommandSet& Set)
				{
					if (!CHECK_CALL(DirectX12Wrapper::ExecuteCommandList(Set.Queue, Set.List)))
						return false;

					uint64 waitValue;
					if (!CHECK_CALL(DirectX12Wrapper::IncrementFence(Set.Queue, Set.Fence, Set.FenceValue, waitValue)))
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::WaitForFence(Set.Fence, waitValue, Set.FenceEvent)))
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::ResetCommandAllocator(Set.Allocator)))
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::ResetCommandList(Set.List, Set.Allocator)))
						return false;

					return true;
				}

				bool DirectX12Device::CopyBuffer(GPUBuffer::Types Type, ResourceInfo* Source, bool SourceIsABuffer, ResourceInfo* Destination, bool DestinationIsABuffer)
				{
					if (!AddTransitionResourceBarrier(m_CopyCommandSet, Source, D3D12_RESOURCE_STATE_COPY_SOURCE))
						return false;

					if (!AddTransitionResourceBarrier(m_CopyCommandSet, Destination, D3D12_RESOURCE_STATE_COPY_DEST))
						return false;

					if (Type == GPUBuffer::Types::Array || Type == GPUBuffer::Types::ElementArray)
					{
						BufferInfo* bufferInfo = nullptr;
						if (DestinationIsABuffer)
							bufferInfo = ReinterpretCast(BufferInfo*, Destination);
						else if (SourceIsABuffer)
							bufferInfo = ReinterpretCast(BufferInfo*, Source);
						else
							return false;

						if (!CHECK_CALL(DirectX12Wrapper::AddCopyBufferCommand(m_CopyCommandSet.List, Source->Resource, Destination->Resource, bufferInfo->Size)))
							return false;
					}
					else
					{
						if (SourceIsABuffer && !DestinationIsABuffer)
						{
							if (!CHECK_CALL(DirectX12Wrapper::AddCopyBufferToTextureCommand(m_CopyCommandSet.List, Source->Resource, Destination->Resource)))
								return false;
						}
						else if (!SourceIsABuffer && DestinationIsABuffer)
						{
							if (!CHECK_CALL(DirectX12Wrapper::AddCopyTextureToBufferCommand(m_CopyCommandSet.List, Source->Resource, Destination->Resource)))
								return false;
						}
					}

					return ExecuteCommands(m_CopyCommandSet);
				}
			}

#undef CHECK_CALL
#undef INITIALIZE_RESOURCE_INFO
#undef BEGIN_UPLOAD
#undef END_UPLOADx
		}
	}
}