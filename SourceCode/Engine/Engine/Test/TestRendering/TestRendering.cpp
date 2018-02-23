

#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Rendering\RenderingManager.h>

using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::Rendering;


void main()
{
	RenderingManager *rendering = RenderingManager::Create(RootAllocator::GetInstance());

	DeviceInterfarce *deivce = rendering->CreateDevice(DeviceInterfarce::Type::OpenGL);
}