# Game Engine

This is an experimental open-source game engine. It wrote from scratch purpose to be cross-platform.

## Project Showcase

Here is as small shots:

[![Deferred Shading](https://github.com/omidshahbazi/omidshahbazi.github.io/tree/master/Materials/GameEngine/DeferredRendering.jpg)]

## Features

* **Build System**
* **Container**
* **Memory Management**
* **Reflection**
* **Parallelizing**
* **Font System**
* **Game Object System**
    * Built-in components: model, sound, animation, camera, collision, light, rigidbody, script, particlesystem
* **Input System**
    * Mouse, Keyboard
* **Resource System**
    * Simple and powerful resource loading
    * Streaming of individual assets
    * Asset Variants - loads compressed textures (DXT, PVR, ETC1, ETC2) based on platform support
* **Rendering**
    * WebGL 2.0 based renderer (with fallback to WebGL 1.0)
    * Physically based rendering (PBR)
    * Directional, point and spot lights
    * Shadow mapping (PCF and VSM implementations)
    * Runtime lightmap baking
    * Static, skinned and morphed meshes
    * GPU Particle engine with editor
    * PostFX library: bloom, edge detect, FXAA, vignette, etc
    * Seamless default material support from Maya, 3DS Max, Blender, etc.
    * Full model export pipeline from Maya, 3DS Max, Blender, etc via [Assets User Manual](https://developer.playcanvas.com/en/user-manual/assets/)
* **Profiler**

## Usage
```c++
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
#include <Containers\StringUtility.h>
#include <InputSystem\InputManager.h>

using namespace Engine::Common;
using namespace Engine::Containers;
using namespace Engine::CoreSystem;
using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::Rendering;
using namespace Engine::ResourceSystem;
using namespace Engine::Platform;
using namespace Engine::GameObjectSystem;
using namespace Engine::Utility;
using namespace Engine::FontSystem;
using namespace Engine::InputSystem;

const int WIDTH = 800;
const int HEIGHT = 600;
const float ASPECT_RATIO = (float)WIDTH / HEIGHT;

class WindowListener : public Window::IListener
{
public:
	WindowListener(Camera Camera) :
		m_Camera(Camera)
	{
	}

private:
	void OnWindowPositioned(Window* Window) override
	{
	}

	void OnWindowResized(Window* Window) override
	{
		m_Camera.SetAspectRatio(Window->GetClientSize().X / (float)Window->GetClientSize().Y);
	}

private:
	Camera m_Camera;
};

void main()
{
	FileSystem::SetWorkingPath(L"../");

	Core* core = Core::Create(RootAllocator::GetInstance());

	Window* window = core->CreateWindow({ WIDTH, HEIGHT }, "Test Launcher");

	core->Initialize();

	SceneManager* sceneMgr = SceneManager::GetInstance();
	ResourceManager* resources = ResourceManager::GetInstance();
	InputManager* input = InputManager::GetInstance();

	TextureResource brickTex = resources->Load<Texture>("Brick.jpg");
	ProgramResource shader = resources->Load<Program>("Shader.shader");
	MeshResource sphereMesh = resources->Load(PrimitiveMeshTypes::Sphere);
	MeshResource quadMesh = resources->Load(PrimitiveMeshTypes::Cube);
	FontResource font = resources->Load<Font>("consola.font");
	ProgramResource textShader = resources->Load<Program>("TextShader.shader");

	Material mat;
	mat.SetQueue(RenderQueues::Geometry);
	Pass pass(*shader);
	pass.SetTexture("diffuseTex", *brickTex);
	mat.AddPass(pass);

	Scene scene = sceneMgr->CreateScene();
	sceneMgr->SetActiveScene(scene);

	const int32 COUNT_X = 10;
	const int32 COUNT_Y = 10;
	GameObject gameObjects[COUNT_X * COUNT_Y];

	GameObject obj = scene.CreateRenderableGameObject();
	{
		Renderer renderer = obj.GetRenderer();
		renderer.SetMesh(*sphereMesh);
		renderer.SetMaterial(&mat);
		Transform tr = obj.GetTransform();
		tr.SetPosition({ 0, 0, 0 });
		tr.SetScale({ 1, 1, 1 });

	}

	Material textMat;
	textMat.SetQueue(RenderQueues::HUD);
	Pass textPass(*textShader);
	auto st = textPass.GetRenderState();
	st.CullMode = IDevice::CullModes::None;
	st.DepthTestFunction = IDevice::TestFunctions::Never;
	textPass.SetRenderState(st);
	textMat.AddPass(textPass);

	GameObject textObj = scene.CreateTextRenderableGameObject();
	TextRenderer textRen = textObj.GetTextRenderer();
	textRen.SetFont(*font);
	textRen.SetMaterial(&textMat);

	GameObject camObj = scene.CreateCameraGameObject();
	Camera camera = camObj.GetCamera();
	camObj.GetTransform().SetPosition({ 0, 0, -10 });

	camera.SetAspectRatio(ASPECT_RATIO);
	camera.SetFieldOfView(60);
	camera.SetNearClipDistance(0.1F);
	camera.SetFarClipDistance(100);

	GameObject amLightObj = scene.CreateLightingGameObject();
	Light amLight = amLightObj.GetLight();
	amLight.SetType(LightTypes::Ambient);
	amLight.SetStrength(1);
	amLight.SetColor({ 255, 255, 255 });

	WindowListener windowListener(camera);
	window->AddListener(&windowListener);


	PlatformFile::Handle watcherHandle = PlatformFile::CreateWatcher(resources->GetAssetsPath().GetValue(), true);
	PlatformFile::WatchInfo watchInfos[1024];

	float32 rot = 0;
	float32 z = 0;

	while (!window->ShouldClose())
	{
		uint32 len;
		PlatformFile::RefreshWatcher(watcherHandle, true, PlatformFile::WatchNotifyFilter::FileRenamed | PlatformFile::WatchNotifyFilter::DirectoryRenamed | PlatformFile::WatchNotifyFilter::LastWriteTimeChanged, watchInfos, 1024, len);

		if (len > 0)
		{
			WStringList files;

			for (uint32 i = 0; i < len; ++i)
			{
				PlatformFile::WatchInfo& info = watchInfos[i];

				if (info.Action != PlatformFile::WatchAction::Modified)
					continue;

				WString file(info.FileName, info.FileNameLength);

				if (!files.Contains(file))
					files.Add(file);
			}

			for each (auto & file in files)
				resources->Reload(file);
		}

		core->Update();

		textRen.SetText("FPS: " + StringUtility::ToString<char8>(core->GetFPS()));

		auto mPos = input->GetMousePosition();

		rot = 360.0F * (mPos.X / (float32)WIDTH);

		obj.GetTransform().SetScale({ abs(sin(rot)) * 2 });
	}

	ResourceManager::Destroy();
	Core::Destroy();
	SceneManager::Destroy();
}
```

## How to Build

Ensure you have Visual Studio 2017 or 2019 (Community Edition is acceptable)

Run SourceCode/Engine/CreateProjectFile.bat to build solution and project files

Open up SourceCode/Engine/Engine.sln

Build and Run

Tests are already placed inside the solution

## Getting Help

Contact me on [sh.omid.m@gmail.com](mailto://sh.omid.m@gmail.com)

## Contributing

Want to help me, Great!

So, let me know
