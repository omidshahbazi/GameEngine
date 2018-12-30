
#include <Common\BitwiseUtils.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\Color.h>
#include <Rendering\IDevice.h>
#include <ResourceSystem\ResourceManager.h>
#include <Profiler\RealtimeProfiler.h>
#include <Profiler\Profiling.h>
#include <Containers\MathContainers.h>


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
	ResourceManager *resources = ResourceManager::Create(RootAllocator::GetInstance());

	DeviceInterface *device = rendering->CreateDevice(DeviceInterface::Type::OpenGL);

	device->Initialize();
	device->SetSampleCount(4);
	device->SetForwardCompatible(true);

	Window *window = device->CreateWindow(WIDTH, HEIGHT, "Test Rendering");

	TextureResource tex = resources->Load<Texture>("WOOD.png");
	TextResource text = resources->Load<Text>("data.txt");
	ProgramResource shader = resources->Load<Program>("Shader.shader");

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
	subMeshInfo.Vertex = vertices;
	subMeshInfo.VertexCount = 4;
	subMeshInfo.Indices = indicesBufferData;
	subMeshInfo.IndexCount = 6;

	MeshInfo meshInfo;
	meshInfo.SubMeshes = &subMeshInfo;
	meshInfo.SubMeshCount = 1;

	Mesh * mesh = device->CreateMesh(&meshInfo, IDevice::BufferUsages::StaticDraw);

	device->SetClearColor(Color(255, 0, 0));

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
		device->BeginRender();

		Matrix4F mvp = projectionMat * viewMat * modelMat;

		shader->SetTexture("difuse", *tex);
		shader->SetMatrix4("MVP", mvp);
		shader->SetColor("difCol", Color(255, 55, 0, 255));
		device->DrawMesh(mesh, *shader);

		//ProfileScope("BeginRender");
		device->EndRender();

		//EndProfilerFrame();
	}

	ResourceManager::Destroy();
	RenderingManager::Destroy();
}