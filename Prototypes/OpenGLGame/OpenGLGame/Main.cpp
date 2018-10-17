
#include "Framework.h"
#include "Shader.h"
#include "Mesh.h"
#include "GameObject.h"
#include "Camera.h"
#include "MeshRenderer.h"

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

		GameObject *cameraObject = m_Framework->CreateGameObject();
		cameraObject->AddComponent(new Camera);

		GameObject *quadObject = m_Framework->CreateGameObject();
		MeshRenderer *meshRendere = new MeshRenderer;
		meshRendere->SetShader(Shader::CreateDefaultShader());
		meshRendere->SetMesh(Mesh::CreateQuadMesh());
		quadObject->AddComponent(meshRendere);

		quadObject->SetPosition(glm::vec3(100, 100, 0));
		quadObject->SetScale(glm::vec3(100, 100, 0));
		quadObject->SetRotation(78);

		quadObject = m_Framework->CreateGameObject();
		meshRendere = new MeshRenderer;
		meshRendere->SetShader(Shader::CreateDefaultShader());
		meshRendere->SetMesh(Mesh::CreateQuadMesh());
		quadObject->AddComponent(meshRendere);
		quadObject->SetScale(glm::vec3(100, 100, 0));
	}

private:
	Framework *m_Framework;
};

void main()
{
	Game game;
}