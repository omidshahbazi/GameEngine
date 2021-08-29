// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef EDITOR_RENDER_DEVICE_BASE_H
#define EDITOR_RENDER_DEVICE_BASE_H

#include <MathContainers\MathContainers.h>
#include <Rendering\Material.h>
#include <Rendering\Mesh.h>

namespace Engine
{
	using namespace MathContainers;
	using namespace Rendering;

	namespace EditorGUI
	{
		class EDITORGUI_API EditorRenderDeviceBase
		{
		public:
			EditorRenderDeviceBase(void);

		public:
			virtual void DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Material* Material);

			virtual void DrawQuad(const Vector2I& Position, float32 Rotation, const Vector2I& Scale, const Material* Material);

			virtual void SetProjectionSize(const Vector2I& Size)
			{
				//To mirror the Y axis
				m_ProjMat.SetOrthographicProjection(Size.X, 0, 0, Size.Y, -1, 100);
			}

			const Matrix4F& GetProjectionMatrix(void) const
			{
				return m_ProjMat;
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

		protected:
			virtual void Render(Mesh* Mesh, const Matrix4F& Model, const Material* Material) = 0;

		private:
			Matrix4F m_ProjMat;
			Matrix4F m_PivotMat;

			Mesh* m_QuadMesh;
		};
	}
}

#endif