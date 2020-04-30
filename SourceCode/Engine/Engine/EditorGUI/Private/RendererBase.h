// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDERER_BASE_H
#define RENDERER_BASE_H

#include <Rendering\Material.h>
#include <MathContainers\MathContainers.h>

namespace Engine
{
	using namespace Rendering;
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

				virtual void Render(EditorRenderDeviceBase* Device, const Vector2I& Position) const = 0;

			protected:
				void SetMaterial(const Material& Material);
				const Material* GetMaterial(void) const
				{
					return &m_Material;
				}

				Pass& GetPass(void) const
				{
					return *m_Pass;
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

			private:
				Material m_Material;
				Pass* m_Pass;

				ColorUI8 m_Color;
				Vector2I m_Dimension;
			};
		}
	}
}

#endif