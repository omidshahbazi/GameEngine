// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\ResourceManager.h>
#include <ResourceSystem\Private\BuiltInAssets.h>
#include <ResourceAssetParser\OBJParser.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\PixelBuffer.h>
#include <Rendering\Sprite.h>
#include <Utility\FileSystem.h>
#include <Utility\Path.h>
#include <Common\CharacterUtility.h>

namespace Engine
{
	using namespace Common;
	using namespace Utility;
	using namespace ResourceAssetParser;

	namespace ResourceSystem
	{
		using namespace Private;

		cwstr ASSETS_DIRECTORY_NAME(L"Assets");
		cwstr LIBRARY_DIRECTORY_NAME(L"Library");

		cwstr INTERNAL_ASSETS_DIRECTORY_PATH(L"../Contents/Resources");
		cwstr INTERNAL_LIBRARY_DIRECTORY_PATH(L"../Contents/Resources/Library");

		Program* CreateProgram(const ProgramInfo* ProgramInfo)
		{
			return RenderingManager::GetInstance()->GetActiveDevice()->CreateProgram(ProgramInfo);
		}

		Mesh* CreateMesh(const SubMeshInfo* SubMeshInfo)
		{
			MeshInfo info(ResourceSystemAllocators::ResourceAllocator);
			info.SubMeshes.Add(ConstCast(Rendering::SubMeshInfo*, SubMeshInfo));

			return RenderingManager::GetInstance()->GetActiveDevice()->CreateMesh(&info);
		}

		SINGLETON_DEFINITION(ResourceManager)

			ResourceManager::ResourceManager(void) :
			ResourceHolder(Path::Combine(FileSystem::GetWorkingPath(), WString(ASSETS_DIRECTORY_NAME)), Path::Combine(FileSystem::GetWorkingPath(), WString(LIBRARY_DIRECTORY_NAME))),
			m_InternalResourceHolder(Path::Combine(FileSystem::GetExecutableDirectory(), WString(INTERNAL_ASSETS_DIRECTORY_PATH)), Path::Combine(FileSystem::GetExecutableDirectory(), WString(INTERNAL_LIBRARY_DIRECTORY_PATH)))
		{
			CreateDefaultResources();

			m_InternalResourceHolder.GetCompiler()->CompileResources();
		}

		ResourceManager::~ResourceManager(void)
		{
		}

		TextureResource* ResourceManager::GetWhiteTexture(void)
		{
			return GetLoaded<Texture>(BuiltInAssets::WHITE_TEXTURE_NAME);
		}

		SpriteResource* ResourceManager::GetWhiteSprite(void)
		{
			return GetLoaded<Sprite>(BuiltInAssets::WHITE_TEXTURE_NAME);
		}

		ProgramResource* ResourceManager::GetDefaultShader(void)
		{
			return GetLoaded<Program>(BuiltInAssets::DEFAULT_PROGRAM_NAME);
		}

		ProgramResource* ResourceManager::GetSpriteRendererShader(void)
		{
			return GetLoaded<Program>(BuiltInAssets::SPRITE_RENDERER_PROGRAM_NAME);
		}

		MeshResource* ResourceManager::GetPrimitiveMesh(PrimitiveMeshTypes Type)
		{
			String name;
			switch (Type)
			{
			case ResourceManager::PrimitiveMeshTypes::Quad:
				name = BuiltInAssets::QUAD_MESH_NAME;
				break;

			case ResourceManager::PrimitiveMeshTypes::Cube:
				name = BuiltInAssets::CUBE_MESH_NAME;
				break;

			case ResourceManager::PrimitiveMeshTypes::Sphere:
				name = BuiltInAssets::SPHERE_MESH_NAME;
				break;

			case ResourceManager::PrimitiveMeshTypes::Cone:
				name = BuiltInAssets::CONE_MESH_NAME;
				break;

			}

			return GetLoaded<Mesh>(name);
		}

		void ResourceManager::CreateDefaultResources(void)
		{
			// White Texture
			{
				TextureInfo info;
				info.Dimension = Vector2I::One;
				info.Format = Formats::RGBA8;
				Texture* tex = RenderingManager::GetInstance()->GetActiveDevice()->CreateTexture(&info);
				auto* buf = tex->GetBuffer();
				buf->Lock(GPUBuffer::Access::WriteOnly);
				buf->GetColorUI8Pixel() = ColorUI8::White;
				buf->Unlock();
				AddFromMemory(BuiltInAssets::WHITE_TEXTURE_NAME, tex);
			}

			// Default Program
			{
				ProgramInfo info;
				info.Source = BuiltInAssets::DEFAULT_PROGRAM_SOURCE;
				AddFromMemory(BuiltInAssets::DEFAULT_PROGRAM_NAME, CreateProgram(&info));
			}

			// Sprite Program
			{
				ProgramInfo info;
				info.Source = BuiltInAssets::SPRITE_RENDERER_PROGRAM_SOURCE;
				AddFromMemory(BuiltInAssets::SPRITE_RENDERER_PROGRAM_NAME, CreateProgram(&info));
			}

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
				info->Type = SubMesh::PolygonTypes::Triangles;
				info->Layout = SubMesh::VertexLayouts::Position | SubMesh::VertexLayouts::TexCoord;

				AddFromMemory(BuiltInAssets::QUAD_MESH_NAME, CreateMesh(info));
			}

			// Cube Mesh
			{
				SubMeshInfo* info = ReinterpretCast(SubMeshInfo*, AllocateMemory(ResourceSystemAllocators::ResourceAllocator, sizeof(SubMeshInfo)));
				Construct(info, ResourceSystemAllocators::ResourceAllocator);

				OBJParser::Parse(ByteBuffer(ReinterpretCast(byte*, ConstCast(char8*, BuiltInAssets::CUBE_MESH_DATA)), CharacterUtility::GetLength(BuiltInAssets::CUBE_MESH_DATA)), *info);
				AddFromMemory(BuiltInAssets::CUBE_MESH_NAME, CreateMesh(info));
			}

			// Sphere Mesh
			{
				SubMeshInfo* info = ReinterpretCast(SubMeshInfo*, AllocateMemory(ResourceSystemAllocators::ResourceAllocator, sizeof(SubMeshInfo)));
				Construct(info, ResourceSystemAllocators::ResourceAllocator);

				OBJParser::Parse(ByteBuffer(ReinterpretCast(byte*, ConstCast(char8*, BuiltInAssets::SPHERE_MESH_DATA)), CharacterUtility::GetLength(BuiltInAssets::SPHERE_MESH_DATA)), *info);
				AddFromMemory(BuiltInAssets::SPHERE_MESH_NAME, CreateMesh(info));
			}

			// Cone Mesh
			{
				SubMeshInfo* info = ReinterpretCast(SubMeshInfo*, AllocateMemory(ResourceSystemAllocators::ResourceAllocator, sizeof(SubMeshInfo)));
				Construct(info, ResourceSystemAllocators::ResourceAllocator);

				OBJParser::Parse(ByteBuffer(ReinterpretCast(byte*, ConstCast(char8*, BuiltInAssets::CONE_MESH_DATA)), CharacterUtility::GetLength(BuiltInAssets::CONE_MESH_DATA)), *info);
				AddFromMemory(BuiltInAssets::CONE_MESH_NAME, CreateMesh(info));
			}
		}
	}
}