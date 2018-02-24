

#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Rendering\RenderingManager.h>

#include <Containers\Vector.h>

using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::Rendering;

const int WIDTH = 1024;
const int HEIGHT = 768;
const float ASPECT_RATIO = (float)WIDTH / HEIGHT;


void main()
{
	Engine::Containers::Vector<int> vector;
	vector.Add(1);

	//for each (auto a in vector)
	//{

	//}


	//RenderingManager *rendering = RenderingManager::Create(RootAllocator::GetInstance());

	//DeviceInterfarce *device = rendering->CreateDevice(DeviceInterfarce::Type::OpenGL);

	//device->Initialize();
	//device->SetSampleCount(4);
	//device->SetForwardCompatible(true);
	//device->SetProfilingEnabled(true);

	//Window *window = device->CreateWindow(WIDTH, HEIGHT, "Test Rendering");

	//RenderingManager::Destroy();
}