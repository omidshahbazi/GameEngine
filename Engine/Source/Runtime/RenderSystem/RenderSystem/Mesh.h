// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef MESH_H
#define MESH_H

#include <RenderSystem\SubMesh.h>

namespace Engine
{
	namespace RenderSystem
	{
		class Mesh
		{
		public:
			Mesh(SubMesh* SubMeshes, uint16 SubMeshCount) :
				m_SubMeshes(SubMeshes),
				m_SubMeshCount(SubMeshCount)
			{
			}

			void SetName(const WString& Name)
			{
				for (uint16 i = 0; i < m_SubMeshCount; ++i)
					m_SubMeshes[i].SetName(Name);
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
	}
}

#endif