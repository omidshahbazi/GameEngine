
#include <CoreSystem\Core.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Rendering\RenderingManager.h>
#include <ResourceSystem\ResourceManager.h>
#include <Containers\MathContainers.h>
#include <Rendering\Material.h>
#include <Platform\PlatformFile.h>
#include <GameObjectSystem\SceneManager.h>
#include <Utility\Window.h>


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

void main()
{
	Core *core = Core::Create(RootAllocator::GetInstance());

	Window *window = core->CreateWindow(WIDTH, HEIGHT, "Test Launcher");

	core->Initialize();

	SceneManager *sceneMgr = SceneManager::GetInstance();
	ResourceManager *resources = ResourceManager::GetInstance();

	TextureResource tex = resources->Load<Texture>("WOOD.png");
	ProgramResource shader = resources->Load<Program>("Shader.shader");
	MeshResource ringMesh = resources->Load<Mesh>("Box.obj");

	Material mat;
	Pass pass(*shader);
	IDevice::State state = pass.GetRenderState();
	state.SetPolygonMode(IDevice::PolygonModes::Fill);
	pass.SetRenderState(state);
	pass.SetTexture("tex1", *tex);
	mat.AddPass(pass);

	Scene scene = sceneMgr->CreateScene();
	sceneMgr->SetActiveScene(scene);
	for (int i = 0; i < 10000; ++i)
	{
		GameObject obj = scene.CreateRenderableGameObject();

		Renderer renderer = obj.GetRenderer();

		renderer.SetMesh(*ringMesh);
		renderer.SetMaterial(&mat);
	}

	GameObject camObj = scene.CreateCameraGameObject();
	Camera camera = camObj.GetCamera();

	camera.SetAspectRatio(ASPECT_RATIO);
	camera.SetFieldOfView(60);
	camera.SetNearClipDistance(0.1F);
	camera.SetFarClipDistance(1000);


	PlatformFile::Handle watcherHandle = PlatformFile::CreateWatcher(resources->GetAssetsPath().GetValue(), true);
	PlatformFile::WatchInfo watchInfos[1024];

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

		core->Update();
	}

	ResourceManager::Destroy();
	Core::Destroy();
	SceneManager::Destroy();
}