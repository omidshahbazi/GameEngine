// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <BuiltInResources\BuiltInResourceManager.h>
#include <RenderSystem\RenderManager.h>
#include <RenderSystem\PixelBuffer.h>
#include <RenderSystem\Sprite.h>
#include <RenderSystem\Program.h>
#include <RenderSystem\Mesh.h>
#include <FileUtility\FileSystem.h>
#include <FileUtility\Path.h>

namespace Engine
{
	using namespace Common;
	using namespace FileUtility;
	using namespace RenderSystem;

	namespace BuiltInResources
	{
		cwstr ASSETS_DIRECTORY_PATH(L"../../Contents/BuiltIn");
		cwstr LIBRARY_DIRECTORY_PATH(L"../../Contents/BuiltIn/Library");

		const GUID WHITE_TEXTURE_GUID = GUID::Create();

		SINGLETON_DEFINITION(BuiltInResourceManager)

			BuiltInResourceManager::BuiltInResourceManager(void) :
			ResourceHolder(Path::Combine(FileSystem::GetExecutableDirectory(), WString(ASSETS_DIRECTORY_PATH)), Path::Combine(FileSystem::GetExecutableDirectory(), WString(LIBRARY_DIRECTORY_PATH)))
		{
			CreateDefaultResources();

			GetCompiler()->CompileResources();
		}

		TextureResource* BuiltInResourceManager::GetWhiteTexture(void)
		{
			return GetLoaded<Texture>(WHITE_TEXTURE_GUID);
		}

		SpriteResource* BuiltInResourceManager::GetWhiteSprite(void)
		{
			return GetLoaded<Sprite>(WHITE_TEXTURE_GUID);
		}

		ProgramResource* BuiltInResourceManager::GetDefaultShader(void)
		{
			return Load<Program>("Programs/Default.program");
		}

		ProgramResource* BuiltInResourceManager::GetSpriteRendererShader(void)
		{
			return Load<Program>("Programs/Sprite.program");
		}

		MeshResource* BuiltInResourceManager::GetQuad(void)
		{
			return Load<Mesh>("Meshes/Quad.obj");
		}

		MeshResource* BuiltInResourceManager::GetCube(void)
		{
			return Load<Mesh>("Meshes/Cube.obj");
		}

		MeshResource* BuiltInResourceManager::GetSphere(void)
		{
			return Load<Mesh>("Meshes/Sphere.obj");
		}

		MeshResource* BuiltInResourceManager::GetCone(void)
		{
			return Load<Mesh>("Meshes/Cone.obj");
		}

		void BuiltInResourceManager::CreateDefaultResources(void)
		{
			// White Texture
			{
				TextureInfo info;
				info.Dimension = Vector2I::One;
				info.Format = Formats::RGBA8;
				Texture* tex = RenderManager::GetInstance()->GetDevice()->CreateTexture(&info);
				tex->SetName("WhiteTexure");
				auto* buf = tex->GetBuffer();
				buf->Lock(GPUBufferAccess::WriteOnly);
				buf->GetColorUI8Pixel() = ColorUI8::White;
				buf->Unlock();
				AddFromMemory(WHITE_TEXTURE_GUID, tex);
			}
		}
	}
}