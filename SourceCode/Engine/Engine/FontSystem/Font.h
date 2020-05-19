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
		class Mesh;
		class Texture;

		typedef ResourceSystem::ResourceHandle<Texture> TextureHandle;
	}

	using namespace MemoryManagement::Allocator;
	using namespace Containers;
	using namespace MathContainers;
	using namespace Rendering;

	namespace FontSystem
	{
		class FontManager;

		class FONTSYSTEM_API Font
		{
			friend class FontManager;

		public:
			enum class RenderTypes
			{
				Mesh = 0,
				Texture = 1
			};

			class Character
			{
				friend class FontManager;

			public:
				Character(void) :
					m_Mesh(nullptr)
				{
				}

				Character(const Character& Other)
				{
					*this = Other;
				}

				Character& operator = (const Character& Other)
				{
					m_Size = Other.m_Size;
					m_Bearing = Other.m_Bearing;
					m_Advance = Other.m_Advance;

					m_Mesh = Other.m_Mesh;

					if (m_Mesh == nullptr)
					{
						m_Texture = Other.m_Texture;
						m_Bounds = Other.m_Bounds;
					}

					return *this;
				}

			private:
				INLINE void SetMesh(Mesh* Mesh)
				{
					m_Mesh = Mesh;
				}

				INLINE void SetTexture(Texture* Texture)
				{
					m_Texture = TextureHandle(Texture);
				}

			public:
				INLINE const Vector2F& GetSize(void) const
				{
					return m_Size;
				}

				INLINE const Vector2F& GetBearing(void) const
				{
					return m_Bearing;
				}

				INLINE const Vector2F& GetAdvance(void) const
				{
					return m_Advance;
				}

				INLINE Mesh* GetMesh(void)
				{
					return m_Mesh;
				}

				INLINE TextureHandle* GetTexture(void)
				{
					return &m_Texture;
				}

			private:
				Vector2F m_Size;
				Vector2F m_Bearing;
				Vector2F m_Advance;

				union
				{
					Mesh* m_Mesh;

					struct
					{
						TextureHandle m_Texture;
						Vector4F m_Bounds;
					};
				};
			};

		private:
			typedef Map<uint64, Character> InitialCharacterhMap;

		public:
			Font(void);
			//TODO: Free font resources

			Font::Character* GetCharacter(const uint64& CharCode);

			INLINE RenderTypes GetRenderType(void) const
			{
				return m_RenderType;
			}

		private:
			RenderTypes m_RenderType;
			InitialCharacterhMap m_Characters;
		};

		typedef ResourceSystem::ResourceHandle<Font> FontHandle;
	}
}

#endif