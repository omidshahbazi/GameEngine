// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\ConstantBuffer.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\Private\ThreadedDevice.h>
#include <Rendering\Private\RenderingAllocators.h>

namespace Engine
{
	namespace Rendering
	{
		using namespace Private;

		ConstantBuffer::ConstantBuffer(ThreadedDevice* Device, uint32 Size, Handle Handle) :
			GPUBuffer(Device, Handle, Size, Types::Constant),
			m_CachedData(nullptr),
			m_CurrentCachedData(nullptr),
			m_IsDirty(false)
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

		void ConstantBuffer::Copy(const ConstantBuffer& Other)
		{
			PlatformMemory::Copy(Other.m_CachedData, m_CachedData, Mathematics::Min<uint16>(GetSize(), Other.GetSize()));

			m_IsDirty = true;
		}

		void ConstantBuffer::Set(const byte* Data, uint16 Size)
		{
			PlatformMemory::Copy(Data, m_CurrentCachedData, Mathematics::Min<uint16>(GetSize(), Size));

			m_IsDirty = true;
		}

		void ConstantBuffer::UploadToGPU(void)
		{
			if (!m_IsDirty)
				return;

			GPUBuffer::Lock(Access::WriteOnly, true);

			PlatformMemory::Copy(m_CachedData, GetCurrentBuffer(), GetSize());

			GPUBuffer::Unlock(true);

			m_IsDirty = false;
		}
	}
}