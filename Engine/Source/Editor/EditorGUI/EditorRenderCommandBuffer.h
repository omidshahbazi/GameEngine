// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef EDITOR_RENDER_COMMAND_BUFFER_H
#define EDITOR_RENDER_COMMAND_BUFFER_H

#include <MathContainers\MathContainers.h>
#include <RenderSystem\CommandBuffer.h>
#include <RenderSystem\Material.h>
#include <RenderSystem\Mesh.h>

namespace Engine
{
	using namespace MathContainers;
	using namespace RenderSystem;

	namespace EditorGUI
	{
		class EDITORGUI_API EditorRenderCommandBuffer : public CommandBuffer
		{
		public:
			EditorRenderCommandBuffer(void);

			void DrawQuad(const Vector2I& Position, float32 Rotation, const Vector2I& Scale, const Material* Material);

			void DrawMesh(Mesh* Mesh, const Matrix4F& Transform, const Material* Material);

			void SetProjectionSize(const Vector2I& Size)
			{
				//-1 To mirror the Y axis
				m_ProjMat.SetOrthographicProjection(Size.X, 0, 0, Size.Y, -1, 100);
			}

			INLINE Vector2I GetPivot(void) const
			{
				Vector3F value = m_PivotMat.GetTranslate();

				return Vector2I(value.X, value.Y);
			}

			INLINE void SetPivot(const Vector2I& Position)
			{
				m_PivotMat.SetTranslate(Vector3F(Position.X, Position.Y, 0));
			}

		private:
			Matrix4F m_ProjMat;
			Matrix4F m_PivotMat;

			Mesh* m_QuadMesh;
		};
	}
}

#endif