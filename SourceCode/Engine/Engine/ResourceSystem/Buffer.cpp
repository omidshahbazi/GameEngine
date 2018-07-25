// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <ResourceSystem\Buffer.h>
#include <Platform\PlatformMemory.h>

namespace Engine
{
	using namespace Platform;

	namespace ResourceSystem
	{
		void Buffer::Allocate(const uint64 &Size)
		{
			m_Capacity = m_Size = Size;
			m_Buffer = AllocateMemory(m_Allocator, m_Capacity);
		}

		void Buffer::Deallocate(void)
		{
			if (m_Buffer == nullptr)
				return;

			DeallocateMemory(m_Allocator, m_Buffer);
			m_Capacity = m_Size = 0;
		}

		void Buffer::Move(Buffer &Buffer)
		{
			m_Allocator = Buffer.m_Allocator;
			m_Buffer = Buffer.m_Buffer;
			m_Capacity = Buffer.m_Capacity;
			m_Size = Buffer.m_Size;

			Buffer.m_Buffer = nullptr;
			Buffer.m_Size = 0;
		}

		void Buffer::Copy(const byte *const Buffer, const uint64 &Index, const uint64 &Size)
		{
			bool mustRealloc = (m_Capacity != Size);

			if (mustRealloc)
			{
				Deallocate();

				m_Capacity = m_Size = Size;
			}

			if (m_Size == 0)
				return;

			if (mustRealloc)
				Allocate(m_Size);

			PlatformMemory::Copy(Buffer, Index, m_Buffer, 0, m_Size);
		}
	}
}