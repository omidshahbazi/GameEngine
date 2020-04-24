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

			virtual void DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Material* Material) = 0;

			virtual void DrawMesh(const Vector2I& Position, float32 Rotation, const Vector2I& Scale, const Material* Material);

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
			Matrix4F m_PivotMat;

			Mesh* m_QuadMesh;
		};
	}
}

#endif