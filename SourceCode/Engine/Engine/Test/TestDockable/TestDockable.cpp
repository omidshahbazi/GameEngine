#include <MathContainers\MathContainers.h>
#include <MemoryManagement\Allocator\DefaultAllocator.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\RenderWindow.h>
#include <ResourceSystem\ResourceManager.h>
#include <Platform\PlatformWindow.h>

#include <EditorGUI\PhysicalWindow.h>

using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::Common;
using namespace Engine::Containers;
using namespace Engine::MathContainers;
using namespace Engine::Rendering;
using namespace Engine::ResourceSystem;
using namespace Engine::Utility;
using namespace Engine::Platform;
using namespace Engine::EditorGUI;

const char* VertexProgram = "#version 330 core\n"
"layout(location = 0) in vec3 aPos;"
"void main()"
"{"
"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);"
"}";
const char* FragmentProgram = "#version 330 core\n"
"out vec4 FragColor;"
"void main()"
"{"
"	FragColor = vec4(1.0f, 0, 0, 1.0f);"
"}";

class EditorRenderDevice : public EditorRenderDeviceBase
{
public:
	EditorRenderDevice(DeviceInterface* Device) :
		m_Device(Device)
	{
	}

	virtual void DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Material* Material) override
	{
		m_Device->DrawMesh(Mesh, Model, Matrix4F::Identity, GetProjectionMatrix(), Material);
	}

private:
	DeviceInterface* m_Device;
};

void main()
{
	RenderWindow window("InitializerWindow");
	window.SetIsVisible(false);


	RenderingManager::Create(DefaultAllocator::GetInstance());

	DeviceInterface* device = RenderingManager::GetInstance()->CreateDevice(DeviceInterface::Type::OpenGL);
	RenderContext* context = device->CreateContext(&window);
	device->SetContext(context);
	device->Initialize();

	ResourceManager::Create(DefaultAllocator::GetInstance());


	PhysicalWindow physWindow;
	physWindow.SetRect({ 0,0,800,600 });

	Button bt;
	bt.SetSize(Vector2I::One * 50);
	physWindow.AddChild(&bt);

	EditorRenderDevice editorRenderDevice(device);

	while (!window.ShouldClose())
	{
		_sleep(16);

		device->BeginRender();

		device->SetRenderTarget(nullptr);
		device->Clear(IDevice::ClearFlags::ColorBuffer | IDevice::ClearFlags::DepthBuffer | IDevice::ClearFlags::StencilBuffer, ColorUI8::Yellow, RenderQueues::Default);

		physWindow.RenderAll(&editorRenderDevice);

		device->EndRender();

		PlatformWindow::PollEvents();
	}
}