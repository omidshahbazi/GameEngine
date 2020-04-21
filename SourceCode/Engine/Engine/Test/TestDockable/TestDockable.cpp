#include <MathContainers\MathContainers.h>
#include <MemoryManagement\Allocator\DefaultAllocator.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\RenderWindow.h>
#include <ResourceSystem\ResourceManager.h>
#include <EditorGUI\RenderableWindow.h>
#include <Platform\PlatformWindow.h>

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

class EditorRenderDevice : public EditorRenderDeviceBase, public RenderWindow::IListener
{
public:
	EditorRenderDevice(DeviceInterface* Device) :
		m_Device(Device),
		m_ViewMat(Matrix4F::Identity)
	{
		OnWindowResized(Device->GetWindow());
	}

	virtual void DrawMesh(Mesh* Mesh, const Matrix4F& Model, Material* Material) override
	{
		m_Device->DrawMesh(Mesh, Model, m_ViewMat, m_ProjMat, Material);
	}

	virtual void OnWindowMoved(Window* Window) override
	{
	}
	virtual void OnWindowResized(Window* Window) override
	{
		Vector2I size = Window->GetClientSize();

		//m_ProjMat.SetOrthographicProjection(size.X, size.Y, -1, 1);
		m_ProjMat.SetOrthographicProjection(size.X, 0, 0, size.Y, -1, 100); //To mirror the Y axis
	}
	virtual void OnKeyDown(PlatformWindow::VirtualKeys Key) override
	{
	}
	virtual void OnKeyUp(PlatformWindow::VirtualKeys Key) override
	{
	}
	virtual void OnKeyPressed(PlatformWindow::VirtualKeys Key) override
	{
	}
	virtual void OnMouseDown(PlatformWindow::VirtualKeys Key, Vector2I Position) override
	{
	}
	virtual void OnMouseUp(PlatformWindow::VirtualKeys Key, Vector2I Position) override
	{
	}
	virtual void OnMouseClick(PlatformWindow::VirtualKeys Key, Vector2I Position) override
	{
	}
	virtual void OnMouseWheel(Vector2I Position, uint16 Delta) override
	{
	}
	virtual void OnMouseMove(Vector2I Position) override
	{
	}
	virtual void OnMouseLeave(void) override
	{
	}
	virtual void OnClosing(void) override
	{
	}

private:
	DeviceInterface* m_Device;

	Matrix4F m_ViewMat;
	Matrix4F m_ProjMat;
};

void main()
{
	RenderWindow window("Test");
	window.SetMinimumSize({ 800, 600 });
	window.SetMaximumSize({ 19200, 10800 });
	window.SetSize({ 800, 600 });

	window.SetShowCaption(false);
	window.SetShowSystemMenu(false);
	window.SetShowMinimizeBox(false);
	window.SetShowMaximizeBox(false);
	window.SetShowFrame(false);


	RenderingManager::Create(DefaultAllocator::GetInstance());

	DeviceInterface* device = RenderingManager::GetInstance()->CreateDevice(DeviceInterface::Type::OpenGL);
	RenderContext* context = device->CreateContext(&window);
	device->SetContext(context);
	device->Initialize();

	ResourceManager::Create(DefaultAllocator::GetInstance());


	RenderableWindow renWin;
	renWin.SetRect({ 100, 100, 650, 450 });


	//Program::Handle programHandle;
	//device.CreateProgram(VertexProgram, FragmentProgram, programHandle);

	//SubMeshInfo subMeshInfo;
	//{
	//	subMeshInfo.Vertices.Add({ Vector3F(-1, 1, 0), Vector2F(0, 1) });
	//	subMeshInfo.Vertices.Add({ Vector3F(-1, -1, 0), Vector2F(0, 0) });
	//	subMeshInfo.Vertices.Add({ Vector3F(1, 1, 0), Vector2F(1, 1) });
	//	subMeshInfo.Vertices.Add({ Vector3F(1, -1, 0), Vector2F(1, 0) });
	//	subMeshInfo.Indices.Add(0);
	//	subMeshInfo.Indices.Add(1);
	//	subMeshInfo.Indices.Add(2);
	//	subMeshInfo.Indices.Add(2);
	//	subMeshInfo.Indices.Add(1);
	//	subMeshInfo.Indices.Add(3);
	//	subMeshInfo.Type = Mesh::SubMesh::PolygonTypes::Triangles;
	//	subMeshInfo.Layout = Mesh::SubMesh::VertexLayouts::Position | Mesh::SubMesh::VertexLayouts::UV;
	//}
	//GPUBuffer::Handle meshHandle;
	//device.CreateMesh(&subMeshInfo, GPUBuffer::Usages::StaticDraw, meshHandle);

	//device.SetPolygonMode(IDevice::CullModes::Both, IDevice::PolygonModes::Line);
	//device.SetClearColor({ 0, 0, 255, 255 }); 
	//device.BindProgram(programHandle);
	//device.BindMesh(meshHandle);

	EditorRenderDevice editorRenderDevice(device);
	window.AddListener(&editorRenderDevice);

	while (!window.ShouldClose())
	{
		//device.SetViewport(Vector2I::Zero, window.GetClientSize());

		//device.DrawIndexed(Mesh::SubMesh::PolygonTypes::Triangles, 6);

		device->BeginRender();

		device->SetRenderTarget(nullptr);
		device->Clear(IDevice::ClearFlags::ColorBuffer | IDevice::ClearFlags::DepthBuffer | IDevice::ClearFlags::StencilBuffer, ColorUI8::Yellow, RenderQueues::Default);

		renWin.Render(&editorRenderDevice);

		device->EndRender();

		PlatformWindow::PollEvents();
	}
}