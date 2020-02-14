
#include <Utility/Window.h>
#include <Rendering/Private/OpenGL/OpenGLDevice.h>
#include <Platform/PlatformWindow.h>
#include <Common/BitwiseUtils.h>
#include <MathContainers/MathContainers.h>
#include <iostream>

using namespace Engine::Common;
using namespace Engine::Utility;
using namespace Engine::Platform;
using namespace Engine::MathContainers;
using namespace Engine::Rendering;
using namespace Engine::Rendering::Private::OpenGL;

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
	Window window1("window1");
	window1.Initialize();
	window1.SetMinimumSize({ 800, 600 });
	window1.SetMaximumSize({ 800, 600 });
	window1.SetSize({ 800, 600 });
	window1.SetIsVisible(true);

	Window window2("window2");
	window2.Initialize();
	window2.SetMinimumSize({ 800, 600 });
	window2.SetMaximumSize({ 800, 600 });
	window2.SetSize({ 800, 600 });
	window2.SetIsVisible(true);

	OpenGLDevice device;

	device.CreateContext(window1.GetHandle());
	device.CreateContext(window2.GetHandle());
	device.SetWindow(window1.GetHandle());

	device.Initialize();
	device.SetDebugCallback([](int32 ID, IDevice::DebugSources Source, cstr Message, IDevice::DebugTypes Type, IDevice::DebugSeverities Severity) {std::cout << Message << std::endl; });

	//device.SetWindow(window2.GetHandle());

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

	while (!window1.ShouldClose())
	{
		device.SetWindow(window1.GetHandle());
		{
			device.SetClearColor({ 0, 0, 0, 255 });
			device.Clear(IDevice::ClearFlags::ColorBuffer | IDevice::ClearFlags::DepthBuffer | IDevice::ClearFlags::StencilBuffer);

			device.BindProgram(programHandle);
			device.BindMesh(meshHandle);

			device.DrawIndexed(Mesh::SubMesh::PolygonTypes::Triangles, 6);

			device.SwapBuffers();
		}

		device.SetWindow(window2.GetHandle());
		{
			IDevice::State state = device.GetState();
			state.DepthTestFunction = IDevice::TestFunctions::Never;
			state.SetStencilTestFunction(IDevice::TestFunctions::Never, 0, 0);
			device.SetState(state);

			//device.SetClearColor({ 255, 255, 255, 255 });
			device.Clear(IDevice::ClearFlags::ColorBuffer | IDevice::ClearFlags::DepthBuffer | IDevice::ClearFlags::StencilBuffer);

			device.BindProgram(programHandle);
			device.BindMesh(meshHandle);

			//device.DrawIndexed(Mesh::SubMesh::PolygonTypes::Triangles, 6);

			device.SwapBuffers();
		}

		PlatformWindow::PollEvents();
	}
}


























































//#include <CoreSystem\Core.h>
//#include <MemoryManagement\Allocator\RootAllocator.h>
//#include <Rendering\RenderingManager.h>
//#include <ResourceSystem\ResourceManager.h>
//#include <MathContainers\MathContainers.h>
//#include <Rendering\Material.h>
//#include <Platform\PlatformFile.h>
//#include <GameObjectSystem\SceneManager.h>
//#include <Utility\Window.h>
//#include <Utility\FileSystem.h>
//#include <Containers\StringUtility.h>
//#include <InputSystem\InputManager.h>
//
//using namespace Engine::Common;
//using namespace Engine::Containers;
//using namespace Engine::CoreSystem;
//using namespace Engine::MemoryManagement::Allocator;
//using namespace Engine::Rendering;
//using namespace Engine::ResourceSystem;
//using namespace Engine::Platform;
//using namespace Engine::GameObjectSystem;
//using namespace Engine::Utility;
//using namespace Engine::FontSystem;
//using namespace Engine::InputSystem;
//
//const int WIDTH = 800;
//const int HEIGHT = 600;
//const float ASPECT_RATIO = (float)WIDTH / HEIGHT;
//
//class WindowListener : public Window::IListener
//{
//public:
//	WindowListener(Camera Camera) :
//		m_Camera(Camera)
//	{
//	}
//
//private:
//	void OnWindowPositioned(Window * Window) override
//	{
//	}
//
//	void OnWindowResized(Window *Window) override
//	{
//		m_Camera.SetAspectRatio(Window->GetClientSize().X / (float)Window->GetClientSize().Y);
//	}
//
//private:
//	Camera m_Camera;
//};
//
//void main()
//{
//	FileSystem::SetWorkingPath(L"D:\\Projects\\GameEngineAssets");
//
//	Core *core = Core::Create(RootAllocator::GetInstance());
//
//	Window* window = core->CreateWindow({ WIDTH, HEIGHT }, "Test Launcher");
//	Window *window1 = core->CreateWindow({ WIDTH, HEIGHT }, "Test Launcher1");
//
//	core->Initialize();
//
//	SceneManager *sceneMgr = SceneManager::GetInstance();
//	ResourceManager *resources = ResourceManager::GetInstance();
//	InputManager *input = InputManager::GetInstance();
//
//	TextureResource brickTex = resources->Load<Texture>("Brick.jpg");
//	ProgramResource shader = resources->Load<Program>("Shader.shader");
//	MeshResource sphereMesh = resources->Load(PrimitiveMeshTypes::Sphere);
//	MeshResource quadMesh = resources->Load(PrimitiveMeshTypes::Cube);
//	FontResource font = resources->Load<Font>("consola.font");
//	ProgramResource textShader = resources->Load<Program>("TextShader.shader");
//
//	Material mat;
//	mat.SetQueue(RenderQueues::Geometry);
//	Pass pass(*shader);
//	pass.SetTexture("diffuseTex", *brickTex);
//	mat.AddPass(pass);
//
//	Scene scene = sceneMgr->CreateScene();
//	sceneMgr->SetActiveScene(scene);
//
//	const int32 COUNT_X = 10;
//	const int32 COUNT_Y = 10;
//	GameObject gameObjects[COUNT_X * COUNT_Y];
//
//	for (int i = 0; i < COUNT_X; ++i)
//		for (int j = 0; j < COUNT_Y; ++j)
//		{
//			GameObject obj = gameObjects[i + (j * COUNT_X)] = scene.CreateRenderableGameObject();
//
//			Renderer renderer = obj.GetRenderer();
//
//			renderer.SetMesh(*sphereMesh);
//			renderer.SetMaterial(&mat);
//
//			Transform tr = obj.GetTransform();
//
//			Vector3F pos((-COUNT_X) + (i * 2), 0, j * -2);
//
//			tr.SetPosition(pos);
//			tr.SetRotation(Vector3F(rand() % 90, rand() % 90, rand() % 90));
//			tr.SetScale({ 2, 2,2 });
//		}
//
//	for (int i = 0; i < COUNT_X; ++i)
//		for (int j = 0; j < COUNT_Y; ++j)
//			for (int k = 0; k < 1; ++k)
//			{
//				GameObject lightObj = scene.CreateLightingGameObject();
//				{
//					lightObj.GetTransform().SetPosition({ (float32)(-COUNT_X + (i * 2)), 1, (float32)(j * -2) });
//					Light pointLight = lightObj.GetLight();
//					pointLight.SetType(LightTypes::Point);
//					pointLight.SetColor({ (uint8)(25 * i), (uint8)(25 * (COUNT_X - i)),(uint8)(25 * j) });
//					pointLight.SetRadius(3.0F);
//					pointLight.SetStrength(2.0F);
//
//
//					//lightObj.GetTransform().SetPosition({ (float32)(-COUNT_X + (i * 2)), 4, (float32)(j * -2) });
//					//lightObj.GetTransform().SetRotation({ 90, 0 ,0 });
//					//Light spotLight = lightObj.GetLight();
//					//spotLight.SetType(LightTypes::Spot);
//					//spotLight.SetColor({ (uint8)(25 * i), (uint8)(25 * (COUNT_X - i)),(uint8)(25 * j) });
//					//spotLight.SetRadius(2.0F);
//					//spotLight.SetInnerCutOff(12.5);
//					//spotLight.SetOuterCutOff(17.5);
//
//				}
//			}
//
//	Material textMat;
//	textMat.SetQueue(RenderQueues::HUD);
//	Pass textPass(*textShader);
//	auto st = textPass.GetRenderState();
//	st.CullMode = IDevice::CullModes::None;
//	st.DepthTestFunction = IDevice::TestFunctions::Never;
//	textPass.SetRenderState(st);
//	textMat.AddPass(textPass);
//
//	GameObject textObj = scene.CreateTextRenderableGameObject();
//	TextRenderer textRen = textObj.GetTextRenderer();
//	textRen.SetFont(*font);
//	textRen.SetMaterial(&textMat);
//	//textRen.SetRightToLeft(true);
//	//textRen.SetSize(0.2F);
//	//textRen.SetOutlineThicknes(0.5F);
//	//textObj.GetTransform().SetPosition({ -400, 350, 0 });
//
//	GameObject camObj = scene.CreateCameraGameObject();
//	Camera camera = camObj.GetCamera();
//	camObj.GetTransform().SetPosition({ 0, -5, 0 });
//
//	camera.SetAspectRatio(ASPECT_RATIO);
//	camera.SetFieldOfView(60);
//	camera.SetNearClipDistance(0.1F);
//	camera.SetFarClipDistance(100);
//
//	//GameObject amLightObj = scene.CreateLightingGameObject();
//	//Light amLight = amLightObj.GetLight();
//	//amLight.SetType(LightTypes::Ambient);
//	//amLight.SetStrength(1);
//	//amLight.SetColor({ 0, 255, 255 });
//
//	WindowListener windowListener(camera);
//	window->AddListener(&windowListener);
//
//	PlatformFile::Handle watcherHandle = PlatformFile::CreateWatcher(resources->GetAssetsPath().GetValue(), true);
//	PlatformFile::WatchInfo watchInfos[1024];
//
//	Vector3F cameraRot;
//
//	while (!window->ShouldClose())
//	{
//		uint32 len;
//		PlatformFile::RefreshWatcher(watcherHandle, true, PlatformFile::WatchNotifyFilter::FileRenamed | PlatformFile::WatchNotifyFilter::DirectoryRenamed | PlatformFile::WatchNotifyFilter::LastWriteTimeChanged, watchInfos, 1024, len);
//
//		if (len > 0)
//		{
//			WStringList files;
//
//			for (uint32 i = 0; i < len; ++i)
//			{
//				PlatformFile::WatchInfo &info = watchInfos[i];
//
//				if (info.Action != PlatformFile::WatchAction::Modified)
//					continue;
//
//				WString file(info.FileName, info.FileNameLength);
//
//				if (!files.Contains(file))
//					files.Add(file);
//			}
//
//			for each (auto &file in files)
//				resources->Reload(file);
//		}
//
//		core->Update();
//
//		textRen.SetText("FPS: " + StringUtility::ToString<char8>(core->GetFPS()));
//
//		auto mPos = input->GetMousePosition();
//		cameraRot.X = 30.0F * (mPos.Y / (float32)HEIGHT);
//		cameraRot.Y = 60.0F * (mPos.X / (float32)WIDTH);
//		//camObj.GetTransform().SetRotation(cameraRot);
//
//		auto camPos = camObj.GetTransform().GetPosition();
//		if (input->GetKey(KeyCodes::KeypadW))
//		{
//			camPos += camObj.GetTransform().GetForward() * 0.1F;
//		}
//		if (input->GetKey(KeyCodes::KeypadS))
//		{
//			camPos -= camObj.GetTransform().GetForward() * 0.1F;
//		}
//		//camObj.GetTransform().SetPosition(camPos);
//	}
//
//	ResourceManager::Destroy();
//	Core::Destroy();
//	SceneManager::Destroy();
//}