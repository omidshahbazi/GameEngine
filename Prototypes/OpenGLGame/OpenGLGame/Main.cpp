
#include "Framework.h"
#include "Shader.h"
#include "Mesh.h"

const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;
const Color CLEAR_COLOR = { 0, 0, 0, 0 };

void main()
{
	Framework framework;
	framework.Initialize();
	framework.CreateWindow("OpenGL Game", WINDOW_WIDTH, WINDOW_HEIGHT);

	framework.SetClearColor(CLEAR_COLOR);

	Shader *shader = Shader::CreateDefaultShader();
	Mesh *mesh = Mesh::CreateQuadMesh();

	framework.Run();

	framework.Deinitialize();
}