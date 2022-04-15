// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\ConstantBuffer.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>

namespace Engine
{
	using namespace RenderCommon::Private;

	namespace RenderSystem
	{
		using namespace Private;

		ConstantBuffer::ConstantBuffer(uint32 Size) :
			m_Size(0),
			m_CachedData(nullptr),
			m_CurrentCachedData(nullptr)
		{
			Resize(Size);
		}

		ConstantBuffer::~ConstantBuffer(void)
		{
			RenderSystemAllocators::ContainersAllocator_Deallocate(m_CachedData);
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

		void ConstantBuffer::Resize(uint32 Size)
		{
			if (m_Size >= Size)
				return;

			m_Size = Size;

			m_CachedData = m_CurrentCachedData = RenderSystemAllocators::ContainersAllocator_ReallocateArray<byte>(m_CachedData, m_Size);
		}
	}
}