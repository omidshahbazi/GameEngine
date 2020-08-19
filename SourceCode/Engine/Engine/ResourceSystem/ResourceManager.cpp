// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\ResourceManager.h>
#include <ResourceSystem\Private\BuiltInAssets.h>
#include <ResourceAssetParser\OBJParser.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\PixelBuffer.h>
#include <Rendering\Sprite.h>
#include <Utility\FileSystem.h>
#include <Utility\Path.h>

namespace Engine
{
	using namespace Utility;
	using namespace ResourceAssetParser;

	namespace ResourceSystem
	{
		using namespace Private;

		const WString ASSETS_DIRECTORY_NAME(L"Assets");
		const WString LIBRARY_DIRECTORY_NAME(L"Library");

		const WString INTERNAL_ASSETS_DIRECTORY_PATH(L"../Contents/Resources");
		const WString INTERNAL_LIBRARY_DIRECTORY_PATH(L"../Contents/Resources/Library");

		Shader* CreateShader(const ShaderInfo* ShaderInfo)
		{
			return RenderingManager::GetInstance()->GetActiveDevice()->CreateShader(ShaderInfo);
		}

		Mesh* CreateMesh(const SubMeshInfo* SubMeshInfo)
		{
			MeshInfo info(&ResourceSystemAllocators::ResourceAllocator);
			info.SubMeshes.Add(ConstCast(Rendering::SubMeshInfo*, SubMeshInfo));

			return RenderingManager::GetInstance()->GetActiveDevice()->CreateMesh(&info, GPUBuffer::Usages::StaticDraw);
		}

		SINGLETON_DEFINITION(ResourceManager)

			ResourceManager::ResourceManager(void) :
			ResourceHolder(Path::Combine(FileSystem::GetWorkingPath(), ASSETS_DIRECTORY_NAME), Path::Combine(FileSystem::GetWorkingPath(), LIBRARY_DIRECTORY_NAME)),
			m_InternalResourceHolder(Path::Combine(FileSystem::GetWorkingPath(), INTERNAL_ASSETS_DIRECTORY_PATH), Path::Combine(FileSystem::GetWorkingPath(), INTERNAL_LIBRARY_DIRECTORY_PATH))
		{
			CreateDefaultResources();
		}

		ResourceManager::~ResourceManager(void)
		{
		}

		TextureResource ResourceManager::GetWhiteTexture(void)
		{
			return GetLoaded<Texture>(BuiltInAssets::WHITE_TEXTURE_NAME);
		}

		SpriteResource ResourceManager::GetWhiteSprite(void)
		{
			return GetLoaded<Sprite>(BuiltInAssets::WHITE_TEXTURE_NAME);
		}

		ShaderResource ResourceManager::GetDefaultShader(void)
		{
			return GetLoaded<Shader>(BuiltInAssets::DEFAULT_SHADER_NAME);
		}

		ShaderResource ResourceManager::GetSpriteRendererShader(void)
		{
			return GetLoaded<Shader>(BuiltInAssets::SPRITE_RENDERER_SHADER_NAME);
		}

		MeshResource ResourceManager::GetPrimitiveMesh(PrimitiveMeshTypes Type)
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
				info.Format = Texture::Formats::RGBA8;
				Texture* tex = RenderingManager::GetInstance()->GetActiveDevice()->CreateTexture(&info);
				auto* buf = tex->GetBuffer();
				buf->Lock(GPUBuffer::Access::WriteOnly);
				buf->GetColorUI8Pixel() = ColorUI8::White;
				buf->Unlock();
				LoadFromMemory(BuiltInAssets::WHITE_TEXTURE_NAME, tex);
			}

			// Default Shader
			{
				ShaderInfo info;
				info.Source = BuiltInAssets::DEFAULT_SHADER_SOURCE;
				LoadFromMemory(BuiltInAssets::DEFAULT_SHADER_NAME, CreateShader(&info));
			}

			// Sprite Shader
			{
				ShaderInfo info;
				info.Source = BuiltInAssets::SPRITE_RENDERER_SHADER_SOURCE;
				LoadFromMemory(BuiltInAssets::SPRITE_RENDERER_SHADER_NAME, CreateShader(&info));
			}

			// Quad Mesh
			{
				SubMeshInfo* info = ReinterpretCast(SubMeshInfo*, AllocateMemory(&ResourceSystemAllocators::ResourceAllocator, sizeof(SubMeshInfo)));
				Construct(info, &ResourceSystemAllocators::ResourceAllocator);

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
				info->Layout = SubMesh::VertexLayouts::Position | SubMesh::VertexLayouts::UV;

				LoadFromMemory(BuiltInAssets::QUAD_MESH_NAME, CreateMesh(info));
			}

			// Cube Mesh
			{
				SubMeshInfo* info = ReinterpretCast(SubMeshInfo*, AllocateMemory(&ResourceSystemAllocators::ResourceAllocator, sizeof(SubMeshInfo)));
				Construct(info, &ResourceSystemAllocators::ResourceAllocator);

				OBJParser::Parse(ByteBuffer(ReinterpretCast(byte*, ConstCast(char8*, BuiltInAssets::CUBE_MESH_DATA.GetValue())), BuiltInAssets::CUBE_MESH_DATA.GetLength()), *info);
				LoadFromMemory(BuiltInAssets::CUBE_MESH_NAME, CreateMesh(info));
			}

			// Sphere Mesh
			{
				SubMeshInfo* info = ReinterpretCast(SubMeshInfo*, AllocateMemory(&ResourceSystemAllocators::ResourceAllocator, sizeof(SubMeshInfo)));
				Construct(info, &ResourceSystemAllocators::ResourceAllocator);

				OBJParser::Parse(ByteBuffer(ReinterpretCast(byte*, ConstCast(char8*, BuiltInAssets::SPHERE_MESH_DATA.GetValue())), BuiltInAssets::SPHERE_MESH_DATA.GetLength()), *info);
				LoadFromMemory(BuiltInAssets::SPHERE_MESH_NAME, CreateMesh(info));
			}

			// Cone Mesh
			{
				SubMeshInfo* info = ReinterpretCast(SubMeshInfo*, AllocateMemory(&ResourceSystemAllocators::ResourceAllocator, sizeof(SubMeshInfo)));
				Construct(info, &ResourceSystemAllocators::ResourceAllocator);

				OBJParser::Parse(ByteBuffer(ReinterpretCast(byte*, ConstCast(char8*, BuiltInAssets::CONE_MESH_DATA.GetValue())), BuiltInAssets::CONE_MESH_DATA.GetLength()), *info);
				LoadFromMemory(BuiltInAssets::CONE_MESH_NAME, CreateMesh(info));
			}
		}
	}
}