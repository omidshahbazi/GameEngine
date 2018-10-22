#include "Framework.h"
#include "Shader.h"
#include "Mesh.h"
#include "GameObject.h"
#include "Camera.h"
#include "MeshRenderer.h"

class BallController : public Component
{
	void Update(void) override
	{
		GameObject *obj = GetGameObject();

		obj->SetRotation(obj->GetFramework()->GetTime() * obj->GetFramework()->GetDeltaTime() * 100);
	}
};

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
		m_Framework->SetOnUpdate(std::bind(&Game::OnUpdate, this));

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
		m_Shader = Shader::FromFile("default.vert", "default.frag");

		GameObject *cameraObject = m_Framework->CreateGameObject();
		m_Camera = new Camera;
		cameraObject->AddComponent(m_Camera);
		cameraObject->SetPosition(glm::vec3(2, 2, -10));

		m_Framework->SetActiveCamera(m_Camera);

		Mesh *mesh = Mesh::CreateCubeMesh();

		const int WIDTH = 10;

		GameObject *obj = m_Framework->CreateGameObject();
		MeshRenderer *meshRendere = new MeshRenderer;
		obj->AddComponent(meshRendere);
		meshRendere->SetShader(m_Shader);
		meshRendere->SetMesh(mesh);
		//obj->AddComponent(new BallController);
	}

	void OnUpdate(void)
	{
		if (m_Framework->IsKeyPressed(Keys::Tab))
			m_Camera->SetPolygonMode(PolygonModes::Wireframe);
	}

private:
	Framework * m_Framework;
	Shader *m_Shader;
	Camera *m_Camera;
};

void main()
{
	Game game;
}