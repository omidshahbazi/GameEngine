// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\DeviceInterface.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\Sprite.h>
#include <Rendering\Program.h>
#include <Rendering\Mesh.h>
#include <Rendering\Private\ThreadedDevice.h>
#include <Rendering\Private\CommandsHolder.h>
#include <Rendering\Private\RenderingAllocators.h>
#include <Rendering\Private\OpenGL\OpenGLDevice.h>
#include <Rendering\Private\DirectX12\DirectX12Device.h>
#include <Rendering\Private\Vulkan\VulkanDevice.h>
#include <Rendering\Private\ProgramCompiler\Compiler.h>
#include <Rendering\Private\ProgramCompiler\CompilerHelper.h>
#include <Rendering\Private\ProgramCompiler\GLSLANGCompiler.h>
#include <Rendering\Private\Commands\ClearCommand.h>
#include <Rendering\Private\Commands\DrawCommand.h>
#include <Rendering\Private\Commands\SwitchRenderTargetCommand.h>
#include <Rendering\Private\Commands\SetViewportCommand.h>
#include <Rendering\Private\Commands\BeginEventCommand.h>
#include <Rendering\Private\Commands\EndEventCommand.h>
#include <Rendering\Private\Commands\SetMarkerCommand.h>
#include <Rendering\Private\Pipeline\PipelineManager.h>
#include <Rendering\ProgramConstantSupplier.h>
#include <Rendering\Private\BuiltiInProgramConstants.h>
#include <Rendering\Material.h>

namespace Engine
{
	namespace Rendering
	{
		using namespace Private;
		using namespace Private::OpenGL;
		using namespace Private::DirectX12;
		using namespace Private::Vulkan;
		using namespace Private::ProgramCompiler;
		using namespace Private::Commands;
		using namespace Private::Pipeline;

#define CHECK_CALL_STRONG(PromiseExpr) \
		auto promise = PromiseExpr; \
		CoreDebugAssert(Categories::Rendering, promise.Wait(), #PromiseExpr);

#define CHECK_CALL_WEAK(PromiseExpr) \
		auto promise = PromiseExpr; \
		if (!promise.Wait()) \
			CoreDebugLogError(Categories::Rendering, #PromiseExpr); \

		template<typename BaseType>
		BaseType* AllocateCommand(CommandsHolder* Holder, RenderQueues Queue)
		{
			return ReinterpretCast(BaseType*, AllocateMemory(&Holder->GetFrontAllocators()[(int8)Queue], sizeof(BaseType)));
		}

		DeviceInterface::DeviceInterface(DeviceTypes DeviceType) :
			m_Initialized(false),
			m_DeviceType(DeviceType),
			m_Device(nullptr),
			m_ThreadedDevice(nullptr),
			m_CommandsHolder(nullptr),
			m_CurentContext(nullptr)
		{
			Compiler::Create(RenderingAllocators::RenderingSystemAllocator);
			ProgramConstantSupplier::Create(RenderingAllocators::RenderingSystemAllocator);
			BuiltiInProgramConstants::Create(RenderingAllocators::RenderingSystemAllocator);
		}

		DeviceInterface::~DeviceInterface(void)
		{
			PipelineManager::Destroy();

			RenderingAllocators::RenderingSystemAllocator_Deallocate(m_ThreadedDevice);

			ProgramConstantSupplier::Destroy();
			BuiltiInProgramConstants::Destroy();
			Compiler::Destroy();

			RenderingAllocators::RenderingSystemAllocator_Deallocate(m_Device);

			GLSLANGCompiler::Destroy();
		}

		void DeviceInterface::Initialize(void)
		{
			PlatformWindow::SetDPIAwareness(PlatformWindow::DPIAwareness::PerMonitorAware2);

			CoreDebugAssert(Categories::Rendering, !m_Initialized, "DeviceInterface already initialized");

			switch (m_DeviceType)
			{
			case DeviceTypes::OpenGL:
			{
				m_Device = RenderingAllocators::RenderingSystemAllocator_Allocate<OpenGLDevice>();
				Construct(ReinterpretCast(OpenGLDevice*, m_Device));
			} break;

			case DeviceTypes::DirectX12:
			{
				m_Device = RenderingAllocators::RenderingSystemAllocator_Allocate<DirectX12Device>();
				Construct(ReinterpretCast(DirectX12Device*, m_Device));
			} break;

			case DeviceTypes::Vulkan:
			{
				m_Device = RenderingAllocators::RenderingSystemAllocator_Allocate<VulkanDevice>();
				Construct(ReinterpretCast(VulkanDevice*, m_Device));
			} break;
			}

			CoreDebugAssert(Categories::Rendering, m_Device != nullptr, "m_Device cannot be null");

			m_ThreadedDevice = RenderingAllocators::RenderingSystemAllocator_Allocate<ThreadedDevice>();
			Construct(m_ThreadedDevice, m_Device, m_DeviceType);

			m_CommandsHolder = m_ThreadedDevice->GetCommandHolder();

			GLSLANGCompiler::Create(RenderingAllocators::ProgramCompilerAllocator);

			PipelineManager::Create(RenderingAllocators::RenderingSystemAllocator);

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
					case IDevice::DebugSeverities::Notification: CoreDebugLogError(Categories::Rendering, stream.GetBuffer()); break;
					case IDevice::DebugSeverities::Low: CoreDebugLogError(Categories::Rendering, stream.GetBuffer()); break;
					case IDevice::DebugSeverities::Medium: CoreDebugLogWarning(Categories::Rendering, stream.GetBuffer()); break;
					case IDevice::DebugSeverities::High: CoreDebugLogInfo(Categories::Rendering, stream.GetBuffer()); break;
					}
				};

				CHECK_CALL_STRONG(m_ThreadedDevice->SetDebugCallback(debugCallback));
			}

			BuiltiInProgramConstants::GetInstance()->Initialize(this);
			PipelineManager::GetInstance()->Initialize(this);

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
			RenderContext::Handle handle;
			CHECK_CALL_STRONG(m_ThreadedDevice->CreateContext(Window->GetHandle(), handle));

			RenderContext* context = RenderingAllocators::RenderingSystemAllocator_Allocate<RenderContext>();
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

		void DeviceInterface::SetViewport(const Vector2I& Position, const Vector2I& Size, RenderQueues Queue)
		{
			SetViewportCommand* cmd = AllocateCommand<SetViewportCommand>(m_CommandsHolder, Queue);
			Construct(cmd, Position, Size);
			AddCommandToQueue(Queue, cmd);
		}

		Texture* DeviceInterface::CreateTexture(const TextureInfo* Info)
		{
			Texture::Handle handle;
			CHECK_CALL_WEAK(m_ThreadedDevice->CreateTexture(Info, handle));
			if (!promise.GetValue())
				return nullptr;

			Texture* texture = RenderingAllocators::ResourceAllocator_Allocate<Texture>();
			ConstructMacro(Texture, texture, m_ThreadedDevice, handle, Info->Type, Info->Format, Info->Dimension);

			if (Info->Data != nullptr)
				texture->GenerateMipMaps();

			return texture;
		}

		Sprite* DeviceInterface::CreateSprite(const TextureInfo* Info)
		{
			Sprite::Handle handle;
			CHECK_CALL_WEAK(m_ThreadedDevice->CreateTexture(Info, handle));
			if (!promise.GetValue())
				return nullptr;

			Sprite* sprite = RenderingAllocators::ResourceAllocator_Allocate<Sprite>();
			ConstructMacro(Sprite, sprite, m_ThreadedDevice, handle, Texture::Types::TwoD, Info->Format, Info->Dimension, Info->Borders);

			if (Info->Data != nullptr)
				sprite->GenerateMipMaps();

			return sprite;
		}

		void DeviceInterface::DestroyTexture(Texture* Texture)
		{
			CHECK_CALL_WEAK(m_ThreadedDevice->DestroyTexture(Texture->GetHandle()));

			RenderingAllocators::ResourceAllocator_Deallocate(Texture);
		}

		RenderTarget* DeviceInterface::CreateRenderTarget(const RenderTargetInfo* Info)
		{
			RenderTarget::Handle handle;
			IDevice::TextureList texturesHandle;
			CHECK_CALL_WEAK(m_ThreadedDevice->CreateRenderTarget(Info, handle, texturesHandle));
			if (!promise.GetValue())
				return nullptr;

			RenderTarget::TexturesList textureList;

			for (uint8 i = 0; i < Info->Textures.GetSize(); ++i)
			{
				const auto& info = Info->Textures[i];

				Texture* tex = RenderingAllocators::ResourceAllocator_Allocate<Texture>();
				ConstructMacro(Texture, tex, m_ThreadedDevice, texturesHandle[i], Texture::Types::TwoD, info.Format, info.Dimension);

				tex->GenerateMipMaps();

				textureList.Add(tex);
			}

			RenderTarget* texture = RenderingAllocators::ResourceAllocator_Allocate<RenderTarget>();
			ConstructMacro(RenderTarget, texture, m_ThreadedDevice, handle, textureList);

			return texture;
		}

		void DeviceInterface::DestroyRenderTarget(RenderTarget* RenderTarget)
		{
			CHECK_CALL_WEAK(m_ThreadedDevice->DestroyRenderTarget(RenderTarget->GetHandle()));

			auto textures = RenderTarget->GetTextures();
			for (auto texture : textures)
				RenderingAllocators::ResourceAllocator_Deallocate(texture);

			RenderingAllocators::ResourceAllocator_Deallocate(RenderTarget);
		}

		void DeviceInterface::SetRenderTarget(RenderTarget* RenderTarget, RenderQueues Queue)
		{
			SwitchRenderTargetCommand* cmd = AllocateCommand<SwitchRenderTargetCommand>(m_CommandsHolder, Queue);
			Construct(cmd, RenderTarget);
			AddCommandToQueue(Queue, cmd);
		}

		bool DeviceInterface::CompileProgram(const ProgramInfo* Info, CompiledProgramInfo* CompiledInfo)
		{
			if (CompilerHelper::Compile(*Info, &m_DeviceType, 1, CompiledInfo))
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

			Program::Handle handle = 0;
			cstr message = nullptr;
			if (!m_ThreadedDevice->CreateProgram(&compiledShaders, handle, &message).Wait())
			{
				if (message != nullptr)
					CoreDebugLogError(Categories::Rendering, message);

				return nullptr;
			}

			Program* program = RenderingAllocators::ResourceAllocator_Allocate<Program>();
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

			RenderingAllocators::ResourceAllocator_Deallocate(Program);
		}

		Mesh* DeviceInterface::CreateMesh(const MeshInfo* Info)
		{
			SubMesh* subMeshes = RenderingAllocators::ResourceAllocator_AllocateArray<SubMesh>(Info->SubMeshes.GetSize());
			uint16 subMeshIndex = 0;

			for (uint16 i = 0; i < Info->SubMeshes.GetSize(); ++i)
			{
				SubMesh::Handle handle;

				auto& subMeshInfo = Info->SubMeshes[i];

				if (subMeshInfo->Vertices.GetSize() == 0)
					continue;

				CHECK_CALL_WEAK(m_ThreadedDevice->CreateMesh(subMeshInfo, handle));
				if (!promise.GetValue())
					return nullptr;

				ConstructMacro(SubMesh, &subMeshes[subMeshIndex++], m_ThreadedDevice, handle, subMeshInfo->Vertices.GetSize(), subMeshInfo->Indices.GetSize(), subMeshInfo->Type, subMeshInfo->Layout);
			}

			Mesh* mesh = RenderingAllocators::ResourceAllocator_Allocate<Mesh>();
			Construct(mesh, subMeshes, subMeshIndex);
			return mesh;
		}

		void DeviceInterface::DestroyMesh(Mesh* Mesh)
		{
			for (uint16 i = 0; i < Mesh->GetSubMeshCount(); ++i)
			{
				CHECK_CALL_WEAK(m_ThreadedDevice->DestroyMesh(Mesh->GetSubMeshes()[i].GetHandle()));
			}

			RenderingAllocators::ResourceAllocator_Deallocate(Mesh->GetSubMeshes());

			RenderingAllocators::ResourceAllocator_Deallocate(Mesh);
		}

		void DeviceInterface::Clear(IDevice::ClearFlags Flags, const ColorUI8& Color, RenderQueues Queue)
		{
			ClearCommand* cmd = AllocateCommand<ClearCommand>(m_CommandsHolder, Queue);
			Construct(cmd, Flags, Color);
			AddCommandToQueue(Queue, cmd);
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

				PassDrawCommand* cmd = AllocateCommand<PassDrawCommand>(m_CommandsHolder, queue);
				Construct(cmd, &m_CommandsHolder->GetFrontAllocators()[(uint32)pass.GetQueue()], m_CommandsHolder->GetFrontIntermediateConstantBuffers(), Mesh, Model, View, Projection, MVP, passPtr);
				AddCommandToQueue(queue, cmd);
			}
		}

		void DeviceInterface::BeginRender(void)
		{
			PipelineManager::GetInstance()->BeginRender();
		}

		void DeviceInterface::EndRender(void)
		{
			m_CommandsHolder->Swap();

			PipelineManager::GetInstance()->EndRender();
		}

		void DeviceInterface::BeginEvent(const String& Label, RenderQueues Queue)
		{
			BeginEvent(Label.ChangeType<char16>(), Queue);
		}

		void DeviceInterface::BeginEvent(const WString& Label, RenderQueues Queue)
		{
			BeginEventCommand* cmd = AllocateCommand<BeginEventCommand>(m_CommandsHolder, Queue);
			Construct(cmd, Label);
			AddCommandToQueue(Queue, cmd);
		}

		void DeviceInterface::EndEvent(RenderQueues Queue)
		{
			EndEventCommand* cmd = AllocateCommand<EndEventCommand>(m_CommandsHolder, Queue);
			Construct(cmd);
			AddCommandToQueue(Queue, cmd);
		}

		void DeviceInterface::SetMarker(const String& Label, RenderQueues Queue)
		{
			SetMarker(Label.ChangeType<char16>(), Queue);
		}

		void DeviceInterface::SetMarker(const WString& Label, RenderQueues Queue)
		{
			SetMarkerCommand* cmd = AllocateCommand<SetMarkerCommand>(m_CommandsHolder, Queue);
			Construct(cmd, Label);
			AddCommandToQueue(Queue, cmd);
		}

		void DeviceInterface::DestroyContextInternal(RenderContext* Context)
		{
			if (m_CurentContext == Context && m_CurentContext != nullptr)
				m_CurentContext->GetWindow()->OnSizeChangedEvent -= EventListener_OnWindowSizeChanged;

			CHECK_CALL_STRONG(m_ThreadedDevice->DestroyContext(Context->GetHandle()));

			RenderingAllocators::RenderingSystemAllocator_Deallocate(Context);
		}

		void DeviceInterface::AddCommandToQueue(RenderQueues Queue, CommandBase* Command)
		{
			CommandList* frontCommands = m_CommandsHolder->GetFrontCommandQueue();

			(frontCommands[(int8)Queue]).Add(Command);
		}

		void DeviceInterface::OnWindowSizeChanged(Window* Window)
		{
			CHECK_CALL_STRONG(m_ThreadedDevice->SetContextSize(Window->GetClientSize()));

			OnContextResizedEvent(m_CurentContext);
		}
	}
}