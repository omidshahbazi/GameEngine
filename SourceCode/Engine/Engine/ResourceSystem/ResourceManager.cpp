// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\ResourceManager.h>
#include <ResourceSystem\Private\BuiltInAssets.h>
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

		SINGLETON_DEFINITION(ResourceManager)

			ResourceManager::ResourceManager(void) :
			ResourceHolder(Path::Combine(FileSystem::GetWorkingPath(), ASSETS_DIRECTORY_NAME), Path::Combine(FileSystem::GetWorkingPath(), LIBRARY_DIRECTORY_NAME))
		{
			CreateDefaultResources();
		}

		ResourceManager::~ResourceManager(void)
		{
		}

		TextureResource ResourceManager::GetWhiteTexture(void)
		{
			return ReinterpretCast(TextureHandle*, GetFromLoaded(BuiltInAssets::WHITE_TEXTURE_NAME));
		}

		ShaderResource ResourceManager::GetDefaultShader(void)
		{
			return Load<Shader>(BuiltInAssets::DEFAULT_SHADER_NAME);
		}

		ShaderResource ResourceManager::GetSpriteRendererShader(void)
		{
			return Load<Shader>(BuiltInAssets::SPRITE_RENDERER_SHADER_NAME);
		}

		void ResourceManager::CreateDefaultResources(void)
		{
			Texture* tex = RenderingManager::GetInstance()->GetActiveDevice()->CreateTexture2D(Vector2I::One, Texture::Formats::RGBA8);
			auto* buf = tex->GetBuffer();
			buf->Lock(GPUBuffer::Access::WriteOnly);
			buf->GetColorUI8Pixel() = ColorUI8::White;
			buf->Unlock();
			AddToLoaded(BuiltInAssets::WHITE_TEXTURE_NAME, AllocateResourceHandle(tex));

			LoadShader(BuiltInAssets::DEFAULT_SHADER_NAME, BuiltInAssets::DEFAULT_SHADER_SOURCE);
			LoadShader(BuiltInAssets::SPRITE_RENDERER_SHADER_NAME, BuiltInAssets::SPRITE_RENDERER_SHADER_SOURCE);
		}
	}
}