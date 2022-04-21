// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceManagement\ResourceManager.h>
#include <BuiltInResources\BuiltInResourceManager.h>
#include <RenderSystem\RenderManager.h>
#include <RenderSystem\PixelBuffer.h>
#include <RenderSystem\Sprite.h>
#include <RenderSystem\Program.h>
#include <RenderSystem\Mesh.h>
#include <FileUtility\FileSystem.h>
#include <FileUtility\Path.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace Common;
	using namespace FileUtility;
	using namespace RenderSystem;
	using namespace BuiltInResources;

	namespace ResourceManagement
	{
		cwstr ASSETS_DIRECTORY_NAME(L"Assets");
		cwstr LIBRARY_DIRECTORY_NAME(L"Library");

		SINGLETON_DEFINITION(ResourceManager);

		ResourceManager::ResourceManager(void) :
			ResourceHolder(Path::Combine(FileSystem::GetWorkingPath(), WString(LIBRARY_DIRECTORY_NAME)), Path::Combine(FileSystem::GetWorkingPath(), WString(ASSETS_DIRECTORY_NAME)), true)
		{
			BuiltInResourceManager::Create(ResourceSystemAllocators::ResourceAllocator);
		}

		ResourceManager::~ResourceManager(void)
		{
			BuiltInResourceManager::Destroy();
		}

		TextureResource* ResourceManager::GetWhiteTexture(void)
		{
			return BuiltInResourceManager::GetInstance()->GetWhiteTexture();
		}

		SpriteResource* ResourceManager::GetWhiteSprite(void)
		{
			return BuiltInResourceManager::GetInstance()->GetWhiteSprite();
		}

		ProgramResource* ResourceManager::GetDefaultShader(void)
		{
			return BuiltInResourceManager::GetInstance()->GetDefaultShader();
		}

		ProgramResource* ResourceManager::GetSpriteRendererShader(void)
		{
			return BuiltInResourceManager::GetInstance()->GetSpriteRendererShader();
		}

		MeshResource* ResourceManager::GetPrimitiveMesh(PrimitiveMeshTypes Type)
		{
			switch (Type)
			{
			case ResourceManager::PrimitiveMeshTypes::Quad:
				return BuiltInResourceManager::GetInstance()->GetQuad();

			case ResourceManager::PrimitiveMeshTypes::Cube:
				return BuiltInResourceManager::GetInstance()->GetCube();

			case ResourceManager::PrimitiveMeshTypes::Sphere:
				return BuiltInResourceManager::GetInstance()->GetSphere();

			case ResourceManager::PrimitiveMeshTypes::Cone:
				return BuiltInResourceManager::GetInstance()->GetCone();
			}

			THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ResourceSystem);
		}
	}
}