#include "MeshRenderer.h"
#include "GameObject.h"
#include "Shader.h"
#include "Mesh.h"

void MeshRenderer::Render(void)
{
	if (m_Shader != nullptr)
	{
		m_Shader->Use();
		m_Shader->SetMatrix("MVP", GetGameObject()->GetWorldTransformMatrix());
	}

	if (m_Mesh != nullptr)
		m_Mesh->Draw();
}