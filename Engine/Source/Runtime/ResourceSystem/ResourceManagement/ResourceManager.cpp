// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceManagement\ResourceManager.h>
#include <ResourceManagement\Private\BuiltInAssets.h>
#include <OBJAssetParser\OBJParser.h>
#include <RenderSystem\RenderManager.h>
#include <RenderSystem\PixelBuffer.h>
#include <RenderSystem\Sprite.h>
#include <RenderSystem\Program.h>
#include <RenderSystem\Mesh.h>
#include <FileUtility\FileSystem.h>
#include <FileUtility\Path.h>
#include <Common\CharacterUtility.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace Common;
	using namespace FileUtility;
	using namespace RenderSystem;
	using namespace ResourceAssetParser;

	namespace ResourceManagement
	{
		using namespace Private;

		cwstr ASSETS_DIRECTORY_NAME(L"Assets");
		cwstr LIBRARY_DIRECTORY_NAME(L"Library");

		cwstr BUILT_IN_ASSETS_DIRECTORY_PATH(L"../../Contents/BuiltIn");
		cwstr BUILT_IN_LIBRARY_DIRECTORY_PATH(L"../../Contents/BuiltIn/Library");

		Program* CreateProgram(ProgramInfo* ProgramInfo)
		{
#ifdef DEBUG_MODE
			ProgramInfo->DebugMode = true;
#endif
			return RenderManager::GetInstance()->GetDevice()->CreateProgram(ProgramInfo);
		}

		Mesh* CreateMesh(const SubMeshInfo* SubMeshInfo)
		{
			MeshInfo info(ResourceSystemAllocators::ResourceAllocator);
			info.SubMeshes.Add(ConstCast(RenderDevice::SubMeshInfo*, SubMeshInfo));

			return RenderManager::GetInstance()->GetDevice()->CreateMesh(&info);
		}

		SINGLETON_DEFINITION(ResourceManager)

			ResourceManager::ResourceManager(void) :
			ResourceHolder(Path::Combine(FileSystem::GetWorkingPath(), WString(ASSETS_DIRECTORY_NAME)), Path::Combine(FileSystem::GetWorkingPath(), WString(LIBRARY_DIRECTORY_NAME))),
			m_InternalResourceHolder(Path::Combine(FileSystem::GetExecutableDirectory(), WString(BUILT_IN_ASSETS_DIRECTORY_PATH)), Path::Combine(FileSystem::GetExecutableDirectory(), WString(BUILT_IN_LIBRARY_DIRECTORY_PATH)))
		{
			CreateDefaultResources();

			m_InternalResourceHolder.GetCompiler()->CompileResources();
		}

		ResourceManager::~ResourceManager(void)
		{
		}

		TextureResource* ResourceManager::GetWhiteTexture(void)
		{
			return GetLoaded<Texture>(BuiltInAssets::WHITE_TEXTURE_GUID);
		}

		SpriteResource* ResourceManager::GetWhiteSprite(void)
		{
			return GetLoaded<Sprite>(BuiltInAssets::WHITE_TEXTURE_GUID);
		}

		ProgramResource* ResourceManager::GetDefaultShader(void)
		{
			return m_InternalResourceHolder.Load<Program>("Programs/Default.program");
		}

		ProgramResource* ResourceManager::GetSpriteRendererShader(void)
		{
			return m_InternalResourceHolder.Load<Program>("Programs/Sprite.program");
		}

		MeshResource* ResourceManager::GetPrimitiveMesh(PrimitiveMeshTypes Type)
		{
			switch (Type)
			{
			case ResourceManager::PrimitiveMeshTypes::Quad:
				return m_InternalResourceHolder.Load<Mesh>("Meshes/Quad.obj");

			case ResourceManager::PrimitiveMeshTypes::Cube:
				return m_InternalResourceHolder.Load<Mesh>("Meshes/Cube.obj");

			case ResourceManager::PrimitiveMeshTypes::Sphere:
				return m_InternalResourceHolder.Load<Mesh>("Meshes/Sphere.obj");

			case ResourceManager::PrimitiveMeshTypes::Cone:
				return m_InternalResourceHolder.Load<Mesh>("Meshes/Cone.obj");
			}

			THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ResourceSystem);
		}

		void ResourceManager::CreateDefaultResources(void)
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
				AddFromMemory(BuiltInAssets::WHITE_TEXTURE_GUID, tex);
			}
		}
	}
}