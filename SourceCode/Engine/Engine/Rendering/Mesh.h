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
			Mesh(GPUBuffer Vertices, GPUBuffer UVs) :
				m_Vertices(Vertices),
				m_UVs(UVs)
			{
			}

			INLINE const GPUBuffer &GetVertices(void) const
			{
				return m_Vertices;
			}

			INLINE const GPUBuffer &GetUVs(void) const
			{
				return m_UVs;
			}

		private:
			GPUBuffer m_Vertices;
			GPUBuffer m_UVs;
		};
	}
}

#endif