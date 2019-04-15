
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
#include <FreeType\include\ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

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


	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		return;
	}

	FT_Face face;
	if (FT_New_Face(ft, "D:/Projects/GameEngineAssets/Assets/calibri.ttf", 0, &face))
	{
		return;
	}

	const float DPI = 300;
	if (FT_Set_Char_Size(face, 0, 0, DPI, DPI))
	{
		return;
	}

	if (FT_Set_Pixel_Sizes(face, 0, 30))
	{
		return;
	}

	if (FT_Load_Char(face, 'd', FT_LOAD_RENDER))
		return;

	if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO))
		return;

	//FT_Glyph glyph;
	//if (FT_Get_Glyph(face->glyph, &glyph))
	//	return;
	//if (FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_MONO, nullptr, true))
	//	return;

	for (int y = 0; y < face->glyph->bitmap.rows; ++y)
	{
		for (int x = 0; x < face->glyph->bitmap.width; ++x)
		{
			uint8 value = 0;

			value |= face->glyph->bitmap.buffer[x + (y * face->glyph->bitmap.pitch)];

			std::cout << (value < 128 ? ' ' : '*');
		}

		std::cout << std::endl;
	}






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


	float32 fps = 0;
	uint32 frameCount = 0;
	uint64 nextCheckTime = HighResolutionTime::GetTime().GetMilliseconds() + 1000;

	while (!window.ShouldClose())
	{
		PlatformWindow::PollEvents();



		device->BeginRender();

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