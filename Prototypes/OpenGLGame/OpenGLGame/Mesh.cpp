
#include <GL\glew.h>
#include "Mesh.h"

Mesh::Mesh(const float *Vertices, unsigned int VertexCount, const unsigned int *Indices, unsigned int IndexCount)
{
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, VertexCount * sizeof(float), Vertices, GL_STATIC_DRAW);

	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexCount * sizeof(float), Indices, GL_STATIC_DRAW);

	unsigned int positionElementIndex = 0;
	unsigned int uvElementIndex = 1;
	unsigned int positionElementCount = 3;
	unsigned int uvElementCount = 2;
	unsigned int positionElementSize = positionElementCount * sizeof(float);
	unsigned int uvElementSize = uvElementCount * sizeof(float);
	unsigned int vertexElementSize = positionElementSize + uvElementSize;

	glVertexAttribPointer(positionElementIndex, positionElementCount, GL_FLOAT, false, vertexElementSize, (void*)0);
	glEnableVertexAttribArray(positionElementIndex);

	glVertexAttribPointer(uvElementIndex, uvElementCount, GL_FLOAT, false, vertexElementSize, (void*)positionElementSize);
	glEnableVertexAttribArray(uvElementIndex);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//int nrAttributes;
	//glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
}

void Mesh::Draw(void)
{
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

Mesh *Mesh::CreateQuadMesh(void)
{
	static float vertices[] =
	{
		-0.5F,	-0.5F,	0.0F,	0.0F,	0.0F,
		-0.5F,	0.5F,	0.0F,	0.0F,	1.0F,
		0.5F,	0.5F,	0.0F,	1.0F,	1.0F,
		0.5F,	-0.5F,	0.0F,	1.0F,	0.0F
	};

	static unsigned int indices[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	static Mesh mesh(vertices, sizeof(vertices) / sizeof(float), indices, sizeof(indices) / sizeof(float));

	return &mesh;
}