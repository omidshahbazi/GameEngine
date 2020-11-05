// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\DirectX12\DirectX12Device.h>
#include <Rendering\Private\DirectX12\DirectX12Wrapper.h>
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
#define CHECK_CALL(Expr) (SUCCEEDED(Expr) || !RaiseDebugMessages(m_InfoQueue, this))

				bool RaiseDebugMessages(ID3D12InfoQueue* InfoQueue, DirectX12Device* Device)
				{
					if (InfoQueue == nullptr)
						return true;

					IDevice::DebugFunction procedure = Device->GetDebugCallback();
					if (procedure == nullptr)
						return true;

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

							procedure(ID, source, Message, IDevice::DebugTypes::All, severity);
						});

					return true;
				}

				DirectX12Device::DirectX12Device(void) :
					m_Initialized(false),
					m_Factory(nullptr),
					m_Adapter(nullptr),
					m_Device(nullptr)
				{
				}

				DirectX12Device::~DirectX12Device(void)
				{
				}

				bool DirectX12Device::Initialize(void)
				{
					Assert(m_CurrentContext != nullptr, "Context is null");

#if DEBUG_MODE
					if (!DirectX12Wrapper::EnableDebugLayer())
						return false;

					if (!DirectX12Wrapper::CreateFactory(&m_Factory, true))
						return false;
#else
					if (!DirectX12Wrapper::CreateFactory(&m_Factory, false))
						return false;
#endif

					if (!DirectX12Wrapper::FindBestAdapter(&m_Adapter, m_Factory))
						return false;

					if (!DirectX12Wrapper::CreateDevice(&m_Device, m_Adapter))
						return false;

#if DEBUG_MODE
					if (!DirectX12Wrapper::GetInfoQueue(&m_InfoQueue, m_Device))
						return false;
#endif

					ID3D12CommandQueue* commandQueue = nullptr;
					if (!DirectX12Wrapper::CreateCommandQueue(&commandQueue, m_Device))
						return false;

					IDXGISwapChain4* swapChain;
					if (!DirectX12Wrapper::CreateSwapChain(&swapChain, m_Factory, m_Device, m_CurrentContext->GetWindowHandle()))
						return false;



					m_Initialized = true;

					return true;
				}

				cstr DirectX12Device::GetVersion(void)
				{
					return cstr();
				}

				cstr DirectX12Device::GetVendorName(void)
				{
					return cstr();
				}

				cstr DirectX12Device::GetRendererName(void)
				{
					return cstr();
				}

				cstr DirectX12Device::GetShadingLanguageVersion(void)
				{
					return cstr();
				}

				RenderContext* DirectX12Device::CreateContext(PlatformWindow::WindowHandle Handle)
				{
					return new RenderContext(0);
				}

				bool DirectX12Device::DestroyContext(RenderContext* Context)
				{
					return true;
				}

				bool DirectX12Device::SetContext(RenderContext* Context)
				{
					return true;
				}

				RenderContext* DirectX12Device::GetContext(void)
				{
					return nullptr;
				}

				bool DirectX12Device::SetViewport(const Vector2I& Position, const Vector2I& Size)
				{
					return true;
				}

				bool DirectX12Device::SetClearColor(const ColorUI8& Color)
				{
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
					return true;
				}

				bool DirectX12Device::SetDebugCallback(DebugFunction Callback)
				{
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