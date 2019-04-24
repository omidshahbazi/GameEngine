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

		public:
			typedef Map<uint64, Mesh*> MeshMap;

		public:
			Font(void);

			INLINE Mesh* GetMesh(const uint64 &CharCode)
			{
				if (m_Meshes.Contains(CharCode))
					return m_Meshes[CharCode];

				return nullptr;
			}

		private:
			MeshMap m_Meshes;
		};

		typedef ResourceSystem::ResourceHandle<Font> FontHandle;
	}
}

#endif