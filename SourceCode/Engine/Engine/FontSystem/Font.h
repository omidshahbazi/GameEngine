// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef FONT_H
#define FONT_H

#include <ResourceSystem\ResourceHandle.h>
#include <Containers\Map.h>
#include <MathContainers\MathContainers.h>

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

		public:
			class Character
			{
				friend class Font;

			public:
				Character(void) :
					m_MeshInfo(nullptr),
					m_Mesh(nullptr)
				{
				}

				Character(MeshInfo *MeshInfo, Vector2F Size, Vector2F Bearing, Vector2F Advance) :
					m_MeshInfo(MeshInfo),
					m_Mesh(nullptr),
					m_Size(Size),
					m_Bearing(Bearing),
					m_Advance(Advance)
				{
				}

			private:
				INLINE void SetMesh(Mesh *Mesh)
				{
					m_Mesh = Mesh;
				}

			public:
				INLINE MeshInfo *GetMeshInfo(void)
				{
					return m_MeshInfo;
				}

				INLINE Mesh *GetMesh(void)
				{
					return m_Mesh;
				}

				INLINE const Vector2F &GetSize(void) const
				{
					return m_Size;
				}

				INLINE const Vector2F &GetBearing(void) const
				{
					return m_Bearing;
				}

				INLINE const Vector2F &GetAdvance(void) const
				{
					return m_Advance;
				}

			private:
				MeshInfo * m_MeshInfo;
				Mesh * m_Mesh;
				Vector2F m_Size;
				Vector2F m_Bearing;
				Vector2F m_Advance;
			};

		private:
			typedef Map<uint64, Character> InitialCharacterhMap;
			typedef Map<uint64, Character*> ReadyCharacterInfoMap;

		public:
			Font(void);

			INLINE Font::Character *GetCharacter(const uint64 &CharCode);

		private:
			InitialCharacterhMap m_InitialCharacters;
			ReadyCharacterInfoMap m_ReadyCharacter;
		};

		typedef ResourceSystem::ResourceHandle<Font> FontHandle;
	}
}

#endif