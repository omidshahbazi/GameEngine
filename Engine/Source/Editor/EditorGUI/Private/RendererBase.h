// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDERER_BASE_H
#define RENDERER_BASE_H

#include <RenderSystem\Material.h>
#include <MathContainers\MathContainers.h>

namespace Engine
{
	using namespace RenderSystem;
	using namespace MathContainers;

	namespace EditorGUI
	{
		class EditorRenderDeviceBase;

		namespace Private
		{
			class RendererBase
			{
			public:
				RendererBase(void);
				virtual ~RendererBase(void)
				{
				}

				virtual void Update(void)
				{
				}

				virtual void Render(EditorRenderDeviceBase* Device, const Vector2I& Position) const = 0;

			protected:
				void SetMaterial(Material* Material);
				Material& GetMaterial(void)
				{
					return m_Material;
				}

				const Material& GetMaterial(void) const
				{
					return m_Material;
				}

			public:
				const ColorUI8& GetColor(void) const
				{
					return m_Color;
				}
				virtual void SetColor(const ColorUI8& Value)
				{
					m_Color = Value;
				}

				const Vector2I& GetDimension(void) const
				{
					return m_Dimension;
				}
				virtual void SetDimension(const Vector2I& Value)
				{
					m_Dimension = Value;
				}

				float32 GetRotation(void) const
				{
					return m_Rotation;
				}
				virtual void SetRotation(float32 Value)
				{
					m_Rotation = Value;
				}

			private:
				Material m_Material;

				ColorUI8 m_Color;
				Vector2I m_Dimension;
				float32 m_Rotation;
			};
		}
	}
}

#endif