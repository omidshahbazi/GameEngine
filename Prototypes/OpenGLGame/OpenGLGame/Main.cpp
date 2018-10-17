
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
		m_Framework->SetOnDeinitialize(std::bind(&Game::OnDeinitialize, this));
		m_Framework->SetOnUpdate(std::bind(&Game::OnUpdate, this));
		m_Framework->SetOnRender(std::bind(&Game::OnRender, this));
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

		GameObject *cameraObject = m_Framework->CreateGameObject();
		cameraObject->AddComponent(new Camera);

		GameObject *quadObject = m_Framework->CreateGameObject();
		MeshRenderer *meshRendere = new MeshRenderer;
		meshRendere->SetShader(Shader::CreateDefaultShader());
		meshRendere->SetMesh(Mesh::CreateQuadMesh());
		quadObject->AddComponent(meshRendere);

		quadObject->SetScale(glm::vec3(100, 100, 0));
	}

	void OnDeinitialize(void)
	{

	}

	void OnUpdate(void)
	{

	}


	void OnRender(void)
	{
		//glm::mat4 vpMat = m_ProjectionMatrix * viewMat;



	}

	void OnDeviceResized(int Width, int Height)
	{
	}

private:
	Framework *m_Framework;
};

void main()
{
	Game game;
}