#include <Utility\Window.h>
#include <Platform\PlatformWindow.h>

using namespace Engine::Utility;
using namespace Engine::Platform;

void main()
{
	Window window("Test");
	window.Initialize();
	window.SetMinimumSize({ 800, 600 });
	window.SetMaximumSize({ 100024, 768000 });
	window.SetSize({ 800, 600 });

	window.SetIsVisible(true);

	window.SetState(Window::States::Maximized);

	window.SetShowMaximizeBox(false);

	while (!window.ShouldClose())
	{
		PlatformWindow::PollEvents();
	}
}




//#include <Utility/Window.h>
//#include <Rendering/Private/OpenGL/OpenGLDevice.h>
//#include <Platform/PlatformWindow.h>
//#include <Common/BitwiseUtils.h>
//#include <MathContainers/MathContainers.h>
//#include <iostream>
//
//using namespace Engine::Common;
//using namespace Engine::Utility;
//using namespace Engine::Platform;
//using namespace Engine::MathContainers;
//using namespace Engine::Rendering;
//using namespace Engine::Rendering::Private::OpenGL;
//
//const char* VertexProgram = "#version 330 core\n"
//"layout(location = 0) in vec3 aPos;"
//"void main()"
//"{"
//"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);"
//"}";
//const char* FragmentProgram = "#version 330 core\n"
//"out vec4 FragColor;"
//"void main()"
//"{"
//"	FragColor = vec4(1.0f, 0, 0, 1.0f);"
//"}";
//
//void main()
//{
//	Window window1("window1");
//	window1.Initialize();
//	window1.SetMinimumSize({ 800, 600 });
//	window1.SetMaximumSize({ 800, 600 });
//	window1.SetSize({ 800, 600 });
//	window1.SetIsVisible(true);
//
//	Window window2("window2");
//	window2.Initialize();
//	window2.SetMinimumSize({ 400, 300 });
//	window2.SetMaximumSize({ 800, 600 });
//	window2.SetSize({ 400, 300 });
//	window2.SetIsVisible(true);
//
//	OpenGLDevice device;
//
//	RenderContext* context1 = device.CreateContext(window1.GetHandle());
//	RenderContext* context2 = device.CreateContext(window2.GetHandle());
//	device.SetContext(context1);
//
//	device.Initialize();
//	device.SetDebugCallback([](int32 ID, IDevice::DebugSources Source, cstr Message, IDevice::DebugTypes Type, IDevice::DebugSeverities Severity) {std::cout << Message << std::endl; });
//
//	//device.SetWindow(window2.GetHandle());
//
//	Program::Handle programHandle;
//	device.CreateProgram(VertexProgram, FragmentProgram, programHandle);
//
//	SubMeshInfo subMeshInfo;
//	{
//		subMeshInfo.Vertices.Add({ Vector3F(-1, 1, 0), Vector2F(0, 1) });
//		subMeshInfo.Vertices.Add({ Vector3F(-1, -1, 0), Vector2F(0, 0) });
//		subMeshInfo.Vertices.Add({ Vector3F(1, 1, 0), Vector2F(1, 1) });
//		subMeshInfo.Vertices.Add({ Vector3F(1, -1, 0), Vector2F(1, 0) });
//		subMeshInfo.Indices.Add(0);
//		subMeshInfo.Indices.Add(1);
//		subMeshInfo.Indices.Add(2);
//		subMeshInfo.Indices.Add(2);
//		subMeshInfo.Indices.Add(1);
//		subMeshInfo.Indices.Add(3);
//		subMeshInfo.Type = Mesh::SubMesh::PolygonTypes::Triangles;
//		subMeshInfo.Layout = Mesh::SubMesh::VertexLayouts::Position | Mesh::SubMesh::VertexLayouts::UV;
//	}
//	GPUBuffer::Handle meshHandle;
//	device.CreateMesh(&subMeshInfo, IDevice::BufferUsages::StaticDraw, meshHandle);
//
//	while (!window1.ShouldClose())
//	{
//		device.SetContext(context1);
//		{
//			device.SetViewport(Vector2I::Zero, window1.GetClientSize());
//			device.SetPolygonMode(IDevice::CullModes::Both, IDevice::PolygonModes::Line);
//
//			device.SetClearColor({ 0, 0, 255, 255 });
//			device.Clear(IDevice::ClearFlags::ColorBuffer | IDevice::ClearFlags::DepthBuffer | IDevice::ClearFlags::StencilBuffer);
//
//			device.BindProgram(programHandle);
//			device.BindMesh(meshHandle);
//
//			device.DrawIndexed(Mesh::SubMesh::PolygonTypes::Triangles, 6);
//
//			device.SwapBuffers();
//		}
//
//		device.SetContext(context2);
//		{
//			device.SetViewport(Vector2I::Zero, window1.GetClientSize());
//			device.SetPolygonMode(IDevice::CullModes::Both, IDevice::PolygonModes::Line);
//
//			device.SetClearColor({ 0, 255, 0, 255 });
//			device.Clear(IDevice::ClearFlags::ColorBuffer | IDevice::ClearFlags::DepthBuffer | IDevice::ClearFlags::StencilBuffer);
//
//			device.BindProgram(programHandle);
//			device.BindMesh(meshHandle);
//
//			device.DrawIndexed(Mesh::SubMesh::PolygonTypes::Triangles, 6);
//
//			device.SwapBuffers();
//		}
//
//		PlatformWindow::PollEvents();
//	}
//}