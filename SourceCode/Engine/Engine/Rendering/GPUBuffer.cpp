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
			m_CurrentBuffer(nullptr)
		{
		}

		GPUBuffer::~GPUBuffer(void)
		{
			GetDevice()->DestroyBuffer(GetHandle());
		}

		byte* GPUBuffer::Lock(Access Access)
		{
			if (!GetDevice()->LockBuffer(GetHandle(), m_Type, Access, &m_StartBuffer).Wait())
				return nullptr;

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