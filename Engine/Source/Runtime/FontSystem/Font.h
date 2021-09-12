// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef FONT_H
#define FONT_H

#include <ResourceCommon\Resource.h>
#include <Containers\Map.h>
#include <MathContainers\MathContainers.h>
#include <FontSystem\FontInfo.h>

namespace Engine
{
	namespace RenderSystem
	{
		class Mesh;
		class Texture;

		typedef ResourceCommon::Resource<Texture> TextureHandle;
	}

	using namespace MemoryManagement::Allocator;
	using namespace Containers;
	using namespace MathContainers;
	using namespace RenderSystem;

	namespace FontSystem
	{
		class FontManager;

		class FONTSYSTEM_API Font
		{
			friend class FontManager;

		public:
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

				INLINE const Vector4F& GetBounds(void) const
				{
					return m_Bounds;
				}

				INLINE Mesh* GetMesh(void) const
				{
					return m_Mesh;
				}

				INLINE const TextureHandle* GetTexture(void) const
				{
					return &m_Texture;
				}

			private:
				Vector2F m_Size;
				Vector2F m_Bearing;
				Vector2F m_Advance;

				Mesh* m_Mesh;

				TextureHandle m_Texture;
				Vector4F m_Bounds;
			};

		private:
			typedef Map<uint64, Character> InitialCharacterhMap;

		public:
			Font(void);
			~Font(void);

			void SetName(const WString& Name)
			{
			}

			Font::Character* GetCharacter(const uint64& CharCode);

			INLINE float32 GetSize(void) const
			{
				return m_Size;
			}

			INLINE FontRenderTypes GetRenderType(void) const
			{
				return m_RenderType;
			}

		private:
			float32 m_Size;
			FontRenderTypes m_RenderType;
			Texture* m_Texture;
			InitialCharacterhMap m_Characters;
		};
	}
}

#endif