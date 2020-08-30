#include <MathContainers\MathContainers.h>
#include <MemoryManagement\Allocator\DefaultAllocator.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\RenderWindow.h>
#include <ResourceSystem\ResourceManager.h>
#include <Platform\PlatformWindow.h>
#include <FontSystem\FontManager.h>

#include <EditorGUI\PhysicalWindow.h>
#include <EditorGUI\Private\Resources.h>

using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::Common;
using namespace Engine::Containers;
using namespace Engine::MathContainers;
using namespace Engine::Rendering;
using namespace Engine::FontSystem;
using namespace Engine::ResourceSystem;
using namespace Engine::Utility;
using namespace Engine::Platform;
using namespace Engine::EditorGUI;
using namespace Engine::EditorGUI::Private;

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

List<int32> Get(void)
{
	List<int32> list;
	list.Recap(4);

	list.Add(111);

	int a[] = { 222, 333, 444 };
	list.AddRange(a, 0, 3);

	//list.AddRange(list);

	list.Sort([](const int32& A, const int32& B) { return (A < B); });

	//list.Insert(0, 100);
	//list.Insert(0, 200);

	return list;
}

void main()
{
	List<int32> ls = Get();

	for each (auto val in ls)
	{
		std::cout << val;
	}

	RenderWindow window("InitializerWindow");
	window.SetIsVisible(false);

	RenderingManager::Create(DefaultAllocator::GetInstance());

	DeviceInterface* device = RenderingManager::GetInstance()->CreateDevice(DeviceInterface::Type::OpenGL);
	RenderContext* context = device->CreateContext(&window);
	device->SetContext(context);
	device->Initialize();

	FontManager::Create(DefaultAllocator::GetInstance());
	ResourceManager::Create(DefaultAllocator::GetInstance());

	Resources::Create(DefaultAllocator::GetInstance());

	{
		PhysicalWindow physWindow;
		physWindow.SetSize({ 800, 600 });
		physWindow.SetTitle("Test Window Title!");

		EditorRenderDevice editorRenderDevice(device);

		while (!physWindow.IsClosed())
		{
			_sleep(16);

			device->BeginRender();

			device->SetRenderTarget(nullptr);
			device->Clear(IDevice::ClearFlags::ColorBuffer | IDevice::ClearFlags::DepthBuffer | IDevice::ClearFlags::StencilBuffer, ColorUI8::White, RenderQueues::Default);

			physWindow.RenderAll(&editorRenderDevice);

			device->EndRender();

			PlatformWindow::PollEvents();
		}
	}

	device->DestroyContext(context);

	Resources::Destroy();
	ResourceManager::Destroy();
	FontManager::Destroy();
	RenderingManager::Destroy();
}