// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Private\ThreadedDevice.h>
#include <RenderSystem\Private\FrameConstantBuffers.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace Debugging;
	using namespace RenderCommon::Private;

	namespace RenderSystem
	{
		namespace Private
		{
#define CHECK_DEVICE() CoreDebugAssert(Categories::RenderSystem, m_Device != nullptr, "m_Device cannot be null")

#define BEGIN_CALL(ResultType, ...) \
			PromiseBlock<ResultType>* promise = AllocatePromiseBlock<ResultType>(RenderSystemAllocators::ContainersAllocator, 1); \
			promise->Grab(); \
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

			ThreadedDevice::ThreadedDevice(IDevice* Device, DeviceTypes DeviceType) :
				m_Device(Device),
				m_IsInitialized(false),
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

				m_IsInitialized = m_Device->Initialize();
				promise->SetValue(m_IsInitialized);

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

			Promise<bool> ThreadedDevice::CreateContext(PlatformWindow::WindowHandle WindowHandle, ResourceHandle& Handle)
			{
				BEGIN_CALL(bool, &, promise, WindowHandle);

				promise->SetValue(m_Device->CreateContext(WindowHandle, Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::DestroyContext(ResourceHandle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->DestroyContext(Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetContext(ResourceHandle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->SetContext(Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::CreateBuffer(GPUBufferTypes Type, uint32 Size, ResourceHandle& Handle)
			{
				BEGIN_CALL(bool, &, promise, Type, Size);

				promise->SetValue(m_Device->CreateBuffer(Type, Size, Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::DestroyBuffer(ResourceHandle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->DestroyBuffer(Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::LockBuffer(ResourceHandle Handle, GPUBufferAccess Access, byte** Buffer)
			{
				BEGIN_CALL(bool, &, promise, Handle, Access, Buffer);

				promise->SetValue(m_Device->LockBuffer(Handle, Access, Buffer));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::UnlockBuffer(ResourceHandle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->UnlockBuffer(Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::CreateProgram(const IDevice::CompiledShaders* Shaders, ResourceHandle& Handle, cstr* ErrorMessage)
			{
				BEGIN_CALL(bool, &, promise, Shaders, ErrorMessage);

				promise->SetValue(m_Device->CreateProgram(Shaders, Handle, ErrorMessage));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::DestroyProgram(ResourceHandle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->DestroyProgram(Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::CreateTexture(const TextureInfo* Info, ResourceHandle& Handle)
			{
				BEGIN_CALL(bool, &, promise, Info);

				promise->SetValue(m_Device->CreateTexture(Info, Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::DestroyTexture(ResourceHandle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->DestroyTexture(Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetTextureVerticalWrapping(ResourceHandle Handle, TextureTypes Type, TextureWrapModes Mode)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type, Mode);

				promise->SetValue(m_Device->SetTextureVerticalWrapping(Handle, Type, Mode));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetTextureHorizontalWrapping(ResourceHandle Handle, TextureTypes Type, TextureWrapModes Mode)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type, Mode);

				promise->SetValue(m_Device->SetTextureHorizontalWrapping(Handle, Type, Mode));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetTextureMinifyFilter(ResourceHandle Handle, TextureTypes Type, TextureMinifyFilters Filter)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type, Filter);

				promise->SetValue(m_Device->SetTextureMinifyFilter(Handle, Type, Filter));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetTextureMagnifyFilter(ResourceHandle Handle, TextureTypes Type, TextureMagnfyFilters Filter)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type, Filter);

				promise->SetValue(m_Device->SetTextureMagnifyFilter(Handle, Type, Filter));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::CreateRenderTarget(const RenderTargetInfo* Info, ResourceHandle& Handle, IDevice::TextureList& Textures)
			{
				BEGIN_CALL(bool, &, promise, Info);

				promise->SetValue(m_Device->CreateRenderTarget(Info, Handle, Textures));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::DestroyRenderTarget(ResourceHandle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->DestroyRenderTarget(Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::CreateMesh(const SubMeshInfo* Info, ResourceHandle& Handle)
			{
				BEGIN_CALL(bool, &, promise, Info);

				promise->SetValue(m_Device->CreateMesh(Info, Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::DestroyMesh(ResourceHandle Handle)
			{
				BEGIN_CALL(bool, &, promise, Handle);

				promise->SetValue(m_Device->DestroyMesh(Handle));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::CreateCommandBuffer(ICommandBuffer::Types Type, ICommandBuffer*& Buffer)
			{
				BEGIN_CALL(bool, &, promise, Type);

				promise->SetValue(m_Device->CreateCommandBuffer(Type, Buffer));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::DestroyCommandBuffer(ICommandBuffer** Buffers, uint16 Count)
			{
				BEGIN_CALL(bool, &, promise, Count);

				promise->SetValue(m_Device->DestroyCommandBuffer(Buffers, Count));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SubmitCommandBuffer(ICommandBuffer* const* Buffers, uint16 Count)
			{
				BEGIN_CALL(bool, &, promise, Buffers, Count);

				promise->SetValue(m_Device->SubmitCommandBuffer(Buffers, Count));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SubmitCommandBufferAsync(ICommandBuffer* const* Buffers, uint16 Count)
			{
				BEGIN_CALL(bool, &, promise, Buffers, Count);

				promise->SetValue(m_Device->SubmitCommandBufferAsync(Buffers, Count));

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SyncConstantBuffers(const FrameConstantBuffers* Buffers)
			{
				BEGIN_CALL(bool, &, promise, Buffers);

				Buffers->UploadBuffersToGPU(m_Device);

				promise->SetValue(true);

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SwapBuffers(void)
			{
				BEGIN_CALL(bool, &, promise);

				promise->SetValue(m_Device->SwapBuffers());

				END_CALL();
			}

			Promise<bool> ThreadedDevice::SetResourceName(ResourceHandle Handle, IDevice::ResourceTypes Type, cwstr Name)
			{
				BEGIN_CALL(bool, &, promise, Handle, Type, Name);

				m_Device->SetResourceName(Handle, Type, Name);

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