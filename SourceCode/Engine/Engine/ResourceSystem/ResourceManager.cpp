// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\ResourceManager.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\PixelBuffer.h>
#include <Utility\FileSystem.h>
#include <Utility\Path.h>

namespace Engine
{
	using namespace Utility;

	namespace ResourceSystem
	{
		using namespace Private;

		const WString ASSETS_DIRECTORY_NAME(L"Assets");
		const WString LIBRARY_DIRECTORY_NAME(L"Library");

		const String DEFAULT_SHADER_NAME("Default.shader");
		String DEFAULT_SHADER_SOURCE("float3 pos : POSITION;const matrix4 _MVP;float4 VertexMain(){return _MVP * float4(pos, 1);}float4 FragmentMain(){return float4(1, 0, 1, 1);}");

		const WString WHITE_TEXTURE_NAME(L"White.tex");

		SINGLETON_DEFINITION(ResourceManager)

			ResourceManager::ResourceManager(void) :
			ResourceHolder(Path::Combine(FileSystem::GetWorkingPath(), ASSETS_DIRECTORY_NAME), Path::Combine(FileSystem::GetWorkingPath(), LIBRARY_DIRECTORY_NAME))
		{
			ResourceFactory::Create(&ResourceSystemAllocators::ResourceAllocator);

			CreateDefaultResources();
		}

		ResourceManager::~ResourceManager(void)
		{
		}

		TextureResource ResourceManager::GetWhiteTexture(void)
		{
			return ReinterpretCast(TextureHandle*, GetFromLoaded(WHITE_TEXTURE_NAME));
		}

		ProgramResource ResourceManager::GetDefaultProgram(void)
		{
			return Load<Program>(DEFAULT_SHADER_NAME);
		}

		void ResourceManager::CreateDefaultResources(void)
		{
			//Texture* tex = RenderingManager::GetInstance()->GetActiveDevice()->CreateTexture2D(1, 1, Texture::Formats::RGBA8);
			//auto* buf = tex->GetBuffer();
			//buf->Lock(IDevice::BufferAccess::WriteOnly);
			//buf->GetColorUI8Pixel() = ColorUI8::White;
			//buf->Unlock();
			//AddToLoaded(WHITE_TEXTURE_NAME, AllocateResourceHandle(tex));

			LoadProgram(DEFAULT_SHADER_NAME, DEFAULT_SHADER_SOURCE);
		}
	}
}