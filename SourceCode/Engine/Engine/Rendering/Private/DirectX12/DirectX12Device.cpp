// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\DirectX12\DirectX12Device.h>
#include <Rendering\Private\DirectX12\DirectX12Wrapper.h>
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
#define CHECK_CALL(Expr) !(!(Expr) && RaiseDebugMessages(m_InfoQueue, this))

				const uint8 BACK_BUFFER_COUNT = 2;

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
						[Device, procedure](D3D12_MESSAGE_ID ID, D3D12_MESSAGE_CATEGORY Category, cstr Message, D3D12_MESSAGE_SEVERITY Severity)
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
					m_ResourceHeap(nullptr),
					m_RenderTargetHeap(nullptr),
					m_BufferHeap(nullptr),
					m_RenderTargetViewDescriptorSize(0),
					m_DepthStencilViewDescriptorSize(0),
					m_CurrentContextHandle(0),
					m_CurrentContext(nullptr),
					m_CurrentViewCount(0),
					m_CurrentRenderTarget(nullptr)
				{
					PlatformMemory::Set(&m_CopyCommandSet, 0, 1);
					PlatformMemory::Set(&m_RenderCommandSet, 0, 1);
				}

				DirectX12Device::~DirectX12Device(void)
				{
					//HITODO: release other resources
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

					if (!CHECK_CALL(DirectX12Wrapper::CreateHeap(m_Device, MegaByte * 100, false, D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_BUFFERS, &m_ResourceHeap)))
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::CreateHeap(m_Device, MegaByte * 100, false, D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES, &m_RenderTargetHeap)))
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::CreateHeap(m_Device, MegaByte * 100, true, D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS, &m_BufferHeap)))
						return false;

					if (!CreateCommandSet(m_CopyCommandSet, D3D12_COMMAND_LIST_TYPE_COPY))
						return false;

					if (!CreateCommandSet(m_RenderCommandSet, D3D12_COMMAND_LIST_TYPE_DIRECT))
						return false;

					m_RenderTargetViewDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
					m_DepthStencilViewDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

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

					for each (auto & item in m_Contexts)
						if (item.GetFirst() == (RenderContext::Handle)WindowHandle)
							return false;

					IDXGISwapChain4* swapChain = nullptr;
					if (!CHECK_CALL(DirectX12Wrapper::CreateSwapChain(m_Factory, m_RenderCommandSet.Queue, WindowHandle, BACK_BUFFER_COUNT, &swapChain)))
						return false;

					ID3D12DescriptorHeap* descriptorHeap = nullptr;
					if (!CHECK_CALL(DirectX12Wrapper::CreateDescriptorHeap(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, BACK_BUFFER_COUNT, &descriptorHeap)))
						return false;

					ID3D12Resource* backBuffers[BACK_BUFFER_COUNT];
					if (!CHECK_CALL(DirectX12Wrapper::GetSwapChainBackBuffers(swapChain, BACK_BUFFER_COUNT, backBuffers)))
						return false;

					for (uint8 i = 0; i < BACK_BUFFER_COUNT; ++i)
						if (!CHECK_CALL(DirectX12Wrapper::CreateRenderTargetView(m_Device, backBuffers[i], descriptorHeap, i, m_RenderTargetViewDescriptorSize)))
							return false;

					//D3D12_RESOURCE_DESC bufferDesc = backBuffers[0]->GetDesc();

					//ID3D12Resource* depthStencilBuffer = nullptr;
					//if (!CHECK_CALL(DirectX12Wrapper::CreateTexture(m_Device, GetTextureType(Texture::Types::TwoD), bufferDesc.Width, bufferDesc.Height, GetTextureFormat(Texture::Formats::Depth24), D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, true, &depthStencilBuffer)))
					//	return false;

					//ID3D12DescriptorHeap* depthStencilDescriptorHeap = nullptr;
					//if (!CHECK_CALL(DirectX12Wrapper::CreateDescriptorHeap(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, &depthStencilDescriptorHeap)))
					//	return false;

					//if (!CHECK_CALL(DirectX12Wrapper::CreateDepthStencilView(m_Device, depthStencilBuffer, depthStencilDescriptorHeap, 0, m_DepthStencilViewDescriptorSize)))
					//	return false;

					Handle = (RenderContext::Handle)WindowHandle;

					RenderContextInfo* info = RenderingAllocators::RenderingSystemAllocator_Allocate<RenderContextInfo>();
					Construct(info);

					info->SwapChain = swapChain;

					for (uint8 i = 0; i < BACK_BUFFER_COUNT; ++i)
					{
						ViewInfo& view = info->Views[i];

						view.Point = (RenderTarget::AttachmentPoints)((uint8)RenderTarget::AttachmentPoints::Color0 + i);
						view.DescriptorHeap = descriptorHeap;
						view.Index = i;
						view.Resource = backBuffers[i];
						view.PrevState = D3D12_RESOURCE_STATE_COMMON;
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

					if (!CHECK_CALL(DirectX12Wrapper::ReleaseResource(info->SwapChain)))
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

				bool DirectX12Device::CreateBuffer(GPUBuffer::Handle& Handle)
				{
					BufferInfo* info = RenderingAllocators::RenderingSystemAllocator_Allocate<BufferInfo>();
					Construct(info);

					Handle = (GPUBuffer::Handle)info;

					return true;
				}

				bool DirectX12Device::DestroyBuffer(GPUBuffer::Handle Handle)
				{
					return true;
				}

				bool DirectX12Device::BindBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type)
				{
					return true;
				}

				bool DirectX12Device::AttachBufferData(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Usages Usage, uint32 Size, const void* Data)
				{
					return true;
				}

				bool DirectX12Device::AttachBufferData(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Usages Usage, uint32 Size, Texture::Handle TextureHandle, Texture::Types TextureType, Texture::Formats TextureFormat, uint32 Level)
				{
					if (Handle == 0)
						return false;

					if (TextureHandle == 0)
						return false;

					D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COPY_DEST;

					ID3D12Resource* bufferResource = nullptr;
					if (!CHECK_CALL(DirectX12Wrapper::CreateResource(m_Device, m_BufferHeap, D3D12_RESOURCE_DIMENSION_BUFFER, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, Size, 1, DXGI_FORMAT_UNKNOWN, D3D12_RESOURCE_FLAG_NONE, state, &bufferResource)))
						return false;

					BufferInfo* bufferInfo = ReinterpretCast(BufferInfo*, Handle);

					bufferInfo->Original = ReinterpretCast(ResourceInfo*, TextureHandle);

					bufferInfo->Buffer.Resource = bufferResource;
					bufferInfo->Buffer.PrevState = state;

					return true;
				}

				bool DirectX12Device::ReadBufferData(GPUBuffer::Handle Handle, GPUBuffer::Types Type, Texture::Handle TextureHandle, Texture::Types TextureType, uint32 Width, uint32 Height, Texture::Formats TextureFormat)
				{
					if (Handle == 0)
						return false;

					BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);

					if (!AddTransitionResourceBarrier(m_CopyCommandSet, info->Original, D3D12_RESOURCE_STATE_COPY_DEST))
						return false;

					if (Type == GPUBuffer::Types::ElementArray)
					{
						if (!CHECK_CALL(DirectX12Wrapper::AddCopyResourceCommand(m_CopyCommandSet.List, info->Buffer.Resource, info->Original->Resource)))
							return false;
					}
					else
					{
						if (!CHECK_CALL(DirectX12Wrapper::AddCopyBufferToTextureCommand(m_CopyCommandSet.List, info->Buffer.Resource, info->Original->Resource)))
							return false;
					}

					return ExecuteCommands(m_CopyCommandSet);
				}

				bool DirectX12Device::LockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Access Access, byte** Buffer)
				{
					if (Handle == 0)
						return false;

					BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);

					if (info->Original == nullptr)
						return false;

					if (!AddTransitionResourceBarrier(m_CopyCommandSet, info->Original, D3D12_RESOURCE_STATE_COPY_SOURCE))
						return false;

					if (Type == GPUBuffer::Types::ElementArray)
					{
						if (!CHECK_CALL(DirectX12Wrapper::AddCopyResourceCommand(m_CopyCommandSet.List, info->Original->Resource, info->Buffer.Resource)))
							return false;
					}
					else
					{
						if (!CHECK_CALL(DirectX12Wrapper::AddCopyTextureToBufferCommand(m_CopyCommandSet.List, info->Original->Resource, info->Buffer.Resource)))
							return false;
					}

					if (!ExecuteCommands(m_CopyCommandSet))
						return false;

					return CHECK_CALL(DirectX12Wrapper::MapResource(info->Buffer.Resource, Buffer));
				}

				bool DirectX12Device::UnlockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type)
				{
					if (Handle == 0)
						return false;

					BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);

					return CHECK_CALL(DirectX12Wrapper::UnmapResource(info->Buffer.Resource));
				}

				bool DirectX12Device::CreateShader(const Shaders* Shaders, Shader::Handle& Handle, cstr* ErrorMessage)
				{
					static Shader::Handle lastHandle = 0;

					D3D12_SHADER_BYTECODE vertByteCode = {};
					if (!CHECK_CALL(DirectX12Wrapper::CompileShader(Shaders->VertexShader, "vs_5_0", &vertByteCode, ErrorMessage)))
						return false;

					D3D12_SHADER_BYTECODE fragByteCode = {};
					if (!CHECK_CALL(DirectX12Wrapper::CompileShader(Shaders->FragmentShader, "ps_5_0", &fragByteCode, ErrorMessage)))
						return false;

					++lastHandle;

					m_Shaders[lastHandle] = { vertByteCode, fragByteCode };

					Handle = lastHandle;

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
					return true;
				}

				bool DirectX12Device::GetShaderConstantHandle(Shader::Handle Handle, const String& Name, Shader::ConstantHandle& ConstantHandle)
				{
					return true;
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

					ID3D12Resource* resource = nullptr;
					if (!CHECK_CALL(DirectX12Wrapper::CreateTexture(m_Device, m_ResourceHeap, GetTextureType(Info->Type), Info->Dimension.X, Info->Dimension.Y, GetTextureFormat(Info->Format), D3D12_RESOURCE_FLAG_NONE, state, &resource)))
						return false;

					ResourceInfo* info = RenderingAllocators::RenderingSystemAllocator_Allocate<ResourceInfo>();

					info->Resource = resource;
					info->PrevState = state;

					Handle = (Texture::Handle)info;

					return true;
				}

				bool DirectX12Device::DestroyTexture(Texture::Handle Handle)
				{
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
					if (Info->Textures.GetSize() == 0)
						return false;

					static RenderTarget::Handle lastHandle = 0;

					Handle = ++lastHandle;

					m_RenderTargets[Handle] = {};

					auto& texturesList = m_RenderTargets[Handle];

					uint8 index = 0;

#define CREATE_VIEW(ResourceType, DescriptorHeapType, DescriptorSize, IsColor, CurrnetState) \
					{ \
						RenderTargetHandles::ViewList viewList; \
						index = 0; \
						for each (const auto & textureInfo in Info->Textures) \
						{ \
							if (!RenderTarget::IsColorPoint(textureInfo.Point) == IsColor) \
								continue; \
							ID3D12Resource* resource = nullptr; \
							if (!CHECK_CALL(DirectX12Wrapper::CreateTexture(m_Device, m_RenderTargetHeap, GetTextureType(Texture::Types::TwoD), textureInfo.Dimension.X, textureInfo.Dimension.Y, GetTextureFormat(textureInfo.Format), ResourceType, CurrnetState, &resource))) \
								return false; \
							ViewInfo view = {}; \
							view.Point = textureInfo.Point; \
							view.Resource = resource; \
							view.PrevState = CurrnetState; \
							viewList.Add(view); \
							Textures.Add((Texture::Handle)resource); \
							++index; \
						} \
						if (index != 0) \
						{ \
							ID3D12DescriptorHeap* descriptorHeap = nullptr; \
							if (!CHECK_CALL(DirectX12Wrapper::CreateDescriptorHeap(m_Device, DescriptorHeapType, index, &descriptorHeap))) \
								return false; \
							index = 0; \
							for (auto& view : viewList) \
							{ \
								view.DescriptorHeap = descriptorHeap; \
								view.Index = index; \
								if (IsColor) \
								{ \
									if (!CHECK_CALL(DirectX12Wrapper::CreateRenderTargetView(m_Device, view.Resource, descriptorHeap, index, DescriptorSize))) \
										return false; \
								} \
								else \
									if (!CHECK_CALL(DirectX12Wrapper::CreateDepthStencilView(m_Device, view.Resource, descriptorHeap, index, DescriptorSize))) \
										return false; \
								++index; \
							} \
							texturesList.Views.AddRange(viewList); \
						} \
					}

					CREATE_VIEW(D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_RenderTargetViewDescriptorSize, true, D3D12_RESOURCE_STATE_COMMON);
					CREATE_VIEW(D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, m_DepthStencilViewDescriptorSize, false, D3D12_RESOURCE_STATE_COMMON);

#undef CREATE_VIEW

					return true;
				}

				bool DirectX12Device::DestroyRenderTarget(RenderTarget::Handle Handle)
				{
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

					if (!m_RenderTargets.Contains(Handle))
						return false;

					m_CurrentRenderTarget = &m_RenderTargets[Handle];

					m_CurrentViewCount = m_CurrentRenderTarget->Views.GetSize();
					for (uint8 i = 0; i < m_CurrentViewCount; ++i)
						m_CurrentViews[i] = &m_CurrentRenderTarget->Views[i];

					//m_CommandList->OMSetRenderTargets()

					return true;
				}

				bool DirectX12Device::CreateMesh(const SubMeshInfo* Info, GPUBuffer::Usages Usage, SubMesh::Handle& Handle)
				{
					return true;
				}

				bool DirectX12Device::GetMeshVertexBuffer(SubMesh::Handle Handle, GPUBuffer::Handle& BufferHandle)
				{
					return true;
				}

				bool DirectX12Device::GetMeshElementBuffer(SubMesh::Handle Handle, GPUBuffer::Handle& BufferHandle)
				{
					return true;
				}

				bool DirectX12Device::DestroyMesh(SubMesh::Handle Handle)
				{
					return true;
				}

				bool DirectX12Device::BindMesh(SubMesh::Handle Handle)
				{
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

							if (!CHECK_CALL(DirectX12Wrapper::AddClearRenderTargetCommand(m_RenderCommandSet.List, view->DescriptorHeap, view->Index, m_RenderTargetViewDescriptorSize, &color.X)))
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

						if (!CHECK_CALL(DirectX12Wrapper::AddClearDepthStencilCommand(m_RenderCommandSet.List, view->DescriptorHeap, view->Index, m_DepthStencilViewDescriptorSize, flags, 1, 1)))
							return false;
					}

					return true;
				}

				bool DirectX12Device::DrawIndexed(SubMesh::PolygonTypes PolygonType, uint32 IndexCount)
				{
					return true;
				}

				bool DirectX12Device::DrawArray(SubMesh::PolygonTypes PolygonType, uint32 VertexCount)
				{
					return true;
				}

				bool DirectX12Device::Execute(void)
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
			}
		}
	}
}