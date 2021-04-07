// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\CPUConstantBuffer.h>
#include <Rendering\Private\RenderingAllocators.h>

namespace Engine
{
	namespace Rendering
	{
		using namespace Private;

		CPUConstantBuffer::CPUConstantBuffer(uint32 Size) :
			ConstantBuffer(nullptr, Size, 0)
		{
			m_Address = RenderingAllocators::ContainersAllocator_AllocateArray<byte>(Size);
		}

		CPUConstantBuffer::~CPUConstantBuffer(void)
		{
			RenderingAllocators::ContainersAllocator_Deallocate(m_Address);
		}

		void CPUConstantBuffer::Lock(Access Access)
		{
			ConstantBuffer::Lock(Access);
		}

		CPUConstantBuffer* CPUConstantBuffer::Clone(void) const
		{
			CPUConstantBuffer* buffer = RenderingAllocators::ContainersAllocator_Allocate<CPUConstantBuffer>();
			Construct(buffer, GetSize());

			buffer->Set(*buffer);

			return buffer;
		}
	}
}