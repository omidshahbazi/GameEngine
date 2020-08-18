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
			m_ProjMat(Matrix4F::Identity),
			m_PivotMat(Matrix4F::Identity)
		{
			m_QuadMesh = Resources::GetQuadMesh();
		}

		void EditorRenderDeviceBase::DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Material* Material)
		{
			Matrix4F modelMat = m_PivotMat * Model;

			Render(Mesh, modelMat, Material);
		}

		void EditorRenderDeviceBase::DrawQuad(const Vector2I& Position, float32 DegreesRotation, const Vector2I& Scale, const Material* Material)
		{
			Vector3F pos(Position.X, Position.Y, 0);
			pos.X += Scale.X / 2.0F;
			pos.Y += Scale.Y / 2.0F;

			Matrix4F modelMat = Matrix4F::Identity;
			modelMat.SetTranslate(pos);

			Matrix4F rotMat = Matrix4F::Identity;
			QuaternionF rot = QuaternionF::FromEuler(Vector3F(0, 0, DegreesRotation * Mathematics::DEGREES_TO_RADIANS));
			rot.ToMatrix(rotMat);

			modelMat *= rotMat;

			Matrix4F scaleMat = Matrix4F::Identity;
			scaleMat.SetScale(Vector3F(Scale.X, -Scale.Y, 0));

			modelMat *= scaleMat;

			modelMat = m_PivotMat * modelMat;

			Render(m_QuadMesh, modelMat, Material);
		}
	}
}