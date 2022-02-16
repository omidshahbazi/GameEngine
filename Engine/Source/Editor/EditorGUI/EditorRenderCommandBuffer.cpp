// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\EditorRenderCommandBuffer.h>
#include <EditorGUI\Private\Resources.h>
#include <EditorGUI\Private\EditorGUIAllocators.h>

namespace Engine
{
	using namespace ResourceSystem;

	namespace EditorGUI
	{
		using namespace Private;

		EditorRenderCommandBuffer::EditorRenderCommandBuffer(void) :
			CommandBuffer("EditorRenderCommandBuffer"),
			m_ProjMat(Matrix4F::Identity),
			m_PivotMat(Matrix4F::Identity),
			m_CopiedMaterials(EditorGUIAllocators::TypesAllocator, 4096)
		{
			m_QuadMesh = Resources::GetInstance()->GetQuadMesh();
		}

		void EditorRenderCommandBuffer::Clear(void)
		{
			CommandBuffer::Clear();

			m_CopiedMaterials.Clear();
		}

		void EditorRenderCommandBuffer::DrawText(Mesh* Mesh, const Matrix4F& Model, const Material& Material)
		{
			m_CopiedMaterials.Add({});
			auto& material = m_CopiedMaterials[m_CopiedMaterials.GetSize() - 1];
			material = Material;

			DrawMesh(Mesh, Model, &material);
		}

		void EditorRenderCommandBuffer::DrawQuad(const Vector2I& Position, float32 DegreesRotation, const Vector2I& Scale, const Material* Material)
		{
			Vector3F pos(Position.X, Position.Y, 0);
			pos.X += Scale.X / 2.0F;
			pos.Y += Scale.Y / 2.0F;

			Matrix4F modelMat = Matrix4F::Identity;
			modelMat.SetTranslate(pos);

			Matrix4F rotMat = Matrix4F::Identity;
			QuaternionF rot = QuaternionF::FromEuler(Vector3F(0, 0, DegreesRotation * Math::DEGREES_TO_RADIANS));
			rot.ToMatrix(rotMat);

			modelMat *= rotMat;

			Matrix4F scaleMat = Matrix4F::Identity;
			scaleMat.SetScale(Vector3F(Scale.X, Scale.Y, 0));

			modelMat *= scaleMat;

			DrawMesh(m_QuadMesh, modelMat, Material);
		}

		void EditorRenderCommandBuffer::DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Material* Material)
		{
			Matrix4F modelMat = m_PivotMat * Model;

			CommandBuffer::DrawMesh(Mesh, modelMat, Matrix4F::Identity, m_ProjMat, Material);
		}
	}
}