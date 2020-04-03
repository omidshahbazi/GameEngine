// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\GPUBuffer.h>

namespace Engine
{
	namespace Rendering
	{
		GPUBuffer::GPUBuffer(IDevice* Device, Handle Handle, IDevice::BufferTypes Type, IDevice::BufferAccess Access) :
			NativeType(Device, Handle),
			m_Type(Type),
			m_Access(Access)
		{
		}

		byte* GPUBuffer::Lock(void)
		{
			byte* buffer = nullptr;

			if (!GetDevice()->LockBuffer(GetHandle(), m_Type, m_Access, &buffer))
				return nullptr;

			return buffer;
		}

		void GPUBuffer::Unlock(void)
		{
			GetDevice()->UnlockBuffer(m_Type);
		}
	}
}