
#include <Common\BitwiseUtils.h>
#include <MemoryManagement\Allocator\Initializer.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Rendering\RenderingManager.h>
#include <Containers\Color.h>
#include <Rendering\IDevice.h>
#include <ResourceSystem\ResourceManager.h>
#include <Profiler\RealtimeProfiler.h>
#include <Profiler\Profiling.h>
#include <MathContainers\MathContainers.h>
#include <Rendering\Material.h>
#include <Platform\PlatformFile.h>
#include <GameObjectSystem\SceneManager.h>
#include <Utility\HighResolutionTime.h>
#include <Utility\Window.h>
#include <Utility\FileSystem.h>
#include <iostream>

using namespace Engine::Common;
using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::Rendering;
using namespace Engine::ResourceSystem;
using namespace Engine::Profiler;
using namespace Engine::Containers;
using namespace Engine::MathContainers;
using namespace Engine::Platform;
using namespace Engine::GameObjectSystem;
using namespace Engine::Utility;

const int WIDTH = 800;
const int HEIGHT = 600;
const float ASPECT_RATIO = (float)WIDTH / HEIGHT;

void main()
{
	Initializer::Create();
	Initializer::GetInstance()->Initialize(GigaByte * 4, L"Alllocators.data");

	FileSystem::Initialize();
	FileSystem::SetWorkingPath(L"D:\\Projects\\GameEngineAssets");

	RealtimeProfiler::Create(RootAllocator::GetInstance());
	RenderingManager* rendering = RenderingManager::Create(RootAllocator::GetInstance());
	DeviceInterface* device = rendering->CreateDevice(DeviceTypes::OpenGL);
	SceneManager* sceneMgr = SceneManager::Create(RootAllocator::GetInstance());

	device->Initialize();

	Window window("Test Rendering");
	window.Initialize();
	window.SetIsVisible(true);
	window.SetMaximumSize({ WIDTH, HEIGHT });
	window.SetMinimumSize({ WIDTH, HEIGHT });
	window.SetSize({ WIDTH, HEIGHT });
	window.SetTitle("Test Rendering");

	RenderContext* context = device->CreateContext(&window);
	device->SetContext(context);


	//Window window1("Test Rendering 1");
	//window1.Initialize();
	//window1.SetIsVisible(true);
	//window1.SetSize({ WIDTH, HEIGHT });
	//window1.SetMaximumSize({ WIDTH, HEIGHT });
	//window1.SetMinimumSize({ WIDTH, HEIGHT });
	//window1.SetTitle("Test Rendering 1");
	//RenderContext* context1 = device->CreateContext(&window1);

	//ResourceManager* resources = ResourceManager::Create(RootAllocator::GetInstance());

	//resources->GetCompiler()->CompileResources();

	//MeshResource* mesh = resources->GetPrimitiveMesh(ResourceManager::PrimitiveMeshTypes::Sphere);
	//TextureResource* brickTex = resources->Load<Texture>("Brick.jpg");
	//ShaderResource* shader = resources->Load<Shader>("ShaderTest.shader");

	//Material mat;
	//Pass pass(shader);
	//pass.SetQueue(RenderQueues::Geometry);
	//pass.SetTexture("diffuseTex", brickTex);
	//mat.AddPass(pass);

	float32 fps = 0;
	uint32 frameCount = 0;
	uint64 nextCheckTime = HighResolutionTime::GetTime().GetMilliseconds() + 1000;

	Matrix4F projection;
	projection.SetPerspectiveProjection(60 * Mathematics::DEGREES_TO_RADIANS, WIDTH / (float32)HEIGHT, 0.1F, 2000);

	while (!window.IsClosed())
	{
		PlatformWindow::PollEvents();

		Matrix4F idMat = Matrix4F::Identity;
		idMat.SetTranslate({ 0, 0, -10 });

		idMat = projection * idMat;

		//device->SetContext(context);

		device->BeginRender();

		device->Clear(IDevice::ClearFlags::ColorBuffer | IDevice::ClearFlags::DepthBuffer, ColorUI8::Cyan);

		//device->DrawMesh(mesh->GetPointer(), idMat, &mat);

		device->EndRender();


		//device->SetContext(context1);

		//device->BeginRender();

		//device->DrawMesh(mesh.GetData()->GetData(), idMat, &characterMat);

		//device->EndRender();

		uint64 time = HighResolutionTime::GetTime().GetMilliseconds();

		++frameCount;

		if (time >= nextCheckTime)
		{
			fps = frameCount;
			frameCount = 0;
			nextCheckTime = time + 1000;



			std::cout << fps << std::endl;
		}
	}

	device->DestroyContext(context);

	SceneManager::Destroy();
	ResourceManager::Destroy();
	RenderingManager::Destroy();
	FileSystem::Deinitialize();
}