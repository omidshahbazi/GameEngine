#include <MathContainers\MathContainers.h>
#include <EntryPointUtility\EntryPoint.h>
#include <Allocators\RootAllocator.h>
#include <RenderSystem\RenderManager.h>
#include <RenderSystem\RenderWindow.h>
#include <DynamicModuleSystem\ModuleManager.h>
#include <ResourceManagement\ResourceManager.h>
#include <Platform\PlatformWindow.h>
#include <FontSystem\FontManager.h>
#include <EditorGUI\EditorGUIManager.h>
#include <EditorGUI\PhysicalWindow.h>
#include <FileUtility\FileSystem.h>
#include <Debugging\LogManager.h>
#include <Platform\PlatformGL.h>
#include <EntryPointUtility\ArgumentParser.h>

#include <iostream>

USE_HIGH_PERFORMANCE_GPU();

using namespace Engine::Allocators;
using namespace Engine::Common;
using namespace Engine::Containers;
using namespace Engine::MathContainers;
using namespace Engine::RenderCommon;
using namespace Engine::RenderSystem;
using namespace Engine::RenderDevice;
using namespace Engine::FontSystem;
using namespace Engine::ResourceManagement;
using namespace Engine::FileUtility;
using namespace Engine::Platform;
using namespace Engine::EditorGUI;
using namespace Engine::EditorGUI::Private;
using namespace Engine::Debugging;
using namespace Engine::DynamicModuleSystem;
using namespace Engine::EntryPointUtility;

BEGIN_ENTRY_POINT
{
	DefaultAllocator::Create();
	RootAllocator::Create(DefaultAllocator::GetInstance());

	ArgumentParser args(ArgumentCount, Arguments, true);

	FileSystem::Initialize();

	LogManager::Create();
	LogManager::GetInstance()->GetCoreLogger()->OnLogEvent += CREATE_LAMBDA_EVENT_LISTENER(Logger::OnLogEventHandler, [](const Logger::Log& Log)
		{
			if (Log.Level > Logger::Levels::Info)
				std::cout << Log.Content.Value << std::endl;
		});

	ModuleManager::Create(RootAllocator::GetInstance());

	RenderManager::Create(RootAllocator::GetInstance());

	DeviceTypes deviceType;
	const String deviceTypeString = args.GetAsString("DeviceType");
	if (deviceTypeString == "OpenGL")
		deviceType = DeviceTypes::OpenGL;
	else if (deviceTypeString == "DirectX12")
		deviceType = DeviceTypes::DirectX12;
	else
		deviceType = DeviceTypes::Vulkan;

	DeviceInterface* device = RenderManager::GetInstance()->CreateDevice(deviceType);
	device->Initialize();

	FontManager::Create(RootAllocator::GetInstance());
	ResourceManager::Create(RootAllocator::GetInstance());

	EditorGUIManager::Create(RootAllocator::GetInstance());

	{
		PhysicalWindow physWindow(device);
		physWindow.SetSize({ 1080, 920 });
		physWindow.SetTitle("Test Window Title");

		EditorRenderCommandBuffer cmd;

		while (!physWindow.IsClosed())
		{
			//_sleep(16);

			physWindow.UpdateAll();

			device->BeginFrame(physWindow.GetContext());

			cmd.Clear();

			physWindow.RenderAll(&cmd);

			device->SubmitCommandBuffer(&cmd);

			device->EndFrame();

			PlatformWindow::PollEvents();
		}
	}

	EditorGUIManager::Destroy();
	ResourceManager::Destroy();
	FontManager::Destroy();
	RenderManager::Destroy();
	FileSystem::Deinitialize();

	ModuleManager::Destroy();

	LogManager::Destroy();
}
END_ENTRY_POINT


//UNDONE:RENDERING -> Put guards in DeviceInterface