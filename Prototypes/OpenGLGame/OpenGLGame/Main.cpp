//Material


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
		cameraObject->AddComponent(new Camera);

		GenerateBlocks();
	}

	void OnUpdate(void)
	{
		if (m_Framework->IsKeyPressed(Keys::Tab))
			m_Framework->SetWireframeEnabled(!m_Framework->GetWireframeEnabled());
	}

private:
	void GenerateBlocks(void)
	{
		Mesh *mesh = Mesh::CreateQuadMesh();

		const int SPACE = 5;
		const int BLOCK_COLUMN_COUNT = 20;
		const int BLOCK_ROW_COUNT = 10;
		const int RIGHT_OFFSET = 100;
		const int TOP_OFFSET = 100;
		const int BLOCK_WIDTH = (m_Framework->GetDeviceWidth() - (RIGHT_OFFSET * 2) - ((BLOCK_COLUMN_COUNT - 1) * SPACE)) / BLOCK_COLUMN_COUNT;
		const int BLOCK_HEIGHT = (m_Framework->GetDeviceWidth() - (TOP_OFFSET * 6) - ((BLOCK_ROW_COUNT - 1) * SPACE)) / BLOCK_ROW_COUNT;

		for (int i = 0; i < BLOCK_COLUMN_COUNT; ++i)
			for (int j = 0; j < BLOCK_ROW_COUNT; ++j)
			{
				GameObject *blockObj = m_Framework->CreateGameObject();
				blockObj->SetPosition(glm::vec3(RIGHT_OFFSET + (BLOCK_WIDTH / 2) + ((BLOCK_WIDTH + SPACE) * i), TOP_OFFSET + (BLOCK_HEIGHT / 2) + ((BLOCK_HEIGHT + SPACE) * j), 0));
				blockObj->SetScale(glm::vec3(BLOCK_WIDTH, BLOCK_HEIGHT, 0));

				MeshRenderer *meshRendere = new MeshRenderer;
				blockObj->AddComponent(meshRendere);

				meshRendere->SetShader(m_Shader);
				meshRendere->SetMesh(mesh);
			}
	}

private:
	Framework * m_Framework;
	Shader *m_Shader;
};

void main()
{
	Game game;
}