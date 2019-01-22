
#include <Common\BitwiseUtils.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\Color.h>
#include <Rendering\IDevice.h>
#include <ResourceSystem\ResourceManager.h>
#include <Profiler\RealtimeProfiler.h>
#include <Profiler\Profiling.h>
#include <Containers\MathContainers.h>
#include <Rendering\Material.h>

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
	ProgramResource shader1 = resources->Load<Program>("Shader1.shader");
	MeshResource mesh1 = resources->Load<Mesh>("ring.obj");

	Material mat;
	Pass pass(*shader);
	IDevice::State state = pass.GetRenderState();
	state.SetPolygonMode(IDevice::PolygonModes::Fill);
	pass.SetRenderState(state);
	mat.AddPass(pass);

	pass.SetProgram(*shader1);
	state = pass.GetRenderState();
	state.SetPolygonMode(IDevice::PolygonModes::Line);
	pass.SetRenderState(state);
	mat.AddPass(pass);

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

		

		device->DrawMesh(*mesh1, mvp, &mat);

		rendering->BeginRender();

		rendering->EndRender();

		//EndProfilerFrame();
	}

	ResourceManager::Destroy();
	RenderingManager::Destroy();
}