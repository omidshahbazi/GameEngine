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
		class EditorRenderDeviceBase
		{
		public:
			EditorRenderDeviceBase(void) :
				m_PivotMat(Matrix4F::Identity)
			{
			}

			virtual void DrawMesh(Mesh* Mesh, const Matrix4F& Model, Material* Material) = 0;

			virtual void DrawMesh(Mesh* Mesh, const Vector3F& Position, const Vector3F& Rotation, const Vector3F& Scale, Material* Material)
			{
				Matrix4F modelMat = Matrix4F::Identity;
				modelMat.SetTranslate(Position);

				Matrix4F rotMat = Matrix4F::Identity;
				QuaternionF rot = QuaternionF::FromEuler(Rotation);
				rot.ToMatrix(rotMat);

				modelMat *= rotMat;

				Matrix4F scaleMat = Matrix4F::Identity;
				scaleMat.SetScale(Scale);

				modelMat *= scaleMat;

				modelMat = m_PivotMat * modelMat;

				DrawMesh(Mesh, modelMat, Material);
			}

			void SetPivot(const Vector3F& Position)
			{
				m_PivotMat.SetTranslate(Position);
			}

		private:
			Matrix4F m_PivotMat;
		};
	}
}

#endif