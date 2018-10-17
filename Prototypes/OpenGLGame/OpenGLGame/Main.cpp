
#include "Framework.h"
#include "Shader.h"
#include "Mesh.h"

#include "glm\matrix.hpp"
#include "glm\gtc\matrix_transform.hpp"

class Game
{
private:
	const unsigned int WINDOW_WIDTH = 800;
	const unsigned int WINDOW_HEIGHT = 600;
	const Color CLEAR_COLOR = { 0, 0, 0, 0 };

public:
	Game(void) :
		m_Framework(new Framework)
	{
		m_Framework->SetOnInitialize(std::bind(&Game::OnInitialize, this));
		m_Framework->SetOnDeinitialize(std::bind(&Game::OnDeinitialize, this));
		m_Framework->SetOnUpdate(std::bind(&Game::OnUpdate, this, std::placeholders::_1));
		m_Framework->SetOnRender(std::bind(&Game::OnRender, this, std::placeholders::_1));
		m_Framework->SetOnDeviceResized(std::bind(&Game::OnDeviceResized, this, std::placeholders::_1, std::placeholders::_2));

		m_Framework->Initialize();

		m_Framework->CreateWindow("OpenGL Game", WINDOW_WIDTH, WINDOW_HEIGHT);

		m_Framework->Run();
	}

	~Game(void)
	{
		m_Framework->Deinitialize();

		delete m_Framework;
	}

	void OnInitialize(void)
	{
		m_Framework->SetClearColor(CLEAR_COLOR);

		m_Shader = Shader::CreateDefaultShader();
		m_Mesh = Mesh::CreateQuadMesh();
	}

	void OnDeinitialize(void)
	{

	}

	void OnUpdate(float Time)
	{

	}

	glm::mat4 projectionMat;

	void OnRender(float Time)
	{
		glm::mat4 modelMat(1.0F);
		modelMat = glm::translate(modelMat, glm::vec3(100.0F, 100.0F, 0.0F));
		modelMat = glm::scale(modelMat, glm::vec3(100.0F, 100.0F, 1.0F));
		modelMat = glm::rotate(modelMat, glm::radians(45.0F), glm::vec3(0, 0, 1));;

		glm::mat4 viewMat(1.0F);
		viewMat = glm::translate(viewMat, glm::vec3(100.0F, 0.0F, 0.0F));

		glm::mat4 mvpMat = projectionMat * viewMat * modelMat;

		m_Shader->SetMatrix("MVP", mvpMat);

		m_Mesh->Draw();
	}

	void OnDeviceResized(int Width, int Height)
	{
		projectionMat = glm::ortho(0.0F, (float)Width, (float)Height, (float)0, -1.0F, 10.0F);
	}

private:
	Framework *m_Framework;

	Shader *m_Shader;
	Mesh *m_Mesh;
};

void main()
{
	Game game;
}