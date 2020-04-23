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

			virtual void DrawMesh(Mesh* Mesh, const Matrix4F& Model, Material* Material) = 0;

			virtual void DrawMesh(const Vector3F& Position, const Vector3F& Rotation, const Vector3F& Scale, Material* Material);

			INLINE void SetPivot(const Vector3F& Position)
			{
				m_PivotMat.SetTranslate(Position);
			}

		private:
			Matrix4F m_PivotMat;

			Mesh* m_QuadMesh;
		};
	}
}

#endif