// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\DeviceThread.h>
#include <Rendering\Private\ShaderCompiler\Compiler.h>
#include <Rendering\Private\Commands\ClearCommand.h>
#include <Rendering\Private\RenderingAllocators.h>

namespace Engine
{
	namespace Rendering
	{
		using namespace Private::ShaderCompiler;

		namespace Private
		{
#define CHECK_DEVICE() Assert(m_Device != nullptr, "m_Device cannot be null")

#define BEGIN_CALL(ResultType, ...) \
			PromiseBlock<ResultType>* promise = AllocatePromiseBlock<ResultType>(RenderingAllocators::ContainersAllocator, [](PromiseBlockBase* Block) { RenderingAllocators::ContainersAllocator_Deallocate(Block); }, 1); \
			m_TasksLock.Lock(); \
			m_Tasks.Enqueue(std::make_shared<Task>([__VA_ARGS__]() \
			{ \
				CHECK_DEVICE();

#define END_CALL() \
			promise->IncreaseDoneCount(); \
			})); \
			m_TasksLock.Release(); \
			return promise;

			DeviceThread::DeviceThread(IDevice* Device, DeviceTypes DeviceType) :
				m_Device(Device),
				m_DeviceType(DeviceType)
			{
				//for (int8 i = 0; i < (int8)RenderQueues::COUNT; ++i)
				//	m_CommandQueues[i] = CommandList(RenderingAllocators::ContainersAllocator, 10000000);

				m_Thread.Initialize([&](void*) { Worker(); });
			}

			DeviceThread::~DeviceThread(void)
			{
			}

			Promise<bool> DeviceThread::Initialize(void)
			{
				BEGIN_CALL(bool, &, promise);

				promise->SetValue(m_Device->Initialize());

				END_CALL();
			}

			Promise<cstr> DeviceThread::GetVersion(void)
			{
				BEGIN_CALL(cstr, &, promise);

				promise->SetValue(m_Device->GetVersion());

				END_CALL();
			}

			Promise<cstr> DeviceThread::GetVendorName(void)
			{
				BEGIN_CALL(cstr, &, promise);

				promise->SetValue(m_Device->GetVendorName());

				END_CALL();
			}

			Promise<cstr> DeviceThread::GetRendererName(void)
			{
				BEGIN_CALL(cstr, &, promise);

				promise->SetValue(m_Device->GetRendererName());

				END_CALL();
			}

			Promise<cstr> DeviceThread::GetShadingLanguageVersion(void)
			{
				BEGIN_CALL(cstr, &, promise);

				promise->SetValue(m_Device->GetShadingLanguageVersion());

				END_CALL();
			}

			Promise<RenderContext*> DeviceThread::CreateContext(PlatformWindow::WindowHandle Handle)
			{
				BEGIN_CALL(RenderContext*, &, promise, Handle);

				promise->SetValue(m_Device->CreateContext(Handle));

				END_CALL();
			}

			Promise<bool> DeviceThread::DestroyContext(RenderContext* Context)
			{
				BEGIN_CALL(bool, &, promise);

				promise->SetValue(m_Device->DestroyContext(Context));

				END_CALL();
			}

			Promise<bool> DeviceThread::SetContext(RenderContext* Context)
			{
				BEGIN_CALL(bool, &, promise);

				promise->SetValue(m_Device->SetContext(Context));

				END_CALL();
			}

			Promise<RenderContext*> DeviceThread::GetContext(void)
			{
				BEGIN_CALL(RenderContext*, &, promise);

				promise->SetValue(m_Device->GetContext());

				END_CALL();
			}

			Promise<bool> DeviceThread::SetViewport(const Vector2I& Position, const Vector2I& Size)
			{
				BEGIN_CALL(bool, &, promise);

				promise->SetValue(m_Device->SetViewport(Position, Size));

				END_CALL();
			}

			Promise<bool> DeviceThread::SetClearColor(const ColorUI8& Color)
			{
				BEGIN_CALL(bool, &, promise);

				promise->SetValue(m_Device->SetClearColor(Color));

				END_CALL();
			}

			Promise<bool> DeviceThread::SetFaceOrder(IDevice::FaceOrders Order)
			{
				BEGIN_CALL(bool, &, promise, Order);

				promise->SetValue(m_Device->SetFaceOrder(Order));

				END_CALL();
			}

			Promise<bool> DeviceThread::SetCullMode(IDevice::CullModes Mode)
			{
				BEGIN_CALL(bool, &, promise, Mode);

				promise->SetValue(m_Device->SetCullMode(Mode));

				END_CALL();
			}

			Promise<bool> DeviceThread::SetDepthTestFunction(IDevice::TestFunctions Function)
			{
				BEGIN_CALL(bool, &, promise, Function);

				promise->SetValue(m_Device->SetDepthTestFunction(Function));

				END_CALL();
			}

			Promise<bool> DeviceThread::SetStencilTestFunction(IDevice::CullModes CullMode, IDevice::TestFunctions Function, int32 Reference, uint32 Mask)
			{
				BEGIN_CALL(bool, &, promise, CullMode, Function, Reference, Mask);

				promise->SetValue(m_Device->SetStencilTestFunction(CullMode, Function, Reference, Mask));

				END_CALL();
			}

			Promise<bool> DeviceThread::SetStencilMask(IDevice::CullModes CullMode, uint32 Mask)
			{
				BEGIN_CALL(bool, &, promise, CullMode, Mask);

				promise->SetValue(m_Device->SetStencilMask(CullMode, Mask));

				END_CALL();
			}

			Promise<bool> DeviceThread::SetStencilOperation(IDevice::CullModes CullMode, IDevice::StencilOperations StencilFailed, IDevice::StencilOperations DepthFailed, IDevice::StencilOperations DepthPassed)
			{
				BEGIN_CALL(bool, &, promise, CullMode, StencilFailed, DepthFailed, DepthPassed);

				promise->SetValue(m_Device->SetStencilOperation(CullMode, StencilFailed, DepthFailed, DepthPassed));

				END_CALL();
			}

			Promise<bool> DeviceThread::SetBlendEquation(IDevice::BlendEquations Equation)
			{
				BEGIN_CALL(bool, &, promise, Equation);

				promise->SetValue(m_Device->SetBlendEquation(Equation));

				END_CALL();
			}

			Promise<bool> DeviceThread::SetBlendFunction(IDevice::BlendFunctions SourceFactor, IDevice::BlendFunctions DestinationFactor)
			{
				BEGIN_CALL(bool, &, promise, SourceFactor, DestinationFactor);

				promise->SetValue(m_Device->SetBlendFunction(SourceFactor, DestinationFactor));

				END_CALL();
			}

			Promise<bool> DeviceThread::SetPolygonMode(IDevice::CullModes CullMode, IDevice::PolygonModes PolygonMode)
			{
				BEGIN_CALL(bool, &, promise, CullMode, PolygonMode);

				promise->SetValue(m_Device->SetPolygonMode(CullMode, PolygonMode));

				END_CALL();
			}

			Promise<bool> DeviceThread::ResetState(void)
			{
				BEGIN_CALL(bool, &, promise);

				promise->SetValue(m_Device->ResetState());

				END_CALL();
			}

			Promise<IDevice::State> DeviceThread::GetState(void)
			{
				BEGIN_CALL(IDevice::State, &, promise);

				promise->SetValue(m_Device->GetState());

				END_CALL();
			}

			Promise<void> DeviceThread::SetState(const IDevice::State& State)
			{
				BEGIN_CALL(void, &, promise);

				m_Device->SetState(State);

				END_CALL();
			}

			Promise<bool> DeviceThread::CreateBuffer(GPUBuffer::Handle& Handle)
			{
				BEGIN_CALL(bool, &, promise);

				promise->SetValue(m_Device->CreateBuffer(Handle));

				END_CALL();
			}

			Promise<bool> DeviceThread::DestroyBuffer(GPUBuffer::Handle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->DestroyBuffer(Handle));

				END_CALL();
			}

			Promise<bool> DeviceThread::BindBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type);

				promise->SetValue(m_Device->BindBuffer(Handle, Type));

				END_CALL();
			}

			Promise<bool> DeviceThread::AttachBufferData(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Usages Usage, uint32 Size, const void* Data)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type, Usage, Size);

				promise->SetValue(m_Device->AttachBufferData(Handle, Type, Usage, Size, Data));

				END_CALL();
			}

			Promise<bool> DeviceThread::AttachBufferData(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Usages Usage, uint32 Size, Texture::Handle TextureHandle, Texture::Types TextureType, Texture::Formats TextureFormat, uint32 Level)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type, Usage, Size, TextureHandle, TextureType, TextureFormat, Level);

				promise->SetValue(m_Device->AttachBufferData(Handle, Type, Usage, Size, TextureHandle, TextureType, TextureFormat, Level));

				END_CALL();
			}

			Promise<bool> DeviceThread::ReadBufferData(GPUBuffer::Handle Handle, GPUBuffer::Types Type, Texture::Handle TextureHandle, Texture::Types TextureType, uint32 Width, uint32 Height, Texture::Formats TextureFormat)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type, TextureHandle, TextureType, Width, Height, TextureFormat);

				promise->SetValue(m_Device->ReadBufferData(Handle, Type, TextureHandle, TextureType, Width, Height, TextureFormat));

				END_CALL();
			}

			Promise<bool> DeviceThread::LockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Access Access, byte** Buffer)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type, Access);

				promise->SetValue(m_Device->LockBuffer(Handle, Type, Access, Buffer));

				END_CALL();
			}

			Promise<bool> DeviceThread::UnlockBuffer(GPUBuffer::Types Type)
			{
				BEGIN_CALL(bool, &, promise, Type);

				promise->SetValue(m_Device->UnlockBuffer(Type));

				END_CALL();
			}

			Promise<bool> DeviceThread::CreateShader(const IDevice::Shaders* Shaders, Shader::Handle& Handle, cstr* ErrorMessage)
			{
				BEGIN_CALL(bool, &, promise);

				promise->SetValue(m_Device->CreateShader(Shaders, Handle, ErrorMessage));

				END_CALL();
			}

			Promise<bool> DeviceThread::DestroyShader(Shader::Handle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->DestroyShader(Handle));

				END_CALL();
			}

			Promise<bool> DeviceThread::BindShader(Shader::Handle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->BindShader(Handle));

				END_CALL();
			}

			Promise<bool> DeviceThread::QueryShaderActiveConstants(Shader::Handle Handle, Shader::ConstantDataList& Constants)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->QueryShaderActiveConstants(Handle, Constants));

				END_CALL();
			}

			Promise<bool> DeviceThread::GetShaderConstantHandle(Shader::Handle Handle, const String& Name, Shader::ConstantHandle& ConstantHandle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->GetShaderConstantHandle(Handle, Name, ConstantHandle));

				END_CALL();
			}

			Promise<bool> DeviceThread::SetShaderFloat32(Shader::ConstantHandle Handle, float32 Value)
			{
				BEGIN_CALL(bool, &, promise, Handle, Value);

				promise->SetValue(m_Device->SetShaderFloat32(Handle, Value));

				END_CALL();
			}

			Promise<bool> DeviceThread::SetShaderColor(Shader::ConstantHandle Handle, const ColorUI8& Value)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->SetShaderColor(Handle, Value));

				END_CALL();
			}

			Promise<bool> DeviceThread::SetShaderVector2(Shader::ConstantHandle Handle, const Vector2F& Value)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->SetShaderVector2(Handle, Value));

				END_CALL();
			}

			Promise<bool> DeviceThread::SetShaderVector3(Shader::ConstantHandle Handle, const Vector3F& Value)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->SetShaderVector3(Handle, Value));

				END_CALL();
			}

			Promise<bool> DeviceThread::SetShaderVector4(Shader::ConstantHandle Handle, const Vector4F& Value)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->SetShaderVector4(Handle, Value));

				END_CALL();
			}

			Promise<bool> DeviceThread::SetShaderMatrix4(Shader::ConstantHandle Handle, const Matrix4F& Value)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->SetShaderMatrix4(Handle, Value));

				END_CALL();
			}

			Promise<bool> DeviceThread::SetShaderTexture(Shader::ConstantHandle Handle, Texture::Types Type, Texture::Handle Value)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type, Value);

				promise->SetValue(m_Device->SetShaderTexture(Handle, Type, Value));

				END_CALL();
			}

			Promise<bool> DeviceThread::CreateTexture(const TextureInfo* Info, Texture::Handle& Handle)
			{
				BEGIN_CALL(bool, &, promise);

				promise->SetValue(m_Device->CreateTexture(Info, Handle));

				END_CALL();
			}

			Promise<bool> DeviceThread::DestroyTexture(Texture::Handle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->DestroyTexture(Handle));

				END_CALL();
			}

			Promise<bool> DeviceThread::BindTexture(Texture::Handle Handle, Texture::Types Type)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type);

				promise->SetValue(m_Device->BindTexture(Handle, Type));

				END_CALL();
			}

			Promise<bool> DeviceThread::SetTextureVerticalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type, Mode);

				promise->SetValue(m_Device->SetTextureVerticalWrapping(Handle, Type, Mode));

				END_CALL();
			}

			Promise<bool> DeviceThread::SetTextureHorizontalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type, Mode);

				promise->SetValue(m_Device->SetTextureHorizontalWrapping(Handle, Type, Mode));

				END_CALL();
			}

			Promise<bool> DeviceThread::SetTextureMinifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MinifyFilters Filter)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type, Filter);

				promise->SetValue(m_Device->SetTextureMinifyFilter(Handle, Type, Filter));

				END_CALL();
			}

			Promise<bool> DeviceThread::SetTextureMagnifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MagnfyFilters Filter)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type, Filter);

				promise->SetValue(m_Device->SetTextureMagnifyFilter(Handle, Type, Filter));

				END_CALL();
			}

			Promise<bool> DeviceThread::GenerateTextureMipMap(Texture::Handle Handle, Texture::Types Type)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type);

				promise->SetValue(m_Device->GenerateTextureMipMap(Handle, Type));

				END_CALL();
			}

			Promise<bool> DeviceThread::CreateRenderTarget(const RenderTargetInfo* Info, RenderTarget::Handle& Handle, IDevice::TextureList& Textures)
			{
				BEGIN_CALL(bool, &, promise);

				promise->SetValue(m_Device->CreateRenderTarget(Info, Handle, Textures));

				END_CALL();
			}

			Promise<bool> DeviceThread::DestroyRenderTarget(RenderTarget::Handle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->DestroyRenderTarget(Handle));

				END_CALL();
			}

			Promise<bool> DeviceThread::BindRenderTarget(RenderTarget::Handle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->BindRenderTarget(Handle));

				END_CALL();
			}

			Promise<bool> DeviceThread::CreateMesh(const SubMeshInfo* Info, GPUBuffer::Usages Usage, SubMesh::Handle& Handle)
			{
				BEGIN_CALL(bool, &, promise, Usage);

				promise->SetValue(m_Device->CreateMesh(Info, Usage, Handle));

				END_CALL();
			}

			Promise<bool> DeviceThread::GetMeshVertexBuffer(SubMesh::Handle Handle, GPUBuffer::Handle& BufferHandle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->GetMeshVertexBuffer(Handle, BufferHandle));

				END_CALL();
			}

			Promise<bool> DeviceThread::GetMeshElementBuffer(SubMesh::Handle Handle, GPUBuffer::Handle& BufferHandle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->GetMeshElementBuffer(Handle, BufferHandle));

				END_CALL();
			}

			Promise<bool> DeviceThread::DestroyMesh(SubMesh::Handle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->DestroyMesh(Handle));

				END_CALL();
			}

			Promise<bool> DeviceThread::BindMesh(SubMesh::Handle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->BindMesh(Handle));

				END_CALL();
			}

			Promise<bool> DeviceThread::SetDebugCallback(IDevice::DebugProcedureType Callback)
			{
				BEGIN_CALL(bool, &, promise, Callback);

				promise->SetValue(m_Device->SetDebugCallback(Callback));

				END_CALL();
			}

			void DeviceThread::CopyCommands(const CommandList* CommandLists)
			{
			}

			void DeviceThread::EraseCommands(void)
			{
				for (int8 i = (int8)RenderQueues::Default; i < (int8)RenderQueues::COUNT; ++i)
				{
					auto& commands = m_CommandQueues[i];

					for each (auto command in commands)
						DestructMacro(CommandBase, command);

					commands.Clear();

					RenderingAllocators::CommandAllocators[i]->Reset();
				}
			}

			Promise<void> DeviceThread::Render(void)
			{
				BEGIN_CALL(void, &, promise);

				for (int8 i = (int8)RenderQueues::Default; i < (int8)RenderQueues::COUNT; ++i)
				{
					auto& commands = m_CommandQueues[i];

					for each (auto command in commands)
						command->Execute(m_Device);
				}

				m_Device->SwapBuffers();

				END_CALL();
			}

			//void DeviceThread::BeginRender(void)
			//{
			//	PipelineManager::GetInstance()->BeginRender();
			//}

			//void DeviceThread::EndRender(void)
			//{
			//	CHECK_DEVICE();

			//	RenderQueue(RenderQueues::Default, RenderQueues::HUD);

			//	EraseQueue(RenderQueues::Default, RenderQueues::HUD);

			//	CHECK_CALL(m_Device->SwapBuffers());

			//	PipelineManager::GetInstance()->EndRender();
			//}

			//void DeviceThread::RenderQueue(RenderQueues From, RenderQueues To)
			//{
			//	for (int8 i = (int8)From; i <= (int8)To; ++i)
			//	{
			//		auto& commands = m_CommandQueues[i];

			//		for each (auto command in commands)
			//			command->Execute(m_Device);
			//	}
			//}

			//void DeviceThread::EraseQueue(RenderQueues From, RenderQueues To)
			//{
			//	for (int8 i = (int8)From; i <= (int8)To; ++i)
			//	{
			//		auto& commands = m_CommandQueues[i];

			//		for each (auto command in commands)
			//			DestructMacro(CommandBase, command);

			//		commands.Clear();

			//		RenderingAllocators::CommandAllocators[i]->Reset();
			//	}
			//}

			void DeviceThread::Worker(void)
			{
				while (true)
				{
					PlatformThread::Sleep(1);

					if (!m_TasksLock.TryLock())
						continue;

					TaskPtr task;
					if (m_Tasks.GetSize() != 0)
						m_Tasks.Dequeue(&task);

					m_TasksLock.Release();

					if (task == nullptr)
						continue;

					(*task)();
				}
			}
		}
	}
}
