// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef FONT_H
#define FONT_H

#include <ResourceSystem\ResourceHandle.h>
#include <Containers\Map.h>

namespace Engine
{
	namespace Rendering
	{
		class MeshInfo;
		class Mesh;
	}
	
	using namespace MemoryManagement::Allocator;
	using namespace Containers;
	using namespace Rendering;

	namespace FontSystem
	{
		class FONTSYSTEM_API Font
		{
			friend class FontManager;

		private:
			typedef Map<uint64, MeshInfo*> MeshInfoMap;
			typedef Map<uint64, Mesh*> MeshMap;

		public:
			Font(void);

			INLINE Mesh* GetMesh(const uint64 &CharCode);

		private:
			MeshInfoMap m_MesheInfos;
			MeshMap m_Meshes;
		};

		typedef ResourceSystem::ResourceHandle<Font> FontHandle;
	}
}

#endif