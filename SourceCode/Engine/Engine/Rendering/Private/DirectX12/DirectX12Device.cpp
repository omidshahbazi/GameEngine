// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\DirectX12\DirectX12Device.h>
#include <Rendering\Private\DirectX12\DirectX12Wrapper.h>
#include <Rendering\Private\DirectX12\DirectX12RenderContext.h>
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
							Debug::Print(Message);

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
					m_CommandQueue(nullptr),
					m_CommandAllocators(nullptr),
					m_CurrentContext(nullptr)
				{
				}

				DirectX12Device::~DirectX12Device(void)
				{
					//HITODO: release other resources

					RenderingAllocators::ResourceAllocator_Deallocate(m_CommandAllocators);
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

					if (!CHECK_CALL(DirectX12Wrapper::CreateCommandQueue(m_Device, &m_CommandQueue)))
						return false;

					m_CommandAllocators = RenderingAllocators::RenderingSystemAllocator_AllocateArray<ID3D12CommandAllocator*>(BACK_BUFFER_COUNT);

					for (uint8 i = 0; i < BACK_BUFFER_COUNT; ++i)
						if (!CHECK_CALL(DirectX12Wrapper::CreateCommandAllocator(m_Device, D3D12_COMMAND_LIST_TYPE_DIRECT, &m_CommandAllocators[i])))
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

				RenderContext* DirectX12Device::CreateContext(PlatformWindow::WindowHandle Handle)
				{
					if (Handle == 0)
						return false;

					for each (auto & context in m_Contexts)
						if (context->GetWindowHandle() == Handle)
							return false;

					IDXGISwapChain4* swapChain = nullptr;
					if (!CHECK_CALL(DirectX12Wrapper::CreateSwapChain(m_Factory, m_CommandQueue, Handle, BACK_BUFFER_COUNT, &swapChain)))
						return false;

					ID3D12DescriptorHeap* descriptorHeap = nullptr;
					if (!CHECK_CALL(DirectX12Wrapper::CreateDescriptorHeap(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, BACK_BUFFER_COUNT, &descriptorHeap)))
						return false;

					if (!CHECK_CALL(DirectX12Wrapper::UpdateRenderTargetViews(m_Device, swapChain, descriptorHeap, BACK_BUFFER_COUNT)))
						return false;

					uint64 index = swapChain->GetCurrentBackBufferIndex();
					ID3D12GraphicsCommandList* commandList = nullptr;
					if (!CHECK_CALL(DirectX12Wrapper::CreateCommandList(m_CommandAllocators[index], m_Device, D3D12_COMMAND_LIST_TYPE_DIRECT, &commandList)))
						return false;
					ID3D12PipelineState* state;

					//commandList->SetPipelineState()

					DirectX12RenderContext* context = RenderingAllocators::RenderingSystemAllocator_Allocate<DirectX12RenderContext>();
					Construct(context, Handle, swapChain);

					m_Contexts.Add(context);

					return context;
				}

				bool DirectX12Device::DestroyContext(RenderContext* Context)
				{
					if (Context == nullptr)
						return true;

					DirectX12RenderContext* context = ReinterpretCast(DirectX12RenderContext*, Context);

					if (m_CurrentContext == context)
						SetContext(nullptr);

					DirectX12Wrapper::ReleaseResource(context->GetSwapChain());

					RenderingAllocators::RenderingSystemAllocator_Deallocate(context);

					m_Contexts.Remove(context);

					return true;
				}

				bool DirectX12Device::SetContext(RenderContext* Context)
				{
					if (m_CurrentContext == Context)
						return true;

					if (m_CurrentContext != nullptr)
						m_CurrentContext->Deactivate();

					if (Context == nullptr)
					{
						m_CurrentContext = nullptr;
						PlatformWindow::MakeCurrentWGLContext(0, 0);
						return true;
					}

					Assert(IsTypeOf(Context, DirectX12RenderContext), "Invalid context type");

					m_CurrentContext = ReinterpretCast(DirectX12RenderContext*, Context);

					m_CurrentContext->Activate();

					ResetState();

					return true;
				}

				RenderContext* DirectX12Device::GetContext(void)
				{
					return m_CurrentContext;
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
					return true;
				}

				bool DirectX12Device::ReadBufferData(GPUBuffer::Handle Handle, GPUBuffer::Types Type, Texture::Handle TextureHandle, Texture::Types TextureType, uint32 Width, uint32 Height, Texture::Formats TextureFormat)
				{
					return true;
				}

				bool DirectX12Device::LockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Access Access, byte** Buffer)
				{
					return true;
				}

				bool DirectX12Device::UnlockBuffer(GPUBuffer::Types Type)
				{
					return true;
				}

				bool DirectX12Device::CreateShader(const Shaders* Shaders, Shader::Handle& Handle, cstr* ErrorMessage)
				{
					D3D12_SHADER_BYTECODE vertByteCode = {};
					if (!CHECK_CALL(DirectX12Wrapper::CompileShader(Shaders->VertexShader, "vs_5_0", &vertByteCode, ErrorMessage)))
						return false;

					D3D12_SHADER_BYTECODE fragByteCode = {};
					if (!CHECK_CALL(DirectX12Wrapper::CompileShader(Shaders->FragmentShader, "ps_5_0", &fragByteCode, ErrorMessage)))
						return false;


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
					return true;
				}

				bool DirectX12Device::DestroyRenderTarget(RenderTarget::Handle Handle)
				{
					return true;
				}

				bool DirectX12Device::BindRenderTarget(RenderTarget::Handle Handle)
				{
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

				bool DirectX12Device::CreateVertexArray(const MeshBufferInfo& Info, NativeType::Handle& Handle)
				{
					return true;
				}

				bool DirectX12Device::DestroyVertexArray(NativeType::Handle Handle)
				{
					return true;
				}

				bool DirectX12Device::BindMesh(SubMesh::Handle Handle)
				{
					return true;
				}

				bool DirectX12Device::Clear(ClearFlags Flags)
				{
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

				bool DirectX12Device::SwapBuffers(void)
				{
					if (m_CurrentContext == nullptr)
						return false;

					return CHECK_CALL(DirectX12Wrapper::Present(m_CurrentContext->GetSwapChain()));
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
			}
		}
	}
}