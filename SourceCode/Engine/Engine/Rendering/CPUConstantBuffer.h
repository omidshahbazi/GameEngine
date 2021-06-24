// Copyright 2013-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CPU_CONSTANT_BUFFER_H
#define CPU_CONSTANT_BUFFER_H

#include <Rendering\ConstantBuffer.h>

namespace Engine
{
	namespace Rendering
	{
		class RENDERING_API CPUConstantBuffer1 : public ConstantBuffer
		{
		public:
			CPUConstantBuffer1(uint32 Size);
			~CPUConstantBuffer1(void);

		public:
			void Lock(Access Access) override;

			void Unlock(void) override
			{
			}

			CPUConstantBuffer1* Clone(void) const;

			byte* GetBuffer(Access Access)
			{
				return m_Address;
			}

		private:
			byte* m_Address;
		};
	}
}

#endif