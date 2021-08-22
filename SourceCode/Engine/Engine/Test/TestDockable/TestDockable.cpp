#include <MathContainers\MathContainers.h>
#include <MemoryManagement\Allocator\Initializer.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\RenderWindow.h>
#include <ResourceSystem\ResourceManager.h>
#include <Platform\PlatformWindow.h>
#include <FontSystem\FontManager.h>
#include <EditorGUI\EditorGUIManager.h>
#include <EditorGUI\PhysicalWindow.h>
#include <FileUtility\FileSystem.h>
#include <Debugging\LogManager.h>

using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::Common;
using namespace Engine::Containers;
using namespace Engine::MathContainers;
using namespace Engine::Rendering;
using namespace Engine::FontSystem;
using namespace Engine::ResourceSystem;
using namespace Engine::FileUtility;
using namespace Engine::Platform;
using namespace Engine::EditorGUI;
using namespace Engine::EditorGUI::Private;
using namespace Engine::Debugging;

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
		m_Device->DrawMesh(Mesh, Model, Matrix4F::Identity, GetProjectionMatrix(), Material);
	}

private:
	DeviceInterface* m_Device;
};

void main(void)
{
	//int a;
	//std::cin >> a;

	Initializer::Create();
	Initializer::GetInstance()->Initialize(GigaByte * 4, L"Alllocators.data");

	FileSystem::Initialize();

	LogManager::Create();

	RenderingManager::Create(RootAllocator::GetInstance());

	DeviceInterface* device = RenderingManager::GetInstance()->CreateDevice(DeviceTypes::DirectX12);
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

			device->SetRenderTarget(nullptr);
			device->Clear(IDevice::ClearFlags::ColorBuffer | IDevice::ClearFlags::DepthBuffer | IDevice::ClearFlags::StencilBuffer, ColorUI8::White, RenderQueues::Default);

			physWindow.RenderAll(&editorRenderDevice);

			device->EndRender();

			PlatformWindow::PollEvents();
		}
	}

	EditorGUIManager::Destroy();
	ResourceManager::Destroy();
	FontManager::Destroy();
	RenderingManager::Destroy();
	FileSystem::Deinitialize();

	LogManager::Destroy();
}