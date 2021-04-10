// Copyright 2013-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CPU_CONSTANT_BUFFER_H
#define CPU_CONSTANT_BUFFER_H

#include <Rendering\ConstantBuffer.h>

namespace Engine
{
	namespace Rendering
	{
		class RENDERING_API CPUConstantBuffer : public ConstantBuffer
		{
		public:
			CPUConstantBuffer(uint32 Size);
			~CPUConstantBuffer(void);

		public:
			void Lock(Access Access) override;

			void Unlock(void) override
			{
			}

			CPUConstantBuffer* Clone(void) const;

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