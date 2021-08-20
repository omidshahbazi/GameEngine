// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\ConstantBuffer.h>
#include <Rendering\Private\RenderingAllocators.h>

namespace Engine
{
	namespace Rendering
	{
		using namespace Private;

		ConstantBuffer::ConstantBuffer(uint32 Size) :
			m_Size(Size),
			m_CachedData(nullptr),
			m_CurrentCachedData(nullptr)
		{
			m_CachedData = m_CurrentCachedData = RenderingAllocators::ContainersAllocator_AllocateArray<byte>(Size);
		}

		ConstantBuffer::~ConstantBuffer(void)
		{
			RenderingAllocators::ContainersAllocator_Deallocate(m_CachedData);
		}

		void ConstantBuffer::Reset(void)
		{
			m_CurrentCachedData = m_CachedData;
		}

		void ConstantBuffer::Move(uint32 Count)
		{
			m_CurrentCachedData += Count;
		}

		void ConstantBuffer::Copy(const ConstantBuffer* const Other)
		{
			PlatformMemory::Copy(Other->m_CachedData, m_CachedData, Math::Min<uint16>(GetSize(), Other->GetSize()));
		}

		void ConstantBuffer::Set(const byte* Data, uint16 Size)
		{
			uint32 remainSize = m_Size - (uint32)(m_CurrentCachedData - m_CachedData);

			PlatformMemory::Copy(Data, m_CurrentCachedData, Math::Min<uint16>(remainSize, Size));
		}
	}
}