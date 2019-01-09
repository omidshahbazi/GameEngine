
#include <Common\BitwiseUtils.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\Color.h>
#include <Rendering\IDevice.h>
#include <ResourceSystem\ResourceManager.h>
#include <Profiler\RealtimeProfiler.h>
#include <Profiler\Profiling.h>
#include <Containers\MathContainers.h>

#include <sstream>
#include <streambuf>


using namespace Engine::Common;
using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::Rendering;
using namespace Engine::ResourceSystem;
using namespace Engine::Profiler;
using namespace Engine::Containers;

const int WIDTH = 1024;
const int HEIGHT = 768;
const float ASPECT_RATIO = (float)WIDTH / HEIGHT;

void main()
{
	RealtimeProfiler::Create(RootAllocator::GetInstance());

	RenderingManager *rendering = RenderingManager::Create(RootAllocator::GetInstance());

	DeviceInterface *device = rendering->CreateDevice(DeviceInterface::Type::OpenGL);

	device->Initialize();
	device->SetSampleCount(4);
	device->SetForwardCompatible(true);

	Window *window = device->CreateWindow(WIDTH, HEIGHT, "Test Rendering");

	ResourceManager *resources = ResourceManager::Create(RootAllocator::GetInstance());

	TextureResource tex = resources->Load<Texture>("WOOD.png");
	TextResource text = resources->Load<Text>("data.txt");
	ProgramResource shader = resources->Load<Program>("Shader.shader");
	MeshResource mesh1 = resources->Load<Mesh>("box.obj");

	Vertex vertices[] =
	{
		Vertex(Vector3F(-0.5, -0.5, 0),	Vector2F(0, 0)),
		Vertex(Vector3F(-0.5, 0.5, 0),	Vector2F(1, 0)),
		Vertex(Vector3F(0.5, 0.5, 0),	Vector2F(0, 1)),
		Vertex(Vector3F(0.5, -0.5, 0),	Vector2F(1, 1))
	};

	uint32 indicesBufferData[] = {
		0, 1, 2,
		0, 2, 3
	};

	SubMeshInfo subMeshInfo;
	subMeshInfo.Layout = SubMeshInfo::VertexLayouts::Position | SubMeshInfo::VertexLayouts::UV;
	subMeshInfo.Vertices.AddRange(vertices, 4);
	subMeshInfo.Indices.AddRange(indicesBufferData, 6);

	MeshInfo meshInfo;
	meshInfo.SubMeshes.Add(subMeshInfo);

	Mesh * mesh = device->CreateMesh(&meshInfo, IDevice::BufferUsages::StaticDraw);

	device->SetClearColor(Color(0, 0, 0));

	Matrix4F projectionMat;
	projectionMat.MakePerspectiveProjectionMatrix(60, WIDTH / (float32)HEIGHT, 0.1F, 1000);

	Matrix4F viewMat;
	viewMat.MakeIdentity();
	viewMat.SetPosition(0, 0, 0);

	Matrix4F modelMat;
	modelMat.MakeIdentity();
	modelMat.SetPosition(0, 0, -10);

	while (!window->ShouldClose())
	{
		//BeginProfilerFrame();

		//ProfileScope("BeginRender");

		Matrix4F mvp = projectionMat * viewMat * modelMat;

		device->DrawMesh(*mesh1, mvp, *shader);

		rendering->BeginRender();

		rendering->EndRender();

		//EndProfilerFrame();
	}

	ResourceManager::Destroy();
	RenderingManager::Destroy();
}