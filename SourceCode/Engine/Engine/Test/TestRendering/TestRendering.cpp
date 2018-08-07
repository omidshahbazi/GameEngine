
#include <Common\BitwiseUtils.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\Color.h>
#include <Rendering\IDevice.h>
#include <ResourceSystem\ResourceManager.h>


using namespace Engine::Common;
using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::Rendering;
using namespace Engine::ResourceSystem;

const int WIDTH = 1024;
const int HEIGHT = 768;
const float ASPECT_RATIO = (float)WIDTH / HEIGHT;

void main()
{
	RenderingManager *rendering = RenderingManager::Create(RootAllocator::GetInstance());
	ResourceManager *resources = ResourceManager::Create(RootAllocator::GetInstance());

	DeviceInterfarce *device = rendering->CreateDevice(DeviceInterfarce::Type::OpenGL);

	device->Initialize();
	device->SetSampleCount(4);
	device->SetForwardCompatible(true);
	device->SetProfilingEnabled(true);

	TextureResource tex = resources->Load<Texture>("Wood.png");

	Window *window = device->CreateWindow(WIDTH, HEIGHT, "Test Rendering");

	device->SetClearColor(Color(255, 0, 0));

	while (!window->ShouldClose())
	{
		rendering->Update();
	}

	ResourceManager::Destroy();
	RenderingManager::Destroy();
}