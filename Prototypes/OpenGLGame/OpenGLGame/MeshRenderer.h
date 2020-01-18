#pragma once

#include "Component.h"

class Shader;
class Mesh;

class MeshRenderer : public Component
{
public:
	MeshRenderer(void) :
		m_Mesh(nullptr)
	{
	}

	void Render(void) override;

	void SetShader(Shader *Shader)
	{
		m_Shader = Shader;
	}

	void SetMesh(Mesh *Mesh)
	{
		m_Mesh = Mesh;
	}

private:
	Shader *m_Shader;
	Mesh *m_Mesh;
};