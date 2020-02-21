#include <MathContainers\MathContainers.h>
#include <Rendering\RenderWindow.h>
#include <Platform\PlatformWindow.h>
#include <Rendering\Private\OpenGL\OpenGLDevice.h>
#include <iostream>

using namespace Engine::Common;
using namespace Engine::MathContainers;
using namespace Engine::Rendering::Private::OpenGL;
using namespace Engine::Rendering;
using namespace Engine::Utility;
using namespace Engine::Platform;

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

void main()
{
	RenderWindow window("Test");
	window.SetMinimumSize({ 800, 600 });
	window.SetMaximumSize({ 19200, 10800 });
	window.SetSize({ 800, 600 });
	window.SetIsFullscreen(false);

	OpenGLDevice device;
	RenderContext* context = device.CreateContext(window.GetHandle());
	device.SetContext(context);
	device.Initialize();
	device.SetDebugCallback([](int32 ID, IDevice::DebugSources Source, cstr Message, IDevice::DebugTypes Type, IDevice::DebugSeverities Severity) { std::cout << Message << std::endl; });

	Program::Handle programHandle;
	device.CreateProgram(VertexProgram, FragmentProgram, programHandle);

	SubMeshInfo subMeshInfo;
	{
		subMeshInfo.Vertices.Add({ Vector3F(-1, 1, 0), Vector2F(0, 1) });
		subMeshInfo.Vertices.Add({ Vector3F(-1, -1, 0), Vector2F(0, 0) });
		subMeshInfo.Vertices.Add({ Vector3F(1, 1, 0), Vector2F(1, 1) });
		subMeshInfo.Vertices.Add({ Vector3F(1, -1, 0), Vector2F(1, 0) });
		subMeshInfo.Indices.Add(0);
		subMeshInfo.Indices.Add(1);
		subMeshInfo.Indices.Add(2);
		subMeshInfo.Indices.Add(2);
		subMeshInfo.Indices.Add(1);
		subMeshInfo.Indices.Add(3);
		subMeshInfo.Type = Mesh::SubMesh::PolygonTypes::Triangles;
		subMeshInfo.Layout = Mesh::SubMesh::VertexLayouts::Position | Mesh::SubMesh::VertexLayouts::UV;
	}
	GPUBuffer::Handle meshHandle;
	device.CreateMesh(&subMeshInfo, IDevice::BufferUsages::StaticDraw, meshHandle);

	device.SetPolygonMode(IDevice::CullModes::Both, IDevice::PolygonModes::Line);
	device.SetClearColor({ 0, 0, 255, 255 });
	device.BindProgram(programHandle);
	device.BindMesh(meshHandle);

	while (!window.ShouldClose())
	{
		device.SetViewport(Vector2I::Zero, window.GetClientSize());

		device.Clear(IDevice::ClearFlags::ColorBuffer | IDevice::ClearFlags::DepthBuffer | IDevice::ClearFlags::StencilBuffer);

		device.DrawIndexed(Mesh::SubMesh::PolygonTypes::Triangles, 6);

		device.SwapBuffers();

		PlatformWindow::PollEvents();
	}
}