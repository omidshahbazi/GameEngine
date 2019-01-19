
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
	MeshResource mesh1 = resources->Load<Mesh>("ring.obj");

	Material mat;
	Pass pass(*shader);

	pass.SetFaceOrder(IDevice::FaceOrders::CounterClockwise);
	pass.SetCullMode(IDevice::CullModes::Back);
	pass.SetDepthTestFunction(IDevice::TestFunctions::Less);
	//pass.SetStencilTestFunction(IDevice::TestFunctions::Always, 1, 0xFF);
	//pass.SetPolygonMode(IDevice::CullModes::Both, IDevice::PolygonModes::Line);
	//pass.SetStencilMask(1);
	//pass.SetStencilOperation(IDevice::StencilOperations::Zero, IDevice::StencilOperations::Zero, IDevice::StencilOperations::Zero);
	//pass.SetBlendFunction(IDevice::BlendFunctions::SourceColor, IDevice::BlendFunctions::OneMinusSourceColor);

	mat.AddPass(pass);

	pass.SetPolygonMode(IDevice::CullModes::Both, IDevice::PolygonModes::Line);
	mat.AddPass(pass);

	device->SetClearFlags(IDevice::ClearFlags::ColorBuffer | IDevice::ClearFlags::DepthBuffer | IDevice::ClearFlags::StencilBuffer);
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

		

		device->DrawMesh(*mesh1, mvp, &mat);

		rendering->BeginRender();

		rendering->EndRender();

		//EndProfilerFrame();
	}

	ResourceManager::Destroy();
	RenderingManager::Destroy();
}