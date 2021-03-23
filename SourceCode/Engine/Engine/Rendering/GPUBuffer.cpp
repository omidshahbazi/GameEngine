// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\GPUBuffer.h>
#include <Rendering\Private\ThreadedDevice.h>

namespace Engine
{
	namespace Rendering
	{
		GPUBuffer::GPUBuffer(ThreadedDevice* Device, Handle Handle, uint32 Size, Types Type) :
			NativeType(Device, Handle),
			m_Size(Size),
			m_Type(Type),
			m_IsLocked(false),
			m_StartBuffer(nullptr),
			m_CurrentBuffer(nullptr),
			m_LastLockAccess(Access::ReadOnly)
		{
		}

		GPUBuffer::~GPUBuffer(void)
		{
			GetDevice()->DestroyBuffer(GetHandle());
		}

		byte* GPUBuffer::GetBuffer(Access Access)
		{
			byte* buffer = nullptr;

			if (!GetDevice()->LockBuffer(GetHandle(), m_Type, Access, &buffer).Wait())
				return nullptr;

			return buffer;
		}

		byte* GPUBuffer::Lock(Access Access)
		{
			m_StartBuffer = GetBuffer(Access);
			if (m_StartBuffer == nullptr)
				return nullptr;

			m_LastLockAccess = Access;

			m_CurrentBuffer = m_StartBuffer;

			m_IsLocked = true;

			return m_CurrentBuffer;
		}

		void GPUBuffer::Unlock(void)
		{
			GetDevice()->UnlockBuffer(GetHandle(), m_Type);

			m_IsLocked = false;
			m_StartBuffer = nullptr;
			m_CurrentBuffer = nullptr;
		}
	}
}