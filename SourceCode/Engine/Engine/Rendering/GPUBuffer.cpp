// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\GPUBuffer.h>

namespace Engine
{
	namespace Rendering
	{
		GPUBuffer::GPUBuffer(IDevice* Device, Handle Handle, uint32 Size, IDevice::BufferTypes Type, IDevice::BufferAccess Access) :
			NativeType(Device, Handle),
			m_Size(Size),
			m_Type(Type),
			m_Access(Access),
			m_IsLocked(false),
			m_StartBuffer(nullptr),
			m_CurrentBuffer(nullptr)
		{
		}

		byte* GPUBuffer::Lock(void)
		{
			if (!GetDevice()->LockBuffer(GetHandle(), m_Type, m_Access, &m_StartBuffer))
				return nullptr;

			m_CurrentBuffer = m_StartBuffer;

			m_IsLocked = true;

			return m_CurrentBuffer;
		}

		void GPUBuffer::Unlock(void)
		{
			GetDevice()->UnlockBuffer(m_Type);

			m_IsLocked = false;
			m_StartBuffer = nullptr;
			m_CurrentBuffer = nullptr;
		}
	}
}