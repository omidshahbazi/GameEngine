
#include <CoreSystem\Core.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Rendering\RenderingManager.h>
#include <ResourceSystem\ResourceManager.h>
#include <Containers\MathContainers.h>
#include <Rendering\Material.h>
#include <Platform\PlatformFile.h>
#include <GameObjectSystem\SceneManager.h>
#include <Utility\Window.h>
#include <Utility\FileSystem.h>


using namespace Engine::Common;
using namespace Engine::Containers;
using namespace Engine::CoreSystem;
using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::Rendering;
using namespace Engine::ResourceSystem;
using namespace Engine::Platform;
using namespace Engine::GameObjectSystem;
using namespace Engine::Utility;

const int WIDTH = 1024;
const int HEIGHT = 768;
const float ASPECT_RATIO = (float)WIDTH / HEIGHT;

class WindowListener : public Window::IListener
{
public:
	WindowListener(Camera Camera) :
		m_Camera(Camera)
	{
	}

private:
	void OnWindowResized(Window *Window) override
	{
		m_Camera.SetAspectRatio(Window->GetSize().X / (float)Window->GetSize().Y);
	}

private:
	Camera m_Camera;
};

void main()
{
	FileSystem::SetWorkingPath(L"D:\\Projects\\GameEngineAssets");

	Core *core = Core::Create(RootAllocator::GetInstance());

	Window *window = core->CreateWindow({ WIDTH, HEIGHT }, "Test Launcher");

	core->Initialize();

	SceneManager *sceneMgr = SceneManager::GetInstance();
	ResourceManager *resources = ResourceManager::GetInstance();

	TextureResource tex = resources->Load<Texture>("WOOD.png");
	ProgramResource shader = resources->Load<Program>("Shader.shader");
	MeshResource ringMesh = resources->Load<Mesh>("Sphere.obj");

	Material mat;
	mat.SetQueue(RenderQueues::Geometry);
	Pass pass(*shader);
	pass.SetTexture("tex1", *tex);
	mat.AddPass(pass);

	Scene scene = sceneMgr->CreateScene();
	sceneMgr->SetActiveScene(scene);

	const int32 GAME_OBJECT_COUNT_X = 10;
	const int32 GAME_OBJECT_COUNT_Y = 10;
	GameObject gameObjects[GAME_OBJECT_COUNT_X * GAME_OBJECT_COUNT_Y];

	for (int i = 0; i < GAME_OBJECT_COUNT_X; ++i)
		for (int j = 0; j < GAME_OBJECT_COUNT_Y; ++j)
		{
			GameObject obj = gameObjects[i + (j * GAME_OBJECT_COUNT_X)] = scene.CreateRenderableGameObject();

			Renderer renderer = obj.GetRenderer();

			renderer.SetMesh(*ringMesh);
			renderer.SetMaterial(&mat);

			Transform tr = obj.GetTransform();

			tr.SetPosition(Vector3F((GAME_OBJECT_COUNT_X / -2.0F) + (i * 2), 0, j * -2));
			tr.SetRotation(Vector3F(rand() % 90, rand() % 90, rand() % 90));
		}

	GameObject camObj = scene.CreateCameraGameObject();
	Camera camera = camObj.GetCamera();
	camObj.GetTransform().SetPosition({ 0, -5, -15 });

	camera.SetAspectRatio(ASPECT_RATIO);
	camera.SetFieldOfView(60);
	camera.SetNearClipDistance(0.1F);
	camera.SetFarClipDistance(1000);

	GameObject amLightObj = scene.CreateLightingGameObject();
	Light amLight = amLightObj.GetLight();
	amLight.SetType(LightTypes::Ambient);
	amLight.SetStrength(0.5);
	amLight.SetColor({ 100, 100, 100 });

	GameObject dirLightObj1 = scene.CreateLightingGameObject();
	{
		Light dirLight = dirLightObj1.GetLight();
		dirLight.SetType(LightTypes::Directional);
		dirLight.SetColor({ 255, 0, 0 });
		dirLight.SetStrength(2);
		dirLightObj1.GetTransform().SetRotation({ 90, 0, 0 });
	}

	GameObject dirLightObj2 = scene.CreateLightingGameObject();
	{
		Light dirLight = dirLightObj2.GetLight();
		dirLight.SetType(LightTypes::Directional);
		dirLight.SetColor({ 128, 0, 128 });
		dirLight.SetStrength(2);
		dirLightObj2.GetTransform().SetRotation({ 45, 45, 0 });
	}


	WindowListener windowListener(camera);
	window->AddListener(&windowListener);


	PlatformFile::Handle watcherHandle = PlatformFile::CreateWatcher(resources->GetAssetsPath().GetValue(), true);
	PlatformFile::WatchInfo watchInfos[1024];

	int32 rot = 0;

	while (!window->ShouldClose())
	{
		uint32 len;
		PlatformFile::RefreshWatcher(watcherHandle, true, PlatformFile::WatchNotifyFilter::FileRenamed | PlatformFile::WatchNotifyFilter::DirectoryRenamed | PlatformFile::WatchNotifyFilter::LastWriteTimeChanged, watchInfos, 1024, len);

		if (len > 0)
		{
			WStringList files;

			for (uint32 i = 0; i < len; ++i)
			{
				PlatformFile::WatchInfo &info = watchInfos[i];

				if (info.Action != PlatformFile::WatchAction::Modified)
					continue;

				WString file(info.FileName, info.FileNameLength);

				if (!files.Contains(file))
					files.Add(file);
			}

			for each (auto &file in files)
				resources->Reload(file);
		}

		rot += 2;

		for (int i = 0; i < GAME_OBJECT_COUNT_X * GAME_OBJECT_COUNT_Y; ++i)
		{
			Transform tr = gameObjects[i].GetTransform();

			//tr.SetRotation(Vector3F(rot, rot, rot));
		}

		core->Update();
	}

	ResourceManager::Destroy();
	Core::Destroy();
	SceneManager::Destroy();
}