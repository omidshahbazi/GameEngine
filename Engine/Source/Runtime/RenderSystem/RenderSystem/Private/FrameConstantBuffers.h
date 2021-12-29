// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef FRAME_CONSTANT_BUFFERS_H
#define FRAME_CONSTANT_BUFFERS_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace RenderSystem
	{
		namespace Private
		{
			class GPUConstantBuffer;

			class FrameConstantBuffers
			{
			public:
				FrameConstantBuffers(void);
				~FrameConstantBuffers(void);

				void Reset(void);

				GPUConstantBuffer* Get(uint16 Size);

			private:
				GPUConstantBuffer* m_Buffers;
				uint16 m_InitializedCap;
				uint16 m_BufferIndex;
			};
		}
	}
}

#endif