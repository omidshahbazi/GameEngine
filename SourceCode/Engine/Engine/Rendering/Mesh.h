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
			class SubMesh
			{
			public:
				SubMesh(GPUBuffer BUffer, uint16 IndexCount) :
					m_Buffer(BUffer),
					m_IndexCount(IndexCount)
				{
				}

				INLINE const GPUBuffer &GetBuffer(void) const
				{
					return m_Buffer;
				}

				INLINE uint16 GetIndexCount(void) const
				{
					return m_IndexCount;
				}

			private:
				GPUBuffer m_Buffer;
				uint16 m_IndexCount;
			};

		public:
			Mesh(SubMesh *SubMeshes, uint16 SubMeshCount) :
				m_SubMeshes(SubMeshes),
				m_SubMeshCount(SubMeshCount)
			{
			}

			INLINE SubMesh *GetSubMeshes(void) const
			{
				return m_SubMeshes;
			}

			INLINE uint16 GetSubMeshCount(void) const
			{
				return m_SubMeshCount;
			}

		private:
			SubMesh * m_SubMeshes;
			uint16 m_SubMeshCount;
		};
	}
}

#endif