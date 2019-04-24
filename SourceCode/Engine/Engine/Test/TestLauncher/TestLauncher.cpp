
#include <CoreSystem\Core.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Rendering\RenderingManager.h>
#include <ResourceSystem\ResourceManager.h>
#include <Containers\MathContainers.h>
#include <Rendering\Material.h>
#include <Platform\PlatformFile.h>
#include <GameObjectSystem\SceneManager.h>
#include <Utility\Window.h>
#include <Utility\FileSystem.h>
#include <FontSystem\FontGenerator.h>


using namespace Engine::Common;
using namespace Engine::Containers;
using namespace Engine::CoreSystem;
using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::Rendering;
using namespace Engine::ResourceSystem;
using namespace Engine::Platform;
using namespace Engine::GameObjectSystem;
using namespace Engine::Utility;
using namespace Engine::FontSystem;

const int WIDTH = 1024;
const int HEIGHT = 768;
const float ASPECT_RATIO = (float)WIDTH / HEIGHT;

class WindowListener : public Window::IListener
{
public:
	WindowListener(Camera Camera) :
		m_Camera(Camera)
	{
	}

private:
	void OnWindowResized(Window *Window) override
	{
		m_Camera.SetAspectRatio(Window->GetSize().X / (float)Window->GetSize().Y);
	}

private:
	Camera m_Camera;
};

void GenerateFont()
{
	const WString ttfPath = L"D:/calibri.ttf";
	uint64 fileSize = FileSystem::GetSize(ttfPath);
	{
		byte *fontBuffer = AllocateMemory(RootAllocator::GetInstance(), fileSize);
		FileSystem::ReadAllBytes(ttfPath, fontBuffer, fileSize);

		FontGenerator fontGenerator;
		fontGenerator.LoadFont(fontBuffer, fileSize);
		fontGenerator.Generate(L"D:/Projects/GameEngineAssets/Assets/calibri.font");

		DeallocateMemory(RootAllocator::GetInstance(), fontBuffer);
	}
}

void main()
{
	//GenerateFont();

	FileSystem::SetWorkingPath(L"D:\\Projects\\GameEngineAssets");

	Core *core = Core::Create(RootAllocator::GetInstance());

	Window *window = core->CreateWindow({ WIDTH, HEIGHT }, "Test Launcher");

	core->Initialize();

	SceneManager *sceneMgr = SceneManager::GetInstance();
	ResourceManager *resources = ResourceManager::GetInstance();

	TextureResource brickTex = resources->Load<Texture>("Brick.jpg");
	ProgramResource shader = resources->Load<Program>("Shader.shader");
	MeshResource sphereMesh = resources->Load(PrimitiveMeshTypes::Sphere);
	MeshResource quadMesh = resources->Load(PrimitiveMeshTypes::Cube);
	FontResource font = resources->Load<Font>("calibri.font");
	ProgramResource textShader = resources->Load<Program>("TextShader.shader");

	Material mat;
	mat.SetQueue(RenderQueues::Geometry);
	Pass pass(*shader);
	pass.SetTexture("diffuseTex", *brickTex);
	mat.AddPass(pass);

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

			renderer.SetMesh(*sphereMesh);
			renderer.SetMaterial(&mat);

			Transform tr = obj.GetTransform();

			Vector3F pos((-COUNT_X) + (i * 2), 0, j * -2);

			tr.SetPosition(pos);
			//tr.SetRotation(Vector3F(rand() % 90, rand() % 90, rand() % 90));
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
					pointLight.SetRadius(2.0F);

					//lightObj.GetTransform().SetPosition({ (float32)(-COUNT_X + (i * 2)), 4, (float32)(j * -2) });
					//lightObj.GetTransform().SetRotation({ 90, 0 ,0 });
					//Light spotLight = lightObj.GetLight();
					//spotLight.SetType(LightTypes::Spot);
					//spotLight.SetColor({ (uint8)(25 * i), (uint8)(25 * (COUNT_X - i)),(uint8)(25 * j) });
					//spotLight.SetRadius(2.0F);
					//spotLight.SetInnerCutOff(12.5);
					//spotLight.SetOuterCutOff(17.5);

				}
			}

	//GameObject obj = scene.CreateRenderableGameObject();
	//{
	//	Renderer renderer = obj.GetRenderer();
	//	renderer.SetMesh(*sphereMesh);
	//	renderer.SetMaterial(&mat);
	//	Transform tr = obj.GetTransform();
	//	tr.SetPosition({ 0, -5, 0 });
	//	tr.SetRotation({ 45, 0 , 0 });
	//	tr.SetScale({ 10, 10, 10 });

	//}

	//GameObject lightObj = scene.CreateLightingGameObject();
	//{
	//	lightObj.GetTransform().SetPosition({ 0, 2.9F, 0 });
	//	lightObj.GetTransform().SetRotation({ 90, 0 ,0 });
	//	Light spotLight = lightObj.GetLight();
	//	spotLight.SetType(LightTypes::Spot);
	//	spotLight.SetColor({0, 255, 0 });
	//	spotLight.SetRadius(3.0F);
	//	spotLight.SetInnerCutOff(30);
	//	spotLight.SetOuterCutOff(45);
	//}

	//GameObject pointLightObj = scene.CreateLightingGameObject();
	//{
	//	Light pointLight = pointLightObj.GetLight();
	//	pointLightObj.GetTransform().SetPosition({0, 1, -5 });

	//	pointLight.SetType(LightTypes::Point);
	//	pointLight.SetColor({ 255, 0, 0 });
	//	pointLight.SetRadius(5.0F);
	//	pointLight.SetStrength(5.0F);

	//}

	Material textMat;
	textMat.SetQueue(RenderQueues::HUD);
	Pass textPass(*textShader);
	auto st = textPass.GetRenderState();
	st.CullMode = IDevice::CullModes::None;
	st.DepthTestFunction = IDevice::TestFunctions::Never;
	textPass.SetRenderState(st);
	textMat.AddPass(pass);

	GameObject textObj = scene.CreateTextRenderableGameObject();
	TextRenderer textRen = textObj.GetTextRenderer();
	textRen.SetFont(*font);
	textRen.SetMaterial(&textMat);
	textRen.SetText(L"Delaram");

	GameObject camObj = scene.CreateCameraGameObject();
	Camera camera = camObj.GetCamera();
	camObj.GetTransform().SetPosition({ 0, -5, -15 });

	camera.SetAspectRatio(ASPECT_RATIO);
	camera.SetFieldOfView(60);
	camera.SetNearClipDistance(0.1F);
	camera.SetFarClipDistance(100);

	GameObject amLightObj = scene.CreateLightingGameObject();
	Light amLight = amLightObj.GetLight();
	amLight.SetType(LightTypes::Ambient);
	amLight.SetStrength(1);
	amLight.SetColor({ 50, 50, 50 });

	//GameObject dirLightObj1 = scene.CreateLightingGameObject();
	//{
	//	Light dirLight = dirLightObj1.GetLight();
	//	dirLight.SetType(LightTypes::Directional);
	//	dirLight.SetColor({ 255, 0, 0 });
	//	dirLight.SetStrength(1);
	//	dirLightObj1.GetTransform().SetRotation({ 45, 45, 0 });
	//}

	//GameObject dirLightObj2 = scene.CreateLightingGameObject();
	//{
	//	Light dirLight = dirLightObj2.GetLight();
	//	dirLight.SetType(LightTypes::Directional);
	//	dirLight.SetColor({ 128, 0, 128 });
	//	dirLight.SetStrength(2);
	//	dirLightObj2.GetTransform().SetRotation({ 45, 45, 0 });
	//}

	WindowListener windowListener(camera);
	window->AddListener(&windowListener);


	PlatformFile::Handle watcherHandle = PlatformFile::CreateWatcher(resources->GetAssetsPath().GetValue(), true);
	PlatformFile::WatchInfo watchInfos[1024];

	int32 rot = 0;

	while (!window->ShouldClose())
	{
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

		rot += 2;

		//for (int i = 0; i < GAME_OBJECT_COUNT_X * GAME_OBJECT_COUNT_Y; ++i)
		//{
		//	Transform tr = gameObjects[i].GetTransform();

		//	//tr.SetRotation(Vector3F(rot, rot, rot));
		//}

		core->Update();
	}

	ResourceManager::Destroy();
	Core::Destroy();
	SceneManager::Destroy();
}