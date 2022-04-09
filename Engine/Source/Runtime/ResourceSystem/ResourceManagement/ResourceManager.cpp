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
			return GetLoaded<Program>(BuiltInAssets::DEFAULT_PROGRAM_GUID);
		}

		ProgramResource* ResourceManager::GetSpriteRendererShader(void)
		{
			return m_InternalResourceHolder.Load<Program>("Programs/Sprite.program");
		}

		MeshResource* ResourceManager::GetPrimitiveMesh(PrimitiveMeshTypes Type)
		{
			const GUID* guid;
			switch (Type)
			{
			case ResourceManager::PrimitiveMeshTypes::Quad:
				guid = &BuiltInAssets::QUAD_MESH_GUID;
				break;

			case ResourceManager::PrimitiveMeshTypes::Cube:
				guid = &BuiltInAssets::CUBE_MESH_GUID;
				break;

			case ResourceManager::PrimitiveMeshTypes::Sphere:
				guid = &BuiltInAssets::SPHERE_MESH_GUID;
				break;

			case ResourceManager::PrimitiveMeshTypes::Cone:
				guid = &BuiltInAssets::CONE_MESH_GUID;
				break;

			}

			return GetLoaded<Mesh>(*guid);
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

			// Default Program
			{
				ProgramInfo info;
				info.Source = BuiltInAssets::DEFAULT_PROGRAM_SOURCE;
				Program* program = CreateProgram(&info);
				program->SetName("DefaultProgram");
				AddFromMemory(BuiltInAssets::DEFAULT_PROGRAM_GUID, program);
			}

			// Sprite Program
			//{
			//	ProgramInfo info;
			//	info.Source = BuiltInAssets::SPRITE_RENDERER_PROGRAM_SOURCE;
			//	Program* program = CreateProgram(&info);
			//	program->SetName("SpriteProgram");
			//	AddFromMemory(BuiltInAssets::SPRITE_RENDERER_PROGRAM_GUID, program);
			//}

			// Quad Mesh
			{
				SubMeshInfo* info = ReinterpretCast(SubMeshInfo*, AllocateMemory(ResourceSystemAllocators::ResourceAllocator, sizeof(SubMeshInfo)));
				Construct(info, ResourceSystemAllocators::ResourceAllocator);

				info->Vertices.Add({ Vector3F(-0.5F, 0.5F, 0), Vector2F(0, 1) });
				info->Vertices.Add({ Vector3F(-0.5F, -0.5F, 0), Vector2F(0, 0) });
				info->Vertices.Add({ Vector3F(0.5F, 0.5F, 0), Vector2F(1, 1) });
				info->Vertices.Add({ Vector3F(0.5F, -0.5F, 0), Vector2F(1, 0) });
				info->Indices.Add(0);
				info->Indices.Add(1);
				info->Indices.Add(2);
				info->Indices.Add(2);
				info->Indices.Add(1);
				info->Indices.Add(3);
				info->Type = PolygonTypes::Triangles;
				info->Layout = VertexLayouts::Position | VertexLayouts::TexCoord;

				Mesh* mesh = CreateMesh(info);
				mesh->SetName("QuadMesh");
				AddFromMemory(BuiltInAssets::QUAD_MESH_GUID, mesh);
			}

			// Cube Mesh
			{
				SubMeshInfo* info = ReinterpretCast(SubMeshInfo*, AllocateMemory(ResourceSystemAllocators::ResourceAllocator, sizeof(SubMeshInfo)));
				Construct(info, ResourceSystemAllocators::ResourceAllocator);

				OBJParser::Parse(ByteBuffer(ReinterpretCast(byte*, ConstCast(char8*, BuiltInAssets::CUBE_MESH_DATA)), CharacterUtility::GetLength(BuiltInAssets::CUBE_MESH_DATA)), *info);

				Mesh* mesh = CreateMesh(info);
				mesh->SetName("CubeMesh");
				AddFromMemory(BuiltInAssets::CUBE_MESH_GUID, mesh);
			}

			// Sphere Mesh
			{
				SubMeshInfo* info = ReinterpretCast(SubMeshInfo*, AllocateMemory(ResourceSystemAllocators::ResourceAllocator, sizeof(SubMeshInfo)));
				Construct(info, ResourceSystemAllocators::ResourceAllocator);

				OBJParser::Parse(ByteBuffer(ReinterpretCast(byte*, ConstCast(char8*, BuiltInAssets::SPHERE_MESH_DATA)), CharacterUtility::GetLength(BuiltInAssets::SPHERE_MESH_DATA)), *info);

				Mesh* mesh = CreateMesh(info);
				mesh->SetName("SphereMesh");
				AddFromMemory(BuiltInAssets::SPHERE_MESH_GUID, mesh);
			}

			// Cone Mesh
			{
				SubMeshInfo* info = ReinterpretCast(SubMeshInfo*, AllocateMemory(ResourceSystemAllocators::ResourceAllocator, sizeof(SubMeshInfo)));
				Construct(info, ResourceSystemAllocators::ResourceAllocator);

				OBJParser::Parse(ByteBuffer(ReinterpretCast(byte*, ConstCast(char8*, BuiltInAssets::CONE_MESH_DATA)), CharacterUtility::GetLength(BuiltInAssets::CONE_MESH_DATA)), *info);

				Mesh* mesh = CreateMesh(info);
				mesh->SetName("ConeMesh");
				AddFromMemory(BuiltInAssets::CONE_MESH_GUID, mesh);
			}
		}
	}
}