
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
#include <Platform\PlatformFile.h>


using namespace Engine::Common;
using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::Rendering;
using namespace Engine::ResourceSystem;
using namespace Engine::Profiler;
using namespace Engine::Containers;
using namespace Engine::Platform;

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

	PlatformFile::Handle watcherHandle = PlatformFile::CreateWatcher(resources->GetAssetsPath().GetValue());

	TextureResource tex = resources->Load<Texture>("WOOD.png");
	TextResource text = resources->Load<Text>("data.txt");
	ProgramResource shader = resources->Load<Program>("Shader.shader");
	ProgramResource shader1 = resources->Load<Program>("Shader1.shader");
	MeshResource ringMesh = resources->Load<Mesh>("ring.obj");
	MeshResource quadMesh = resources->Load(PrimitiveMeshTypes::Quad);

	RenderTargetInfo info;

	RenderTextureInfo colorTexInfo1;
	colorTexInfo1.Width = WIDTH;
	colorTexInfo1.Height = HEIGHT;
	colorTexInfo1.Format = Texture::Formats::RGB8;
	colorTexInfo1.Point = RenderTarget::AttachmentPoints::Color0;
	info.Textures.Add(colorTexInfo1);

	RenderTextureInfo colorTexInfo2;
	colorTexInfo2.Width = WIDTH;
	colorTexInfo2.Height = HEIGHT;
	colorTexInfo2.Format = Texture::Formats::RGB8;
	colorTexInfo2.Point = RenderTarget::AttachmentPoints::Color1;
	info.Textures.Add(colorTexInfo2);

	RenderTextureInfo depthTexInfo;
	depthTexInfo.Width = WIDTH;
	depthTexInfo.Height = HEIGHT;
	depthTexInfo.Format = Texture::Formats::Depth24;
	depthTexInfo.Point = RenderTarget::AttachmentPoints::Depth;
	info.Textures.Add(depthTexInfo);

	RenderTarget *rt = device->CreateRenderTarget(&info);

	Matrix4F projectionMat;
	projectionMat.MakePerspectiveProjectionMatrix(60, WIDTH / (float32)HEIGHT, 0.1F, 1000);

	Matrix4F viewMat;
	viewMat.MakeIdentity();
	viewMat.SetPosition(0, 0, 0);

	Material mat;
	Pass pass(*shader);
	IDevice::State state = pass.GetRenderState();
	state.SetPolygonMode(IDevice::PolygonModes::Fill);
	pass.SetRenderState(state);
	pass.SetTexture("tex1", *tex);
	mat.AddPass(pass);

	Matrix4F modelMat;
	modelMat.MakeIdentity();
	modelMat.SetPosition(0, 0, -2);
	float32 yaw = 0.0F;

	Matrix4F quadMat;
	quadMat.MakeIdentity();

	Material mat1;
	Pass pass1(*shader1);
	IDevice::State state1 = pass1.GetRenderState();
	state1.SetPolygonMode(IDevice::PolygonModes::Fill);
	pass1.SetRenderState(state1);
	pass1.SetTexture("tex0", rt->GetTexture(0));
	pass1.SetTexture("tex1", rt->GetTexture(1));
	pass1.SetTexture("tex2", rt->GetTexture(2));
	mat1.AddPass(pass1);

	PlatformFile::WatchInfo watchInfos[1024];

	while (!window->ShouldClose())
	{

		uint32 len;
		PlatformFile::RefreshWatcher(watcherHandle, true, PlatformFile::WatchNotifyFilter::FileRenamed, watchInfos, 1024, len);

		if (len > 0)
		{
			resources->Reload(WString(watchInfos->FileName, watchInfos->FileNameLength));
		}
		//BeginProfilerFrame();

		//ProfileScope("BeginRender");


		//device->SetRenderTarget(rt);
		//device->Clear(IDevice::ClearFlags::ColorBuffer | IDevice::ClearFlags::DepthBuffer, Color(0, 0, 0, 255));

		//yaw += 10.0F;
		//modelMat.SetRotation(yaw, yaw, yaw);
		//Matrix4F mvp = projectionMat * viewMat * modelMat;
		//device->DrawMesh(*ringMesh, mvp, &mat);

		//device->SetRenderTarget(nullptr);
		//device->Clear(IDevice::ClearFlags::ColorBuffer | IDevice::ClearFlags::DepthBuffer, Color(0, 0, 0, 255));

		//device->DrawMesh(*quadMesh, quadMat, &mat1);




		//device->BeginRender();

		//device->SubmitCommands();

		//device->EndRender();




		//EndProfilerFrame();
	}

	ResourceManager::Destroy();
	RenderingManager::Destroy();




	PlatformFile::CloseWatcher(watcherHandle);
}