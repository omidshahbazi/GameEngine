// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\GPUBuffer.h>
#include <RenderSystem\Private\ThreadedDevice.h>
#include <RenderDevice\ICommandBuffer.h>

namespace Engine
{
	using namespace RenderDevice;

	namespace RenderSystem
	{
		GPUBuffer::GPUBuffer(ThreadedDevice* Device, ResourceHandle Handle, uint32 Size) :
			NativeType(Device, Handle),
			m_Size(Size),
			m_IsLocked(false),
			m_StartBuffer(nullptr),
			m_CurrentBuffer(nullptr),
			m_LastLockAccess(GPUBufferAccess::ReadOnly)
		{
			SetName("GPUBuffer");
		}

		GPUBuffer::~GPUBuffer(void)
		{
			if (GetDevice() == nullptr)
				return;

			GetDevice()->DestroyBuffer(GetHandle());
		}

		void GPUBuffer::SetNameInternal(const WString& Name)
		{
			GetDevice()->SetResourceName(GetHandle(), IDevice::ResourceTypes::Buffer, GetName().GetValue());
		}

		byte* GPUBuffer::GetBuffer(GPUBufferAccess Access, bool Directly)
		{
			m_LastLockAccess = Access;

			byte* buffer = nullptr;

			if (Directly)
			{
				if (!GetDevice()->GetDevice()->LockBuffer(GetHandle(), Access, &buffer))
					return nullptr;
			}
			else
			{
				if (!GetDevice()->LockBuffer(GetHandle(), Access, &buffer).Wait())
					return nullptr;
			}

			return buffer;
		}

		void GPUBuffer::UngetBuffer(bool Directly)
		{
			if (Directly)
				GetDevice()->GetDevice()->UnlockBuffer(GetHandle());
			else
				GetDevice()->UnlockBuffer(GetHandle());
		}

		byte* GPUBuffer::Lock(GPUBufferAccess Access, bool Directly)
		{
			m_StartBuffer = GetBuffer(Access, Directly);
			if (m_StartBuffer == nullptr)
				return nullptr;

			m_CurrentBuffer = m_StartBuffer;

			m_IsLocked = true;

			return m_CurrentBuffer;
		}

		void GPUBuffer::Unlock(bool Directly)
		{
			UngetBuffer(Directly);

			m_IsLocked = false;
			m_StartBuffer = nullptr;
			m_CurrentBuffer = nullptr;
		}

		void GPUBuffer::CopyTo(ResourceHandle Handle)
		{
			ICommandBuffer* cb = nullptr;
			if (!GetDevice()->CreateCommandBuffer(cb).Wait())
				return;

			cb->BeginEvent(L"CopyCommandBuffer");
			cb->CopyBuffer(GetHandle(), Handle);
			cb->EndEvent();

			if (!GetDevice()->SubmitCommandBuffers(&cb, 1).Wait())
				return;

			GetDevice()->DestroyCommandBuffers(&cb, 1).Wait();
		}
	}
}