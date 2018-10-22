// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef MESH_H
#define MESH_H

#include <Rendering\GPUBuffer.h>

namespace Engine
{
	namespace Rendering
	{
		class Mesh
		{
		public:
			Mesh(GPUBuffer BUffer) :
				m_Buffer(BUffer)
			{
			}

			INLINE const GPUBuffer &GetBuffer(void) const
			{
				return m_Buffer;
			}

		private:
			GPUBuffer m_Buffer;
		};
	}
}

#endif