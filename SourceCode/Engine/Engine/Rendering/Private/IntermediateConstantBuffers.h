// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef INTERMEDIATE_CONSTANT_BUFFERS_H
#define INTERMEDIATE_CONSTANT_BUFFERS_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace Rendering
	{
		class ConstantBuffer;

		namespace Private
		{
			class IntermediateConstantBuffers
			{
			public:
				IntermediateConstantBuffers(void);
				~IntermediateConstantBuffers(void);

				void Reset(void);

				ConstantBuffer* Get(uint16 Size);

			private:
				ConstantBuffer* m_Buffers;
				uint16 m_InitializedCap;
				uint16 m_BufferIndex;
			};
		}
	}
}

#endif