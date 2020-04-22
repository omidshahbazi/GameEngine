// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef MESH_H
#define MESH_H

#include <Rendering\SubMesh.h>

namespace Engine
{
	namespace Rendering
	{
		class Mesh
		{
		public:
			Mesh(SubMesh* SubMeshes, uint16 SubMeshCount) :
				m_SubMeshes(SubMeshes),
				m_SubMeshCount(SubMeshCount)
			{
			}

			INLINE SubMesh* GetSubMeshes(void) const
			{
				return m_SubMeshes;
			}

			INLINE uint16 GetSubMeshCount(void) const
			{
				return m_SubMeshCount;
			}

		private:
			SubMesh* m_SubMeshes;
			uint16 m_SubMeshCount;
		};

		typedef ResourceSystem::ResourceHandle<Mesh> MeshHandle;
	}
}

#endif