// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\EditorRenderDeviceBase.h>
#include <EditorGUI\Private\Resources.h>

namespace Engine
{
	using namespace ResourceSystem;

	namespace EditorGUI
	{
		using namespace Private;

		EditorRenderDeviceBase::EditorRenderDeviceBase(void) :
			m_PivotMat(Matrix4F::Identity)
		{
			m_QuadMesh = Resources::GetQuadMesh();
		}

		void EditorRenderDeviceBase::DrawMesh(const Vector3F& Position, const Vector3F& Rotation, const Vector3F& Scale, const Material* Material)
		{
			Vector3F pos = Position;
			pos.X += Scale.X / 2.0F;
			pos.Y += Scale.Y / 2.0F;
			pos.Y += 5;

			Matrix4F modelMat = Matrix4F::Identity;
			modelMat.SetTranslate(pos);

			Matrix4F rotMat = Matrix4F::Identity;
			QuaternionF rot = QuaternionF::FromEuler(Rotation);
			rot.ToMatrix(rotMat);

			modelMat *= rotMat;

			Matrix4F scaleMat = Matrix4F::Identity;
			scaleMat.SetScale(Scale);

			modelMat *= scaleMat;

			modelMat = m_PivotMat * modelMat;

			DrawMesh(m_QuadMesh, modelMat, Material);
		}
	}
}