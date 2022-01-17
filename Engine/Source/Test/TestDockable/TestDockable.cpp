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

class EditorRenderDevice : public EditorRenderDeviceBase
{
public:
	EditorRenderDevice(DeviceInterface* Device) :
		m_Device(Device)
	{
	}

protected:
	virtual void Render(Mesh* Mesh, const Matrix4F& Model, const Material* Material) override
	{
		//m_Device->DrawMesh(Mesh, Model, Matrix4F::Identity, GetProjectionMatrix(), Material);
	}

private:
	DeviceInterface* m_Device;
};

void test()
{
	StringStream stream;
	stream << "In [";
}

BEGIN_ENTRY_POINT
{
	DefaultAllocator::Create();
	RootAllocator::Create(DefaultAllocator::GetInstance());

	FileSystem::Initialize();

	test();

	LogManager::Create();
	LogManager::GetInstance()->GetCoreLogger()->OnLogEvent += CREATE_LAMBDA_EVENT_LISTENER(Logger::OnLogEventHandler, [](const Logger::Log& Log)
		{
			if (Log.Level > Logger::Levels::Info)
				std::cout << Log.Content.Value << std::endl;
		});

	ModuleManager::Create(RootAllocator::GetInstance());

	RenderManager::Create(RootAllocator::GetInstance());

	DeviceInterface* device = RenderManager::GetInstance()->CreateDevice(DeviceTypes::OpenGL);
	device->Initialize();

	FontManager::Create(RootAllocator::GetInstance());
	ResourceManager::Create(RootAllocator::GetInstance());

	EditorGUIManager::Create(RootAllocator::GetInstance());

	{
		PhysicalWindow physWindow;
		physWindow.SetSize({ 800, 600 });
		physWindow.SetTitle("Test Window Title!");

		EditorRenderDevice editorRenderDevice(device);

		while (!physWindow.IsClosed())
		{
			//_sleep(16);

			physWindow.UpdateAll();

			device->BeginRender();

			//device->SetContext(nullptr);
			//device->Clear(ClearFlags::ColorBuffer | ClearFlags::DepthBuffer | ClearFlags::StencilBuffer, ColorUI8::White, RenderQueues::Default);

			physWindow.RenderAll(&editorRenderDevice);

			device->EndRender();

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