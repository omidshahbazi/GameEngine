// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\DeviceInterface.h>
#include <RenderSystem\IPipeline.h>
#include <RenderSystem\Sprite.h>
#include <RenderSystem\Material.h>
#include <RenderSystem\Program.h>
#include <RenderSystem\Mesh.h>
#include <RenderSystem\RenderContext.h>
#include <RenderSystem\RenderTarget.h>
#include <RenderSystem\RenderTargetPool.h>
#include <RenderSystem\CommandBuffer.h>
#include <RenderSystem\CommandBufferFence.h>
#include <RenderSystem\Private\ThreadedDevice.h>
#include <RenderSystem\Private\FrameDataChain.h>
#include <RenderSystem\ProgramConstantSupplier.h>
#include <RenderSystem\Private\BuiltiInProgramConstants.h>
#include <RenderSystem\Private\InternalRenderTargets.h>
#include <RenderSystem\Private\CommandBufferHelper.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <ProgramCompiler\ProgramToAPICompiler.h>
#include <ProgramCompiler\Compiler.h>
#include <DynamicModuleSystem\ModuleManager.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace RenderCommon::Private;
	using namespace ProgramCompiler;
	using namespace Debugging;
	using namespace DynamicModuleSystem;

	namespace RenderSystem
	{
		using namespace Private;

#define CHECK_CALL_STRONG(PromiseExpr) \
		auto promise = PromiseExpr; \
		CoreDebugAssert(Categories::RenderSystem, promise.Wait(), #PromiseExpr);

#define CHECK_CALL_WEAK(PromiseExpr) \
		auto promise = PromiseExpr; \
		if (!promise.Wait()) \
			CoreDebugLogError(Categories::RenderSystem, #PromiseExpr); \

		DeviceInterface::DeviceInterface(DeviceTypes DeviceType) :
			m_Initialized(false),
			m_DeviceType(DeviceType),
			m_Device(nullptr),
			m_ThreadedDevice(nullptr),
			m_FrameDataChain(nullptr),
			m_DefaultTexture(nullptr),
			m_LastContext(nullptr),
			m_Pipeline(nullptr)
		{
			ProgramToAPICompiler::Create(RenderSystemAllocators::RenderSystemAllocator);
			ProgramConstantSupplier::Create(RenderSystemAllocators::RenderSystemAllocator);
			BuiltiInProgramConstants::Create(RenderSystemAllocators::RenderSystemAllocator);
			RenderTargetPool::Create(RenderSystemAllocators::RenderSystemAllocator);
			InternalRenderTargets::Create(RenderSystemAllocators::RenderSystemAllocator);
		}

		DeviceInterface::~DeviceInterface(void)
		{
			if (m_Pipeline != nullptr)
				m_Pipeline->Uninitialize();

			DestroyTexture(m_DefaultTexture);

			RenderSystemAllocators::RenderSystemAllocator_Deallocate(m_FrameDataChain);

			RenderSystemAllocators::RenderSystemAllocator_Deallocate(m_ThreadedDevice);

			InternalRenderTargets::Destroy();
			RenderTargetPool::Destroy();
			BuiltiInProgramConstants::Destroy();
			ProgramConstantSupplier::Destroy();
			ProgramToAPICompiler::Destroy();

			ModuleManager::GetInstance()->Unload(m_Device).Wait();
		}

		void DeviceInterface::Initialize(void)
		{
			PlatformWindow::SetDPIAwareness(PlatformWindow::DPIAwareness::PerMonitorAware2);

			CoreDebugAssert(Categories::RenderSystem, !m_Initialized, "DeviceInterface already initialized");

			String moduleName = "";
			switch (m_DeviceType)
			{
			case DeviceTypes::OpenGL:
				moduleName = "OpenGLRenderDevice";
				break;

			case DeviceTypes::DirectX12:
				moduleName = "DirectX12RenderDevice";
				break;

			case DeviceTypes::Vulkan:
				moduleName = "VulkanRenderDevice";
				break;

			default:
				CoreDebugAssert(Categories::ProgramCompiler, false, "Device type is not supported");
			}

			m_Device = ModuleManager::GetInstance()->Load<IDevice>(moduleName);
			CoreDebugAssert(Categories::ProgramCompiler, m_Device != nullptr, "Couldn't load %S module", moduleName);

			m_ThreadedDevice = RenderSystemAllocators::RenderSystemAllocator_Allocate<ThreadedDevice>();
			Construct(m_ThreadedDevice, m_Device, m_DeviceType);

			m_FrameDataChain = RenderSystemAllocators::ContainersAllocator_Allocate<FrameDataChain>();
			Construct(m_FrameDataChain, m_ThreadedDevice);

			CHECK_CALL_STRONG(m_ThreadedDevice->Initialize());

			TextureInfo info;
			info.Dimension = Vector2I::One;
			info.Format = Formats::R8;
			info.Type = TextureTypes::TwoD;
			m_DefaultTexture = CreateTexture(&info);

			{
				auto debugCallback = [&](int32 ID, IDevice::DebugSources Source, cstr Message, IDevice::DebugTypes Type, IDevice::DebugSeverities Severity)
				{
					StringStream stream;
					stream << "In [";

					switch (Source)
					{
					case IDevice::DebugSources::API: stream << "API"; break;
					case IDevice::DebugSources::WindowSystem: stream << "WindowSystem"; break;
					case IDevice::DebugSources::ProgramCompiler: stream << "ProgramCompiler"; break;
					case IDevice::DebugSources::ThirdParty: stream << "ThirdParty"; break;
					case IDevice::DebugSources::Application: stream << "Application"; break;
					case IDevice::DebugSources::Other: stream << "Other"; break;
					}

					stream << "] a [";

					switch (Severity)
					{
					case IDevice::DebugSeverities::Notification: stream << "Notification"; break;
					case IDevice::DebugSeverities::Low: stream << "Low"; break;
					case IDevice::DebugSeverities::Medium: stream << "Medium"; break;
					case IDevice::DebugSeverities::High: stream << "High"; break;
					}

					stream << "] {" << Message << "} happend\0";

					switch (Severity)
					{
					case IDevice::DebugSeverities::Notification: CoreDebugLogInfo(Categories::RenderSystem, stream.GetBuffer()); break;
					case IDevice::DebugSeverities::Low: CoreDebugLogWarning(Categories::RenderSystem, stream.GetBuffer()); break;
					case IDevice::DebugSeverities::Medium: CoreDebugLogError(Categories::RenderSystem, stream.GetBuffer()); break;
					case IDevice::DebugSeverities::High: CoreDebugLogError(Categories::RenderSystem, stream.GetBuffer()); break;
					}

					printf(stream.GetBuffer());
					printf("\n");
				};

				CHECK_CALL_STRONG(m_ThreadedDevice->SetDebugCallback(debugCallback));
			}

			BuiltiInProgramConstants::GetInstance()->Initialize(this);
			RenderTargetPool::GetInstance()->Initialize(this);

			m_Initialized = true;
		}

		cstr DeviceInterface::GetVersion(void)
		{
			CHECK_CALL_STRONG(m_ThreadedDevice->GetVersion());

			return promise.GetValue();
		}

		cstr DeviceInterface::GetVendorName(void)
		{
			CHECK_CALL_STRONG(m_ThreadedDevice->GetVendorName());

			return promise.GetValue();
		}

		cstr DeviceInterface::GetRendererName(void)
		{
			CHECK_CALL_STRONG(m_ThreadedDevice->GetRendererName());

			return promise.GetValue();
		}

		cstr DeviceInterface::GetShadingLanguageVersion(void)
		{
			CHECK_CALL_STRONG(m_ThreadedDevice->GetShadingLanguageVersion());

			return promise.GetValue();
		}

		RenderContext* DeviceInterface::CreateContext(Window* Window)
		{
			ResourceHandle handle;
			CHECK_CALL_STRONG(m_ThreadedDevice->CreateContext(Window->GetHandle(), handle));

			RenderContext* context = RenderSystemAllocators::RenderSystemAllocator_Allocate<RenderContext>();
			ConstructMacro(RenderContext, context, handle, Window);

			return context;
		}

		void DeviceInterface::DestroyContext(RenderContext* Context)
		{
			DestroyContextInternal(Context);
		}

		Texture* DeviceInterface::CreateTexture(const TextureInfo* Info)
		{
			ResourceHandle handle;
			CHECK_CALL_WEAK(m_ThreadedDevice->CreateTexture(Info, handle));
			if (!promise.GetValue())
				return nullptr;

			Texture* texture = RenderSystemAllocators::ResourceAllocator_Allocate<Texture>();
			ConstructMacro(Texture, texture, m_ThreadedDevice, handle, Info->Type, Info->Format, Info->Dimension);

			return texture;
		}

		Sprite* DeviceInterface::CreateSprite(const TextureInfo* Info)
		{
			ResourceHandle handle;
			CHECK_CALL_WEAK(m_ThreadedDevice->CreateTexture(Info, handle));
			if (!promise.GetValue())
				return nullptr;

			Sprite* sprite = RenderSystemAllocators::ResourceAllocator_Allocate<Sprite>();
			ConstructMacro(Sprite, sprite, m_ThreadedDevice, handle, Info->Format, Info->Dimension, Info->Borders);

			return sprite;
		}

		void DeviceInterface::DestroyTexture(Texture* Texture)
		{
			CHECK_CALL_WEAK(m_ThreadedDevice->DestroyTexture(Texture->GetHandle()));

			RenderSystemAllocators::ResourceAllocator_Deallocate(Texture);
		}

		RenderTarget* DeviceInterface::CreateRenderTarget(const RenderTargetInfo* Info)
		{
			ResourceHandle handle;
			IDevice::TextureList texturesHandle;
			CHECK_CALL_WEAK(m_ThreadedDevice->CreateRenderTarget(Info, handle, texturesHandle));
			if (!promise.GetValue())
				return nullptr;

			RenderTarget::TexturesList textureList;

			for (uint8 i = 0; i < Info->Textures.GetSize(); ++i)
			{
				const auto& info = Info->Textures[i];

				Texture* tex = RenderSystemAllocators::ResourceAllocator_Allocate<Texture>();
				ConstructMacro(Texture, tex, m_ThreadedDevice, texturesHandle[i], TextureTypes::TwoD, info.Format, info.Dimension);

				tex->GenerateMipMaps();

				textureList.Add(tex);
			}

			RenderTarget* texture = RenderSystemAllocators::ResourceAllocator_Allocate<RenderTarget>();
			ConstructMacro(RenderTarget, texture, m_ThreadedDevice, handle, textureList);

			return texture;
		}

		void DeviceInterface::DestroyRenderTarget(RenderTarget* RenderTarget)
		{
			CHECK_CALL_WEAK(m_ThreadedDevice->DestroyRenderTarget(RenderTarget->GetHandle()));

			auto textures = RenderTarget->GetTextures();
			for (auto texture : textures)
				RenderSystemAllocators::ResourceAllocator_Deallocate(texture);

			RenderSystemAllocators::ResourceAllocator_Deallocate(RenderTarget);
		}

		bool DeviceInterface::CompileProgram(const ProgramInfo* Info, CompiledProgramInfo* CompiledInfo)
		{
			if (Compiler::Compile(*Info, &m_DeviceType, 1, CompiledInfo))
				return true;

			CoreDebugLogError(Categories::ProgramCompiler, ("Compiling a program has failed in DeviceInterface {" + CompiledInfo->ErrorMessage + "}").GetValue());

			return false;
		}

		Program* DeviceInterface::CreateProgram(const ProgramInfo* Info)
		{
			static byte compiledVeretexShader[DEFAULT_COMPILED_SHADER_BUFFER_SIZE];
			static byte compiledTessellationShaderShader[DEFAULT_COMPILED_SHADER_BUFFER_SIZE];
			static byte compiledGeometryShader[DEFAULT_COMPILED_SHADER_BUFFER_SIZE];
			static byte compiledFragmentShader[DEFAULT_COMPILED_SHADER_BUFFER_SIZE];
			static byte compiledComputeShader[DEFAULT_COMPILED_SHADER_BUFFER_SIZE];

			CompiledProgramInfo compiledInfo = {};
			compiledInfo.VertexShader.Buffer = compiledVeretexShader;
			compiledInfo.VertexShader.Size = DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE;
			compiledInfo.TessellationShader.Buffer = compiledTessellationShaderShader;
			compiledInfo.TessellationShader.Size = DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE;
			compiledInfo.GeometryShader.Buffer = compiledGeometryShader;
			compiledInfo.GeometryShader.Size = DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE;
			compiledInfo.FragmentShader.Buffer = compiledFragmentShader;
			compiledInfo.FragmentShader.Size = DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE;
			compiledInfo.ComputeShader.Buffer = compiledComputeShader;
			compiledInfo.ComputeShader.Size = DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE;

			if (!CompileProgram(Info, &compiledInfo))
				return nullptr;

			return CreateProgram(&compiledInfo);
		}

		Program* DeviceInterface::CreateProgram(const CompiledProgramInfo* Info)
		{
			IDevice::CompiledShaders compiledShaders = {};
			compiledShaders.VertexShader.Buffer = Info->VertexShader.Buffer;
			compiledShaders.VertexShader.Size = Info->VertexShader.Size;
			compiledShaders.TessellationShader.Buffer = Info->TessellationShader.Buffer;
			compiledShaders.TessellationShader.Size = Info->TessellationShader.Size;
			compiledShaders.GeometryShader.Buffer = Info->GeometryShader.Buffer;
			compiledShaders.GeometryShader.Size = Info->GeometryShader.Size;
			compiledShaders.FragmentShader.Buffer = Info->FragmentShader.Buffer;
			compiledShaders.FragmentShader.Size = Info->FragmentShader.Size;
			compiledShaders.ComputeShader.Buffer = Info->ComputeShader.Buffer;
			compiledShaders.ComputeShader.Size = Info->ComputeShader.Size;

			ResourceHandle handle = 0;
			cstr message = nullptr;
			if (!m_ThreadedDevice->CreateProgram(&compiledShaders, handle, &message).Wait())
			{
				if (message != nullptr)
					CoreDebugLogError(Categories::RenderSystem, message);

				return nullptr;
			}

			Program* program = RenderSystemAllocators::ResourceAllocator_Allocate<Program>();
			ConstructMacro(Program, program, m_ThreadedDevice, handle, Info->MetaInfo);

			return program;
		}

		void DeviceInterface::DestroyProgram(Program* Program)
		{
			CHECK_CALL_WEAK(m_ThreadedDevice->DestroyProgram(Program->GetHandle()));

			RenderSystemAllocators::ResourceAllocator_Deallocate(Program);
		}

		Mesh* DeviceInterface::CreateMesh(const MeshInfo* Info)
		{
			SubMesh* subMeshes = RenderSystemAllocators::ResourceAllocator_AllocateArray<SubMesh>(Info->SubMeshes.GetSize());
			uint16 subMeshIndex = 0;

			for (uint16 i = 0; i < Info->SubMeshes.GetSize(); ++i)
			{
				ResourceHandle handle;

				auto& subMeshInfo = Info->SubMeshes[i];

				if (subMeshInfo->Vertices.GetSize() == 0)
					continue;

				CHECK_CALL_WEAK(m_ThreadedDevice->CreateMesh(subMeshInfo, handle));
				if (!promise.GetValue())
					return nullptr;

				ConstructMacro(SubMesh, &subMeshes[subMeshIndex++], m_ThreadedDevice, handle, subMeshInfo->Vertices.GetSize(), subMeshInfo->Indices.GetSize(), subMeshInfo->Type, subMeshInfo->Layout);
			}

			Mesh* mesh = RenderSystemAllocators::ResourceAllocator_Allocate<Mesh>();
			Construct(mesh, subMeshes, subMeshIndex);
			return mesh;
		}

		void DeviceInterface::DestroyMesh(Mesh* Mesh)
		{
			for (uint16 i = 0; i < Mesh->GetSubMeshCount(); ++i)
			{
				CHECK_CALL_WEAK(m_ThreadedDevice->DestroyMesh(Mesh->GetSubMeshes()[i].GetHandle()));
			}

			RenderSystemAllocators::ResourceAllocator_Deallocate(Mesh->GetSubMeshes());

			RenderSystemAllocators::ResourceAllocator_Deallocate(Mesh);
		}

		void DeviceInterface::SubmitCommandBuffer(const CommandBuffer* Buffer)
		{
			ICommandBuffer* commandBuffer = nullptr;
			CHECK_CALL_WEAK(m_ThreadedDevice->CreateCommandBuffer(commandBuffer));
			auto buffers = m_FrameDataChain->GetFrontConstantBuffers();

			if (!ConstCast(CommandBuffer*, Buffer)->PrepareNativeBuffers(commandBuffer, buffers, m_DefaultTexture, m_LastContext))
				return;

			{
				//UNDONE:RENDERING -> ConstantBufferSyncing
				//This would execute all buffers per cmd, not the buffers which are in use in the current cmd
				CHECK_CALL_WEAK(m_ThreadedDevice->SyncConstantBuffers(buffers));
			}

			CommandBufferHelper::SubmitAndDestroy(m_ThreadedDevice, commandBuffer);
		}

		CommandBufferFence* DeviceInterface::CreateFence(void)
		{
			IFence* nativeFence = nullptr;
			CHECK_CALL_WEAK(m_ThreadedDevice->CreateFence(nativeFence));

			CommandBufferFence* fence = RenderSystemAllocators::ResourceAllocator_Allocate<CommandBufferFence>();
			ConstructMacro(CommandBufferFence, fence, nativeFence);

			return fence;
		}

		void DeviceInterface::DestroyFence(CommandBufferFence* Fence)
		{
			IFence* nativeFence = Fence->GetFence();
			CHECK_CALL_WEAK(m_ThreadedDevice->DestroyFences(&nativeFence, 1));

			RenderSystemAllocators::ResourceAllocator_Deallocate(Fence);
		}

		void DeviceInterface::BeginFrame(const RenderContext* Context)
		{
			CoreDebugAssert(Categories::RenderSystem, Context != nullptr, "Context cannot be null");

			m_LastContext = Context;

			Window* window = Context->GetWindow();

			m_ThreadedDevice->SetContext(Context->GetHandle());

			OnContextChangedEvent(m_LastContext);
			OnContextResizedEvent(window->GetClientSize());

			if (m_Pipeline != nullptr)
				m_Pipeline->BeginRender();
		}

		void DeviceInterface::EndFrame(void)
		{
			if (m_Pipeline != nullptr)
				m_Pipeline->EndRender();

			m_FrameDataChain->Swap();

			m_ThreadedDevice->SwapBuffers();

			m_ThreadedDevice->SetContext(0);

			m_LastContext = nullptr;
		}

		void DeviceInterface::SetPipeline(IPipeline* Pipeline)
		{
			if (m_Pipeline != nullptr)
				m_Pipeline->Uninitialize();

			m_Pipeline = Pipeline;

			if (m_Pipeline != nullptr)
				m_Pipeline->Initialize(this);
		}

		void DeviceInterface::DestroyContextInternal(RenderContext* Context)
		{
			CHECK_CALL_STRONG(m_ThreadedDevice->DestroyContext(Context->GetHandle()));

			RenderSystemAllocators::RenderSystemAllocator_Deallocate(Context);
		}
	}
}