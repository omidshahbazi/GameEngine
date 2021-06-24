// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\CPUConstantBuffer.h>
#include <Rendering\Private\RenderingAllocators.h>

namespace Engine
{
	namespace Rendering
	{
		using namespace Private;

		CPUConstantBuffer1::CPUConstantBuffer1(uint32 Size) :
			ConstantBuffer(nullptr, Size, 0)
		{
			m_Address = RenderingAllocators::ContainersAllocator_AllocateArray<byte>(Size);
		}

		CPUConstantBuffer1::~CPUConstantBuffer1(void)
		{
			RenderingAllocators::ContainersAllocator_Deallocate(m_Address);
		}

		void CPUConstantBuffer1::Lock(Access Access)
		{
			ConstantBuffer::Lock(Access);
		}

		CPUConstantBuffer1* CPUConstantBuffer1::Clone(void) const
		{
			CPUConstantBuffer1* buffer = RenderingAllocators::ContainersAllocator_Allocate<CPUConstantBuffer1>();
			Construct(buffer, GetSize());

			buffer->Set(*buffer);

			return buffer;
		}
	}
}