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

		void* GPUBuffer::Map(void)
		{
			void* buffer = nullptr;

			if (!GetDevice()->MapBuffer(GetHandle(), m_Type, m_Access, &buffer))
				return nullptr;

			return buffer;
		}

		void GPUBuffer::Unmap(void)
		{
			GetDevice()->UnmapBuffer(m_Type);
		}
	}
}