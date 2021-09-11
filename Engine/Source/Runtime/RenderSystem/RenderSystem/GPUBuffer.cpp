// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\GPUBuffer.h>
#include <RenderSystem\Private\ThreadedDevice.h>

namespace Engine
{
 namespace RenderSystem
	{
		GPUBuffer::GPUBuffer(ThreadedDevice* Device, ResourceHandle Handle, uint32 Size, GPUBufferTypes Type) :
			NativeType(Device, Handle),
			m_Size(Size),
			m_Type(Type),
			m_IsLocked(false),
			m_StartBuffer(nullptr),
			m_CurrentBuffer(nullptr),
			m_LastLockAccess(GPUBufferAccess::ReadOnly)
		{
		}

		GPUBuffer::~GPUBuffer(void)
		{
			if (GetDevice() == nullptr)
				return;

			GetDevice()->DestroyBuffer(GetHandle());
		}

		byte* GPUBuffer::GetBuffer(GPUBufferAccess Access, bool Directly)
		{
			m_LastLockAccess = Access;

			byte* buffer = nullptr;

			if (Directly)
			{
				if (!GetDevice()->GetDevice()->LockBuffer(GetHandle(), m_Type, Access, &buffer))
					return nullptr;
			}
			else
			{
				if (!GetDevice()->LockBuffer(GetHandle(), m_Type, Access, &buffer).Wait())
					return nullptr;
			}

			return buffer;
		}

		void GPUBuffer::UngetBuffer(bool Directly)
		{
			if (Directly)
				GetDevice()->GetDevice()->UnlockBuffer(GetHandle(), m_Type);
			else
				GetDevice()->UnlockBuffer(GetHandle(), m_Type);
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
	}
}