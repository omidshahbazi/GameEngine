
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
#include <GameObjectSystem\SceneManager.h>
#include <Utility\HighResolutionTime.h>
#include <Utility\Window.h>
#include <IMGUI\imgui.h>

#include <iostream>


using namespace Engine::Common;
using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::Rendering;
using namespace Engine::ResourceSystem;
using namespace Engine::Profiler;
using namespace Engine::Containers;
using namespace Engine::Platform;
using namespace Engine::GameObjectSystem;
using namespace Engine::Utility;

const int WIDTH = 1024;
const int HEIGHT = 768;
const float ASPECT_RATIO = (float)WIDTH / HEIGHT;

void main()
{
	RealtimeProfiler::Create(RootAllocator::GetInstance());
	RenderingManager *rendering = RenderingManager::Create(RootAllocator::GetInstance());
	DeviceInterface *device = rendering->CreateDevice(DeviceInterface::Type::OpenGL);
	SceneManager *sceneMgr = SceneManager::Create(RootAllocator::GetInstance());

	Window window("Test Rendering");
	window.Initialize();
	window.SetSize({ WIDTH, HEIGHT });
	window.SetTitle("Test Rendering");

	device->SetWindow(&window);

	device->Initialize();

	ResourceManager *resources = ResourceManager::Create(RootAllocator::GetInstance());

	PlatformFile::Handle watcherHandle = PlatformFile::CreateWatcher(resources->GetAssetsPath().GetValue(), true);

	TextureResource tex = resources->Load<Texture>("WOOD.png");
	TextResource text = resources->Load<Text>("data.txt");
	ProgramResource shader = resources->Load<Program>("Shader.shader");
	ProgramResource shader1 = resources->Load<Program>("Shader1.shader");
	MeshResource ringMesh = resources->Load<Mesh>("Box.obj");
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
	TextureHandle *tex0 = resources->AllocateResourceHandle(rt->GetTexture(0));
	TextureHandle *tex1 = resources->AllocateResourceHandle(rt->GetTexture(1));
	TextureHandle *tex2 = resources->AllocateResourceHandle(rt->GetTexture(2));



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
	pass1.SetTexture("tex0", tex0);
	pass1.SetTexture("tex1", tex1);
	pass1.SetTexture("tex2", tex2);
	mat1.AddPass(pass1);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
	io.BackendPlatformName = "imgui_impl_glfw";
	io.DisplaySize.x = WIDTH;
	io.DisplaySize.y = HEIGHT;
	io.Fonts->AddFontDefault();
	io.Fonts->Build();

	// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
	//io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
	//io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	//io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	//io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	//io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	//io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
	//io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
	//io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	//io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	//io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
	//io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	//io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	//io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
	//io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	//io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	//io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	//io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	//io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	//io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	//io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	//io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

	//io.SetClipboardTextFn = ImGui_ImplGlfw_SetClipboardText;
	//io.GetClipboardTextFn = ImGui_ImplGlfw_GetClipboardText;
	//io.ClipboardUserData = g_Window;
	io.ImeWindowHandle = window.GetHandle();

	ImGui::StyleColorsDark();


	PlatformFile::WatchInfo watchInfos[1024];

	float32 fps = 0;
	uint32 frameCount = 0;
	uint64 nextCheckTime = HighResolutionTime::GetTime().GetMilliseconds() + 1000;

	while (!window.ShouldClose())
	{
		PlatformWindow::PollEvents();

		uint32 len;
		PlatformFile::RefreshWatcher(watcherHandle, true, PlatformFile::WatchNotifyFilter::FileRenamed | PlatformFile::WatchNotifyFilter::DirectoryRenamed | PlatformFile::WatchNotifyFilter::LastWriteTimeChanged, watchInfos, 1024, len);

		if (len > 0)
		{
			WStringList files;

			for (uint32 i = 0; i < len; ++i)
			{
				PlatformFile::WatchInfo &info = watchInfos[i];

				if (info.Action != PlatformFile::WatchAction::Modified)
					continue;

				WString file(info.FileName, info.FileNameLength);

				if (!files.Contains(file))
					files.Add(file);
			}

			for each (auto &file in files)
				resources->Reload(file);

		}

		device->SetRenderTarget(rt);
		device->Clear(IDevice::ClearFlags::ColorBuffer | IDevice::ClearFlags::DepthBuffer, Color(255, 0, 0, 255));

		yaw += 10.0F;
		modelMat.SetRotation(yaw, yaw, yaw);
		Matrix4F mvp = projectionMat * viewMat * modelMat;
		device->DrawMesh(*ringMesh, mvp, &mat);

		device->SetRenderTarget(nullptr);
		device->Clear(IDevice::ClearFlags::ColorBuffer | IDevice::ClearFlags::DepthBuffer, Color(0, 0, 0, 255));

		device->DrawMesh(*quadMesh, quadMat, &mat1);

		device->BeginRender();

		device->SubmitCommands();

		ImGui::NewFrame();

		ImGui::Text("This is some useful text.");

		ImGui::Render();

		device->EndRender();

		uint64 time = HighResolutionTime::GetTime().GetMilliseconds();

		++frameCount;

		if (time >= nextCheckTime)
		{
			fps = frameCount;
			frameCount = 0;
			nextCheckTime = time + 1000;


			std::cout << fps << std::endl;
		}
	}
}