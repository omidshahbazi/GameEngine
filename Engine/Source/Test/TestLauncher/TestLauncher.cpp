#include <CoreSystem\Core.h>
#include <MemoryManagement\Allocator\Initializer.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Rendering\RenderingManager.h>
#include <ResourceCommon\Resource.h>
#include <ResourceSystem\ResourceManager.h>
#include <MathContainers\MathContainers.h>
#include <Rendering\Material.h>
#include <Platform\PlatformFile.h>
#include <GameObjectSystem\SceneManager.h>
#include <WindowUtility\Window.h>
#include <FileUtility\FileSystem.h>
#include <Containers\StringUtility.h>
#include <InputSystem\InputManager.h>
#include <Mathematics\Math.h>

using namespace Engine::Common;
using namespace Engine::Containers;
using namespace Engine::MathContainers;
using namespace Engine::CoreSystem;
using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::Rendering;
using namespace Engine::ResourceCommon;
using namespace Engine::ResourceSystem;
using namespace Engine::Platform;
using namespace Engine::GameObjectSystem;
using namespace Engine::WindowUtility;
using namespace Engine::FileUtility;
using namespace Engine::FontSystem;
using namespace Engine::InputSystem;
using namespace Engine::Mathematics;

const int WIDTH = 800;
const int HEIGHT = 600;
const float ASPECT_RATIO = (float)WIDTH / HEIGHT;

void main()
{
	Initializer::Create();
	Initializer::GetInstance()->Initialize(GigaByte * 4, L"../Alllocators.data");

	Core* core = Core::Create(RootAllocator::GetInstance());

	Core::Info info;
	info.WorkingPath = L"D:\\Projects\\GameEngineAssets";

	for (int i = 0; i < 2; ++i)
	{
		Window* window = core->CreateWindow({ WIDTH, HEIGHT }, "Test Launcher");
		//Window *window1 = core->CreateWindow({ WIDTH, HEIGHT }, "Test Launcher1");

		core->Initialize(&info);

		SceneManager* sceneMgr = SceneManager::GetInstance();
		ResourceManager* resources = ResourceManager::GetInstance();
		InputManager* input = InputManager::GetInstance();

		TextureResource* brickTex = resources->Load<Texture>("Brick.jpg");
		ProgramResource* shader = resources->Load<Program>("Shader.program");
		MeshResource* sphereMesh = resources->GetPrimitiveMesh(ResourceManager::PrimitiveMeshTypes::Sphere);
		MeshResource* quadMesh = resources->GetPrimitiveMesh(ResourceManager::PrimitiveMeshTypes::Cube);
		FontResource* font = resources->Load<Font>("calibri.ttf");
		ProgramResource* textShader = resources->Load<Program>("TextShader.program");
		
		_sleep(5000);

		Material mat;
		Pass pass(shader);
		pass.SetQueue(RenderQueues::Geometry);
		pass.SetTexture("DiffuseTexture", brickTex);
		mat.AddPass(pass);
		MaterialResource matRes(&mat);

		Scene scene = sceneMgr->CreateScene();
		sceneMgr->SetActiveScene(scene);

		const int32 COUNT_X = 10;
		const int32 COUNT_Y = 10;
		GameObject gameObjects[COUNT_X * COUNT_Y];

		for (int i = 0; i < COUNT_X; ++i)
			for (int j = 0; j < COUNT_Y; ++j)
			{
				GameObject obj = gameObjects[i + (j * COUNT_X)] = scene.CreateRenderableGameObject();

				Renderer renderer = obj.GetRenderer();

				renderer.SetMesh(sphereMesh);
				renderer.SetMaterial(&matRes);

				Transform tr = obj.GetTransform();

				Vector3F pos((-COUNT_X) + (i * 2), 0, j * -2);

				tr.SetPosition(pos);
				tr.SetRotation(Vector3F(rand() % 90, rand() % 90, rand() % 90) * Math::DEGREES_TO_RADIANS);
				tr.SetScale({ 2, 2,2 });
			}

		for (int i = 0; i < COUNT_X; ++i)
			for (int j = 0; j < COUNT_Y; ++j)
				for (int k = 0; k < 1; ++k)
				{
					GameObject lightObj = scene.CreateLightingGameObject();
					{
						lightObj.GetTransform().SetPosition({ (float32)(-COUNT_X + (i * 2)), 1, (float32)(j * -2) });
						Light pointLight = lightObj.GetLight();
						pointLight.SetType(LightTypes::Point);
						pointLight.SetColor({ (uint8)(25 * i), (uint8)(25 * (COUNT_X - i)),(uint8)(25 * j) });
						pointLight.SetRadius(3.0F);
						pointLight.SetStrength(2.0F);


						//lightObj.GetTransform().SetPosition({ (float32)(-COUNT_X + (i * 2)), 4, (float32)(j * -2) });
						//lightObj.GetTransform().SetRotation({ 90, 0 ,0 });
						//Light spotLight = lightObj.GetLight();
						//spotLight.SetType(LightTypes::Spot);
						//spotLight.SetColor({ (uint8)(25 * i), (uint8)(25 * (COUNT_X - i)),(uint8)(25 * j) });
						//spotLight.SetRadius(2.0F);
						//spotLight.SetInnerCutOff(12.5 * Math::DEGREES_TO_RADIANS);
						//spotLight.SetOuterCutOff(17.5 * Math::DEGREES_TO_RADIANS);

					}
				}

		Material textMat;
		Pass textPass(textShader);
		textPass.SetQueue(RenderQueues::HUD);
		auto st = textPass.GetRenderState();
		st.CullMode = IDevice::CullModes::None;
		st.DepthTestFunction = IDevice::TestFunctions::Never;
		textPass.SetRenderState(st);
		textMat.AddPass(textPass);
		MaterialResource textMatRes(&textMat);

		GameObject textObj = scene.CreateTextRenderableGameObject();
		TextRenderer textRen = textObj.GetTextRenderer();
		textRen.SetFont(font);
		textRen.SetMaterial(&textMatRes);
		//textRen.SetRightToLeft(true);
		//textRen.SetSize(0.2F);
		//textRen.SetOutlineThicknes(0.5F);
		//textObj.GetTransform().SetPosition({ -400, 350, 0 });

		GameObject camObj = scene.CreateCameraGameObject();
		Camera camera = camObj.GetCamera();
		camObj.GetTransform().SetPosition({ 0, -5, -10 });

		camera.SetAspectRatio(ASPECT_RATIO);
		camera.SetFieldOfView(60 * Math::DEGREES_TO_RADIANS);
		camera.SetNearClipDistance(0.1F);
		camera.SetFarClipDistance(1000);

		//GameObject amLightObj = scene.CreateLightingGameObject();
		//Light amLight = amLightObj.GetLight();
		//amLight.SetType(LightTypes::Ambient);
		//amLight.SetStrength(1);
		//amLight.SetColor({ 0, 255, 255 });

		auto windowResizedHandler = DECLARE_LAMBDA_EVENT_LISTENER(Window::SizeChangedEventHandler, [&](Window* Window)
			{
				camera.SetAspectRatio(Window->GetClientSize().X / (float)Window->GetClientSize().Y);
			});

		window->OnSizeChangedEvent += windowResizedHandler;

		PlatformFile::Handle watcherHandle = PlatformFile::CreateWatcher(resources->GetCompiler()->GetResourcesPath().GetValue(), true);
		PlatformFile::WatchInfo watchInfos[1024];

		Vector3F cameraRot;

		while (!window->IsClosed())
		{
			uint32 len;
			PlatformFile::RefreshWatcher(watcherHandle, true, PlatformFile::WatchNotifyFilter::FileRenamed | PlatformFile::WatchNotifyFilter::DirectoryRenamed | PlatformFile::WatchNotifyFilter::LastWriteTimeChanged, watchInfos, 1024, len);

			if (len > 0)
			{
				WStringList files;

				for (uint32 i = 0; i < len; ++i)
				{
					PlatformFile::WatchInfo& info = watchInfos[i];

					if (info.Action != PlatformFile::WatchAction::Modified)
						continue;

					WString file(info.FileName, info.FileNameLength);

					if (!files.Contains(file))
						files.Add(file);
				}

				//for each (auto & file in files)
				//	resources->Reload(file);
			}

			core->Update();

			textRen.SetText("FPS: " + StringUtility::ToString<char8>(core->GetFPS()));

			auto mPos = input->GetMousePosition();
			cameraRot.X = 30.0F * (mPos.Y / (float32)HEIGHT);
			cameraRot.Y = 60.0F * (mPos.X / (float32)WIDTH);
			//camObj.GetTransform().SetRotation(cameraRot);

			auto camPos = camObj.GetTransform().GetPosition();
			if (input->GetKey(KeyCodes::KeypadW))
			{
				camPos += camObj.GetTransform().GetForward() * 0.1F;
			}
			if (input->GetKey(KeyCodes::KeypadS))
			{
				camPos -= camObj.GetTransform().GetForward() * 0.1F;
			}
			//camObj.GetTransform().SetPosition(camPos);
		}

		core->DeInitialize();
	}

	Core::Destroy();
}