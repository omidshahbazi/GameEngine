// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\DeviceInterface.h>
#include <RenderSystem\IPipeline.h>
#include <RenderSystem\Sprite.h>
#include <RenderSystem\Program.h>
#include <RenderSystem\Mesh.h>
#include <RenderSystem\RenderContext.h>
#include <RenderSystem\RenderTarget.h>
#include <RenderSystem\Private\ThreadedDevice.h>
#include <RenderSystem\Private\Commands\CommandsHolder.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <RenderSystem\Private\Commands\ClearCommand.h>
#include <RenderSystem\Private\Commands\DrawCommand.h>
#include <RenderSystem\Private\Commands\SwitchRenderTargetCommand.h>
#include <RenderSystem\Private\Commands\SetViewportCommand.h>
#include <RenderSystem\Private\Commands\BeginEventCommand.h>
#include <RenderSystem\Private\Commands\EndEventCommand.h>
#include <RenderSystem\Private\Commands\SetMarkerCommand.h>
#include <RenderSystem\ProgramConstantSupplier.h>
#include <RenderSystem\Private\BuiltiInProgramConstants.h>
#include <RenderSystem\Material.h>
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
		using namespace Private::Commands;

#define CHECK_CALL_STRONG(PromiseExpr) \
		auto promise = PromiseExpr; \
		CoreDebugAssert(Categories::RenderSystem, promise.Wait(), #PromiseExpr);

#define CHECK_CALL_WEAK(PromiseExpr) \
		auto promise = PromiseExpr; \
		if (!promise.Wait()) \
			CoreDebugLogError(Categories::RenderSystem, #PromiseExpr); \

		template<typename BaseType>
		BaseType* AllocateCommand(CommandsHolder* Holder, RenderQueues Queue)
		{
			return ReinterpretCast(BaseType*, AllocateMemory(&Holder->GetFrontAllocators()[(int8)Queue], sizeof(BaseType)));
		}

		DeviceInterface::DeviceInterface(DeviceTypes DeviceType) :
			m_Initialized(false),
			m_DeviceType(DeviceType),
			m_Device(nullptr),
			m_Pipeline(nullptr),
			m_ThreadedDevice(nullptr),
			m_CommandsHolder(nullptr),
			m_CurentContext(nullptr)
		{
			ProgramToAPICompiler::Create(RenderSystemAllocators::RenderSystemAllocator);
			ProgramConstantSupplier::Create(RenderSystemAllocators::RenderSystemAllocator);
			BuiltiInProgramConstants::Create(RenderSystemAllocators::RenderSystemAllocator);
		}

		DeviceInterface::~DeviceInterface(void)
		{
			if (m_Pipeline != nullptr)
				m_Pipeline->Uninitialize();

			RenderSystemAllocators::RenderSystemAllocator_Deallocate(m_ThreadedDevice);

			ProgramConstantSupplier::Destroy();
			BuiltiInProgramConstants::Destroy();
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

			m_CommandsHolder = m_ThreadedDevice->GetCommandHolder();

			CHECK_CALL_STRONG(m_ThreadedDevice->Initialize());

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

					stream << "] {" << Message << "} happend";

					switch (Severity)
					{
					case IDevice::DebugSeverities::Notification: CoreDebugLogError(Categories::RenderSystem, stream.GetBuffer()); break;
					case IDevice::DebugSeverities::Low: CoreDebugLogError(Categories::RenderSystem, stream.GetBuffer()); break;
					case IDevice::DebugSeverities::Medium: CoreDebugLogWarning(Categories::RenderSystem, stream.GetBuffer()); break;
					case IDevice::DebugSeverities::High: CoreDebugLogInfo(Categories::RenderSystem, stream.GetBuffer()); break;
					}
				};

				CHECK_CALL_STRONG(m_ThreadedDevice->SetDebugCallback(debugCallback));
			}

			BuiltiInProgramConstants::GetInstance()->Initialize(this);

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

		void DeviceInterface::SetContext(RenderContext* Context)
		{
			if (m_CurentContext == Context)
				return;

			if (m_CurentContext != nullptr)
				m_CurentContext->GetWindow()->OnSizeChangedEvent -= EventListener_OnWindowSizeChanged;

			Window* window = nullptr;
			if (Context != nullptr)
				window = Context->GetWindow();

			m_ThreadedDevice->SetContext(Context == nullptr ? 0 : Context->GetHandle());

			m_CurentContext = Context;

			if (window != nullptr)
			{
				window->OnSizeChangedEvent += EventListener_OnWindowSizeChanged;

				m_ThreadedDevice->SetContextSize(window->GetClientSize());
			}

			OnContextChangedEvent(m_CurentContext);
		}

		RenderContext* DeviceInterface::GetContext(void)
		{
			return m_CurentContext;
		}

		Texture* DeviceInterface::CreateTexture(const TextureInfo* Info)
		{
			ResourceHandle handle;
			CHECK_CALL_WEAK(m_ThreadedDevice->CreateTexture(Info, handle));
			if (!promise.GetValue())
				return nullptr;

			Texture* texture = RenderSystemAllocators::ResourceAllocator_Allocate<Texture>();
			ConstructMacro(Texture, texture, m_ThreadedDevice, handle, Info->Type, Info->Format, Info->Dimension);

			if (Info->Data != nullptr)
				texture->GenerateMipMaps();

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

			if (Info->Data != nullptr)
				sprite->GenerateMipMaps();

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

		CommandBuffer* DeviceInterface::CreateCommandBuffer(void)
		{
			return RenderSystemAllocators::ResourceAllocator_Allocate<CommandBuffer>();
		}

		void DeviceInterface::DestroyCommandBuffer(CommandBuffer* Buffer)
		{
		}

		void DeviceInterface::SubmitCommandBuffer(CommandBuffer* Buffer)
		{
		}

		void DeviceInterface::DrawMesh(Mesh* Mesh, const Matrix4F& Transform, Program* Program, RenderQueues Queue)
		{
			if (Mesh == nullptr)
				return;

			static Matrix4F id;
			id = Matrix4F::Identity;

			DrawMesh(Mesh, id, id, id, Transform, Program, Queue);
		}

		void DeviceInterface::DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, Program* Program, RenderQueues Queue)
		{
			if (Mesh == nullptr)
				return;

			Matrix4F mvp = Projection;
			mvp *= View;
			mvp *= Model;

			DrawMesh(Mesh, Model, View, Projection, mvp, Program, Queue);
		}

		void DeviceInterface::DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, Program* Program, RenderQueues Queue)
		{
			if (Mesh == nullptr)
				return;

			DrawCommand* cmd = AllocateCommand<DrawCommand>(m_CommandsHolder, Queue);
			Construct(cmd, &m_CommandsHolder->GetFrontAllocators()[(uint32)Queue], m_CommandsHolder->GetFrontIntermediateConstantBuffers(), Mesh, Model, View, Projection, MVP, Program, Program);
			AddCommandToQueue(Queue, cmd);
		}

		void DeviceInterface::DrawMesh(Mesh* Mesh, const Matrix4F& Transform, Material* Material)
		{
			if (Mesh == nullptr)
				return;

			if (Material == nullptr)
				return;

			static Matrix4F id = Matrix4F::Identity;

			DrawMesh(Mesh, id, id, id, Transform, Material);
		}

		void DeviceInterface::DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Material* Material)
		{
			if (Mesh == nullptr)
				return;

			if (Material == nullptr)
				return;

			Matrix4F mvp = Projection;
			mvp *= View;
			mvp *= Model;

			DrawMesh(Mesh, Model, View, Projection, mvp, Material);
		}

		void DeviceInterface::DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, const Material* Material)
		{
			if (Mesh == nullptr)
				return;

			if (Material == nullptr)
				return;

			for (auto& pass : Material->GetPasses())
			{
				Pass* passPtr = ConstCast(Pass*, &pass);

				if (passPtr->GetProgram() == nullptr)
					continue;

				auto queue = pass.GetQueue();

				DrawCommandEx* cmd = AllocateCommand<DrawCommandEx>(m_CommandsHolder, queue);
				Construct(cmd, &m_CommandsHolder->GetFrontAllocators()[(uint32)pass.GetQueue()], m_CommandsHolder->GetFrontIntermediateConstantBuffers(), Mesh, Model, View, Projection, MVP, passPtr->GetProgram()->GetPointer(), passPtr, passPtr->GetRenderState());
				AddCommandToQueue(queue, cmd);
			}
		}

		void DeviceInterface::BeginRender(void)
		{
			if (m_Pipeline != nullptr)
				m_Pipeline->BeginRender();
		}

		void DeviceInterface::EndRender(void)
		{
			m_CommandsHolder->Swap();

			if (m_Pipeline != nullptr)
				m_Pipeline->EndRender();
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
			if (m_CurentContext == Context && m_CurentContext != nullptr)
				m_CurentContext->GetWindow()->OnSizeChangedEvent -= EventListener_OnWindowSizeChanged;

			CHECK_CALL_STRONG(m_ThreadedDevice->DestroyContext(Context->GetHandle()));

			RenderSystemAllocators::RenderSystemAllocator_Deallocate(Context);
		}

		void DeviceInterface::AddCommandToQueue(RenderQueues Queue, CommandBase* Command)
		{
			CommandsHolder::CommandList* frontCommands = m_CommandsHolder->GetFrontCommandQueue();

			(frontCommands[(int8)Queue]).Add(Command);
		}

		void DeviceInterface::OnWindowSizeChanged(Window* Window)
		{
			CHECK_CALL_STRONG(m_ThreadedDevice->SetContextSize(Window->GetClientSize()));

			OnContextResizedEvent(m_CurentContext);
		}
	}
}