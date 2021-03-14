// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\ThreadedDevice.h>
#include <Rendering\Private\ProgramCompiler\Compiler.h>
#include <Rendering\Private\RenderingAllocators.h>
#include <Rendering\Private\Commands\CommandBase.h>

namespace Engine
{
	namespace Rendering
	{
		using namespace Private::ProgramCompiler;

		namespace Private
		{
			using namespace Commands;

#define CHECK_DEVICE() Assert(m_Device != nullptr, "m_Device cannot be null")

#define BEGIN_CALL(ResultType, ...) \
			PromiseBlock<ResultType>* promise = AllocatePromiseBlock<ResultType>(RenderingAllocators::ContainersAllocator, [](PromiseBlockBase* Block) { RenderingAllocators::ContainersAllocator_Deallocate(Block); }, 1); \
			TaskPtr task = std::make_shared<Task>([__VA_ARGS__](bool ForceQuit) \
			{ \
				if (!ForceQuit) \
				{ \
					CHECK_DEVICE();

#define END_CALL() \
				} \
			promise->IncreaseDoneCount(); \
			promise->Drop(); \
			}); \
			m_TasksLock.Lock(); \
			m_Tasks.Enqueue(task); \
			m_TasksLock.Release(); \
			return promise;

			void RenderQueue(IDevice* Device, CommandList** Commands)
			{
				for (int8 i = 0; i < (int8)RenderQueues::COUNT; ++i)
				{
					auto& commands = *(Commands[i]);

					for (auto& command : commands)
						command->Execute(Device);
				}
			}

			ThreadedDevice::ThreadedDevice(IDevice* Device, DeviceTypes DeviceType) :
				m_Device(Device),
				m_DeviceType(DeviceType)
			{
				m_Thread.Initialize([&](void*) { Worker(); });
				m_Thread.SetName("ThreadedDevice Worker");
			}

			ThreadedDevice::~ThreadedDevice(void)
			{
				m_Thread.Shutdown().Wait();
			}

			Promise<bool> ThreadedDevice::Initialize(void)
			{
				BEGIN_CALL(bool, &, promise);

				promise->SetValue(m_Device->Initialize());

				END_CALL();
			}

			Promise<cstr> ThreadedDevice::GetVersion(void)
			{
				BEGIN_CALL(cstr, &, promise);

				promise->SetValue(m_Device->GetVersion());

				END_CALL();
			}

			Promise<cstr> ThreadedDevice::GetVendorName(void)
			{
				BEGIN_CALL(cstr, &, promise);

				promise->SetValue(m_Device->GetVendorName());

				END_CALL();
			}

			Promise<cstr> ThreadedDevice::GetRendererName(void)
			{
				BEGIN_CALL(cstr, &, promise);

				promise->SetValue(m_Device->GetRendererName());

				END_CALL();
			}

			Promise<cstr> ThreadedDevice::GetShadingLanguageVersion(void)
			{
				BEGIN_CALL(cstr, &, promise);

				promise->SetValue(m_Device->GetShadingLanguageVersion());

				END_CALL();
			}

			Promise<bool> ThreadedDevice::CreateContext(PlatformWindow::WindowHandle WindowHandle, RenderContext::Handle& Handle)
			{
				BEGIN_CALL(bool, &, promise, WindowHandle);

				promise->SetValue(m_Device->CreateContext(WindowHandle, Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::DestroyContext(RenderContext::Handle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->DestroyContext(Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetContext(RenderContext::Handle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->SetContext(Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetViewport(const Vector2I& Position, const Vector2I& Size)
			{
				BEGIN_CALL(bool, &, promise);

				promise->SetValue(m_Device->SetViewport(Position, Size));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetClearColor(const ColorUI8& Color)
			{
				BEGIN_CALL(bool, &, promise);

				promise->SetValue(m_Device->SetClearColor(Color));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetFaceOrder(IDevice::FaceOrders Order)
			{
				BEGIN_CALL(bool, &, promise, Order);

				promise->SetValue(m_Device->SetFaceOrder(Order));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetCullMode(IDevice::CullModes Mode)
			{
				BEGIN_CALL(bool, &, promise, Mode);

				promise->SetValue(m_Device->SetCullMode(Mode));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetDepthTestFunction(IDevice::TestFunctions Function)
			{
				BEGIN_CALL(bool, &, promise, Function);

				promise->SetValue(m_Device->SetDepthTestFunction(Function));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetStencilTestFunction(IDevice::CullModes CullMode, IDevice::TestFunctions Function, int32 Reference, uint32 Mask)
			{
				BEGIN_CALL(bool, &, promise, CullMode, Function, Reference, Mask);

				promise->SetValue(m_Device->SetStencilTestFunction(CullMode, Function, Reference, Mask));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetStencilMask(IDevice::CullModes CullMode, uint32 Mask)
			{
				BEGIN_CALL(bool, &, promise, CullMode, Mask);

				promise->SetValue(m_Device->SetStencilMask(CullMode, Mask));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetStencilOperation(IDevice::CullModes CullMode, IDevice::StencilOperations StencilFailed, IDevice::StencilOperations DepthFailed, IDevice::StencilOperations DepthPassed)
			{
				BEGIN_CALL(bool, &, promise, CullMode, StencilFailed, DepthFailed, DepthPassed);

				promise->SetValue(m_Device->SetStencilOperation(CullMode, StencilFailed, DepthFailed, DepthPassed));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetBlendEquation(IDevice::BlendEquations Equation)
			{
				BEGIN_CALL(bool, &, promise, Equation);

				promise->SetValue(m_Device->SetBlendEquation(Equation));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetBlendFunction(IDevice::BlendFunctions SourceFactor, IDevice::BlendFunctions DestinationFactor)
			{
				BEGIN_CALL(bool, &, promise, SourceFactor, DestinationFactor);

				promise->SetValue(m_Device->SetBlendFunction(SourceFactor, DestinationFactor));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetPolygonMode(IDevice::CullModes CullMode, IDevice::PolygonModes PolygonMode)
			{
				BEGIN_CALL(bool, &, promise, CullMode, PolygonMode);

				promise->SetValue(m_Device->SetPolygonMode(CullMode, PolygonMode));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::ResetState(void)
			{
				BEGIN_CALL(bool, &, promise);

				promise->SetValue(m_Device->ResetState());

				END_CALL();
			}

			Promise<IDevice::State> ThreadedDevice::GetState(void)
			{
				BEGIN_CALL(IDevice::State, &, promise);

				promise->SetValue(m_Device->GetState());

				END_CALL();
			}

			Promise<void> ThreadedDevice::SetState(const IDevice::State& State)
			{
				BEGIN_CALL(void, &, promise);

				m_Device->SetState(State);

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetResourceName(NativeType::Handle Handle, IDevice::ResourceTypes Type, cwstr Name)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type, Name);

				m_Device->SetResourceName(Handle, Type, Name);

				END_CALL();
			}

			Promise<bool> ThreadedDevice::CreateBuffer(GPUBuffer::Handle& Handle)
			{
				BEGIN_CALL(bool, &, promise);

				promise->SetValue(m_Device->CreateBuffer(Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::DestroyBuffer(GPUBuffer::Handle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->DestroyBuffer(Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::BindBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type);

				promise->SetValue(m_Device->BindBuffer(Handle, Type));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::CopyFromVertexToBuffer(GPUBuffer::Handle Handle, SubMesh::Handle FromMeshHandle, uint32 Size)
			{
				BEGIN_CALL(bool, &, promise, FromMeshHandle, Size);

				promise->SetValue(m_Device->CopyFromVertexToBuffer(Handle, FromMeshHandle, Size));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::CopyFromIndexToBuffer(GPUBuffer::Handle Handle, SubMesh::Handle FromMeshHandle, uint32 Size)
			{
				BEGIN_CALL(bool, &, promise, FromMeshHandle, Size);

				promise->SetValue(m_Device->CopyFromIndexToBuffer(Handle, FromMeshHandle, Size));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::CopyFromTextureToBuffer(GPUBuffer::Handle Handle, Texture::Handle FromTextureHandle, uint32 Size, Texture::Types TextureType, Texture::Formats TextureFormat, uint32 Level)
			{
				BEGIN_CALL(bool, &, promise, Handle, FromTextureHandle, Size, TextureType, TextureFormat, Level);

				promise->SetValue(m_Device->CopyFromTextureToBuffer(Handle, FromTextureHandle, Size, TextureType, TextureFormat, Level));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::CopyFromBufferToVertex(GPUBuffer::Handle Handle, Texture::Handle ToMeshHandle, uint32 Size)
			{
				BEGIN_CALL(bool, &, promise, Handle, ToMeshHandle, Size);

				promise->SetValue(m_Device->CopyFromBufferToVertex(Handle, ToMeshHandle, Size));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::CopyFromBufferToIndex(GPUBuffer::Handle Handle, Texture::Handle ToMeshHandle, uint32 Size)
			{
				BEGIN_CALL(bool, &, promise, Handle, ToMeshHandle, Size);

				promise->SetValue(m_Device->CopyFromBufferToIndex(Handle, ToMeshHandle, Size));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::CopyFromBufferToTexture(GPUBuffer::Handle Handle,Texture::Handle ToTextureHandle, Texture::Types TextureType, uint32 Width, uint32 Height, Texture::Formats TextureFormat)
			{
				BEGIN_CALL(bool, &, promise, Handle, ToTextureHandle, TextureType, Width, Height, TextureFormat);

				promise->SetValue(m_Device->CopyFromBufferToTexture(Handle, ToTextureHandle, TextureType, Width, Height, TextureFormat));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::LockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Access Access, byte** Buffer)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type, Access);

				promise->SetValue(m_Device->LockBuffer(Handle, Type, Access, Buffer));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::UnlockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type);

				promise->SetValue(m_Device->UnlockBuffer(Handle, Type));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::CompileProgram(const IDevice::Shaders* Shaders, IDevice::CompiledShaders* CompiledShaders, cstr* ErrorMessage)
			{
				BEGIN_CALL(bool, &, promise);

				promise->SetValue(m_Device->CompileProgram(Shaders, CompiledShaders, ErrorMessage));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::CreateProgram(const IDevice::CompiledShaders* Shaders, Program::Handle& Handle, cstr* ErrorMessage)
			{
				BEGIN_CALL(bool, &, promise);

				promise->SetValue(m_Device->CreateProgram(Shaders, Handle, ErrorMessage));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::DestroyProgram(Program::Handle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->DestroyProgram(Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::BindProgram(Program::Handle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->BindProgram(Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetProgramFloat32(Program::ConstantHandle Handle, float32 Value)
			{
				BEGIN_CALL(bool, &, promise, Handle, Value);

				promise->SetValue(m_Device->SetProgramFloat32(Handle, Value));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetProgramColor(Program::ConstantHandle Handle, const ColorUI8& Value)
			{
				BEGIN_CALL(bool, &, promise, Handle, Value);

				promise->SetValue(m_Device->SetProgramColor(Handle, Value));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetProgramVector2(Program::ConstantHandle Handle, const Vector2F& Value)
			{
				BEGIN_CALL(bool, &, promise, Handle, Value);

				promise->SetValue(m_Device->SetProgramVector2(Handle, Value));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetProgramVector3(Program::ConstantHandle Handle, const Vector3F& Value)
			{
				BEGIN_CALL(bool, &, promise, Handle, Value);

				promise->SetValue(m_Device->SetProgramVector3(Handle, Value));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetProgramVector4(Program::ConstantHandle Handle, const Vector4F& Value)
			{
				BEGIN_CALL(bool, &, promise, Handle, Value);

				promise->SetValue(m_Device->SetProgramVector4(Handle, Value));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetProgramMatrix4(Program::ConstantHandle Handle, const Matrix4F& Value)
			{
				BEGIN_CALL(bool, &, promise, Handle, Value);

				promise->SetValue(m_Device->SetProgramMatrix4(Handle, Value));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetProgramTexture(Program::ConstantHandle Handle, Texture::Types Type, Texture::Handle Value)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type, Value);

				promise->SetValue(m_Device->SetProgramTexture(Handle, Type, Value));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::CreateTexture(const TextureInfo* Info, Texture::Handle& Handle)
			{
				BEGIN_CALL(bool, &, promise);

				promise->SetValue(m_Device->CreateTexture(Info, Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::DestroyTexture(Texture::Handle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->DestroyTexture(Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::BindTexture(Texture::Handle Handle, Texture::Types Type)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type);

				promise->SetValue(m_Device->BindTexture(Handle, Type));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetTextureVerticalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type, Mode);

				promise->SetValue(m_Device->SetTextureVerticalWrapping(Handle, Type, Mode));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetTextureHorizontalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type, Mode);

				promise->SetValue(m_Device->SetTextureHorizontalWrapping(Handle, Type, Mode));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetTextureMinifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MinifyFilters Filter)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type, Filter);

				promise->SetValue(m_Device->SetTextureMinifyFilter(Handle, Type, Filter));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetTextureMagnifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MagnfyFilters Filter)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type, Filter);

				promise->SetValue(m_Device->SetTextureMagnifyFilter(Handle, Type, Filter));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::GenerateTextureMipMap(Texture::Handle Handle, Texture::Types Type)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type);

				promise->SetValue(m_Device->GenerateTextureMipMap(Handle, Type));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::CreateRenderTarget(const RenderTargetInfo* Info, RenderTarget::Handle& Handle, IDevice::TextureList& Textures)
			{
				BEGIN_CALL(bool, &, promise);

				promise->SetValue(m_Device->CreateRenderTarget(Info, Handle, Textures));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::DestroyRenderTarget(RenderTarget::Handle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->DestroyRenderTarget(Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::BindRenderTarget(RenderTarget::Handle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->BindRenderTarget(Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::CreateMesh(const SubMeshInfo* Info, SubMesh::Handle& Handle)
			{
				BEGIN_CALL(bool, &, promise);

				promise->SetValue(m_Device->CreateMesh(Info, Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::DestroyMesh(SubMesh::Handle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->DestroyMesh(Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::BindMesh(SubMesh::Handle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->BindMesh(Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::Clear(IDevice::ClearFlags Flags)
			{
				BEGIN_CALL(bool, &, promise, Flags);

				promise->SetValue(m_Device->Clear(Flags));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::DrawIndexed(SubMesh::PolygonTypes PolygonType, uint32 IndexCount)
			{
				BEGIN_CALL(bool, &, promise, PolygonType, IndexCount);

				promise->SetValue(m_Device->DrawIndexed(PolygonType, IndexCount));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::DrawArray(SubMesh::PolygonTypes PolygonType, uint32 VertexCount)
			{
				BEGIN_CALL(bool, &, promise, PolygonType, VertexCount);

				promise->SetValue(m_Device->DrawArray(PolygonType, VertexCount));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::BeginEvent(cwstr Label)
			{
				BEGIN_CALL(bool, &, promise, Label);

				promise->SetValue(m_Device->BeginEvent(Label));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::EndEvent(void)
			{
				BEGIN_CALL(bool, &, promise);

				promise->SetValue(m_Device->EndEvent());

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetMarker(cwstr Label)
			{
				BEGIN_CALL(bool, &, promise, Label);

				promise->SetValue(m_Device->SetMarker(Label));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetDebugCallback(IDevice::DebugFunction Callback)
			{
				BEGIN_CALL(bool, &, promise, Callback);

				promise->SetValue(m_Device->SetDebugCallback(Callback));

				END_CALL();
			}

			void ThreadedDevice::Worker(void)
			{
				TaskPtr task;
				while (!m_Thread.GetShouldExit())
				{
					if (m_Tasks.GetSize() != 0 && m_TasksLock.TryLock())
					{
						while (m_Tasks.GetSize() != 0)
						{
							m_Tasks.Dequeue(&task);

							(*task)(false);
						}

						m_TasksLock.Release();
					}

					if (m_CommandsHolder.TryLock())
					{
						m_Device->BeginExecute();

						RenderQueue(m_Device, m_CommandsHolder.GetBackCommandQueue());

						m_Device->EndExecute();

						m_Device->SwapBuffers();

						m_CommandsHolder.Release();
					}
				}

				while (m_Tasks.GetSize() != 0)
				{
					m_Tasks.Dequeue(&task);
					(*task)(true);
				}
			}
		}
	}
}