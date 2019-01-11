
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
		Vertex(Vector3F(-1.000000 ,-1.000000 ,1.000000),	Vector2F(0.000000 ,0.000000)),
		Vertex(Vector3F(-1.000000, -1.000000, -1.000000),	Vector2F(1.000000 ,0.000000)),
		Vertex(Vector3F(1.000000  ,-1.000000 ,-1.000000),	Vector2F(1.000000 ,1.000000)),
		Vertex(Vector3F(1.000000  ,-1.000000 ,1.000000),	Vector2F(0.000000 ,1.000000)),
		Vertex(Vector3F(-1.000000, 1.000000  ,1.000000),	Vector2F(0.000000 ,0.000000)),
		Vertex(Vector3F(-1.000000, 1.000000  ,-1.000000),	Vector2F(1.000000 ,0.000000)),
		Vertex(Vector3F(1.000000  ,1.000000  ,-1.000000),	Vector2F(1.000000 ,1.000000)),
		Vertex(Vector3F(1.000000  ,1.000000  ,1.000000),	Vector2F(0.000000 ,1.000000))
	};

	uint32 indicesBufferData[] = {
		//5, 6, 2,
		//5, 2, 1,

		//6, 7, 3,
		//6, 3, 2,

		//7, 8, 4,
		//7, 4, 3,

		8, 5, 1,
		8, 1, 3
	};

	SubMeshInfo subMeshInfo;
	subMeshInfo.Layout = SubMeshInfo::VertexLayouts::Position | SubMeshInfo::VertexLayouts::UV;
	subMeshInfo.Vertices.AddRange(vertices, sizeof(vertices) / sizeof(Vertex));
	subMeshInfo.Indices.AddRange(indicesBufferData, sizeof(indicesBufferData) / sizeof(uint32));

	MeshInfo meshInfo;
	meshInfo.SubMeshes.Add(subMeshInfo);

	MeshResource mesh(new ResourceHandle<Mesh>(device->CreateMesh(&meshInfo, IDevice::BufferUsages::StaticDraw)));

	device->SetClearColor(Color(0, 0, 0));

	Matrix4F projectionMat;
	projectionMat.MakePerspectiveProjectionMatrix(60, WIDTH / (float32)HEIGHT, 0.1F, 1000);

	Matrix4F viewMat;
	viewMat.MakeIdentity();
	viewMat.SetPosition(0, 0, 0);

	Matrix4F modelMat;
	modelMat.MakeIdentity();
	modelMat.SetPosition(0, 0, -5);

	float32 yaw = 0.0F;

	while (!window->ShouldClose())
	{
		//BeginProfilerFrame();

		//ProfileScope("BeginRender");

		yaw += 10.0F;
		modelMat.SetRotation(yaw, yaw, yaw);

		Matrix4F mvp = projectionMat * viewMat * modelMat;

		device->DrawMesh(*mesh1, mvp, *shader);

		rendering->BeginRender();

		rendering->EndRender();

		//EndProfilerFrame();
	}

	ResourceManager::Destroy();
	RenderingManager::Destroy();
}