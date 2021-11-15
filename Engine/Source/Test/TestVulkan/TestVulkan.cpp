#include <RenderSystem\RenderManager.h>
#include <Allocators\DefaultAllocator.h>
#include <Allocators\RootAllocator.h>
#include <DynamicModuleSystem\ModuleManager.h>
#include <FileUtility\FileSystem.h>

using namespace Engine::Allocators;
using namespace Engine::RenderCommon;
using namespace Engine::RenderSystem;
using namespace Engine::DynamicModuleSystem;
using namespace Engine::FileUtility;

void main()
{
	DefaultAllocator::Create();
	RootAllocator::Create(DefaultAllocator::GetInstance());
	FileSystem::Initialize();
	ModuleManager::Create(RootAllocator::GetInstance());
	RenderManager::Create(RootAllocator::GetInstance());

	DeviceInterface* device = RenderManager::GetInstance()->CreateDevice(DeviceTypes::Vulkan);

	device->Initialize();
}