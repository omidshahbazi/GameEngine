#pragma once

class Mesh
{
public:
	// Vertex Layout |Position:x,y,z|UV:u,v|
	Mesh(const float *Vertices, unsigned int VertexCount, const unsigned int *Indices, unsigned int IndexCount);

	void Draw(void);

	static Mesh *CreateQuadMesh(void);

private:
	unsigned int m_VAO;
};