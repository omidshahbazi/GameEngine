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
			virtual void DrawMesh(Mesh* Mesh, const Matrix4F& Model, Material* Material) = 0;

			virtual void DrawMesh(Mesh* Mesh, const Vector3F& Position, const Vector3F& Rotation, const Vector3F& Scale, Material* Material)
			{
				Matrix4F ModelMat = Matrix4F::Identity;
				ModelMat.SetTranslate(Position);

				Matrix4F rotMat = Matrix4F::Identity;
				QuaternionF rot = QuaternionF::FromEuler(Rotation);
				rot.ToMatrix(rotMat);

				ModelMat *= rotMat;

				Matrix4F scaleMat = Matrix4F::Identity;
				scaleMat.SetScale(Scale);

				ModelMat *= scaleMat;

				DrawMesh(Mesh, ModelMat, Material);
			}
		};
	}
}

#endif